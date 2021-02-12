#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h>
#include <EEPROM.h>
#include <ESP8266WebServer.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h>

#define WEBSERVE_PORT   80
#define ARRAY_NUM       38
#define ARRAY_DAT_NUM   50
#define MAX_REC_LEN     (ARRAY_NUM)*(ARRAY_DAT_NUM)
#define SERIAL_RATES    115200
#define ERROR           -1
#define ID_EQUAL        0
char tmp_data[ARRAY_NUM][ARRAY_DAT_NUM];

WebSocketsClient webSocket;
#define WIFI_ON        true
#define WIFI_OFF       false
#define ON             1
#define OFF            0
#define SPACE          ' '
#define DELIM          ','
#define SSID_POS       0
#define PASSWORD_POS   1
#define APIKEY_POS     2
#define QNT_RELAY_POS  3
#define ALL_ON_POS     4
#define ALL_OFF_POS    5
#define RELAY_POS      6
#define NEED_CLEAR_MEM 0
#define APP_NAME       "KINCONY"
//---------------------------------------------------------------------------------------------------------------------------------------------
#define HEARTBEAT_INTERVAL      300000 // 5 Minutes  
uint64_t heartbeatTimestamp = 0;
bool isWebSocketConnected = false;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
const char* SET_RELAY  = "RELAY-SET-255,";
const char* ON_ALL     = "RELAY-AON-255,1,1";
const char* OFF_ALL    = "RELAY-AOF-255,1,1";
const char* appName    = APP_NAME ;
const IPAddress apIP(192, 168, 1, 1);
boolean settingMode = WIFI_OFF;
//char ssidList[1500];
const String initialForm = " <form method=\"get\" action=\"turnOnOff\">\
<label>Test Relay:</label> <input name=\"relay\" type=\"integer\" value=\"1\">\
<select name=\"value\" id=\"value\"><option value=\"ON\">ON</option><option value=\"OFF\">OFF</option></select><input type=\"submit\" value=\"Test\"></form>\
<form method=\"get\" action=\"turnOnOff\"><input name=\"value\" type=\"hidden\" value=\"AON\"><input type=\"submit\" value=\"ON ALL\"></form>\
<form method=\"get\" action=\"turnOnOff\"><input name=\"value\" type=\"hidden\" value=\"AOF\"><input type=\"submit\" value=\"OFF ALL\"></form>";

String formSettings = "";
DNSServer dnsServer;
ESP8266WebServer webServer(WEBSERVE_PORT);

void setup() {
  Serial.begin(SERIAL_RATES);
  delay(200);
  EEPROM.begin(MAX_REC_LEN);
  // Serial.println("setup");
  memset(tmp_data, 0, sizeof(tmp_data));
#if NEED_CLEAR_MEM
  clear_memory(true);
#endif
  pinMode(LED_BUILTIN, OUTPUT);
  delay(10);
  digitalWrite(LED_BUILTIN, LOW);
  if (restoreConfig()) {
    if (checkConnection()) {
      settingMode = WIFI_ON;
      generateHTMLVariable();
      startWebServer();
      startWebSocket();
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);
      return;
    }
  }
  settingMode = WIFI_OFF;
  setupMode();
  generateHTMLVariable();
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  if (settingMode == WIFI_OFF) {
    dnsServer.processNextRequest();
  } else {
    webSocket.loop();
    uint64_t now = millis();
    if (isWebSocketConnected) {
      if ((now - heartbeatTimestamp) > HEARTBEAT_INTERVAL) {
        heartbeatTimestamp = now;
        webSocket.sendTXT("H");
      }
    }
  }

  webServer.handleClient();
}

void generateHTMLVariable() {
  // Serial.println("initialForm " + String(initialForm.length()));

  char label[20];
  char html[200];
  char value[50];
  char extra[5];
  formSettings = "<form method=\"get\" action=\"setap\">" ;
  if (settingMode == WIFI_OFF) {
    formSettings += "<p>Please enter your password by selecting the SSID.</p>" ;
  } else {
    formSettings += "<p><a href=\"/resetWifi\">RESET WIFI</a></p>" ;
  }
  for (uint8_t i = 0; i < ARRAY_NUM; i++) {
    strcpy(extra, "");
    if (settingMode == WIFI_OFF) {
      strcpy(value, "");
    } else {
      strcpy(value,  tmp_data[i]  );
    }
    if ( i == SSID_POS ) {
      strcpy(label, "Wifi ID");
      //  if (settingMode == WIFI_ON) {
      //  continue ;
      //  }
      //  sprintf(html, "<label>SSID: </label><select name=\"%d\">" , i ) ;
      //  formSettings += String(html);
      //  formSettings += ssidList ;
      //  formSettings += "</select>";
      //  continue;
    } else if ( i == PASSWORD_POS ) {
      //  if (settingMode == WIFI_ON) {
      //  continue;
      //  }
      strcpy(label, "Wifi Password");
    } else if ( i == APIKEY_POS ) {
      strcpy(label, "Sinric API");
      strcpy(extra, "<br>");
    } else if ( i == QNT_RELAY_POS  ) {
      strcpy(label, "Relay's");
    } else if ( i == ALL_ON_POS  ) {
      strcpy(label, "ALL ON");
    } else if ( i == ALL_OFF_POS  ) {
      strcpy(label, "ALL OFF");
      strcpy(extra, "<br>");
    } else {
      int relay = i - RELAY_POS + 1;
      sprintf(label, "R-%02d" , relay  );
      if ( ( (i - RELAY_POS + 1 ) % 4  ) == 0 ) {
        strcpy(extra, "<br>");
      }
      if ( relay > String(tmp_data[QNT_RELAY_POS]).toInt()  && String(tmp_data[QNT_RELAY_POS]).toInt() > 1 ) {
        //  Serial.println("relay " + String(relay) + " = " +  tmp_data[QNT_RELAY_POS]);
        continue;
      }
    }
    sprintf(html, "<label>%s:</label><input name=\"%d\" type=\"text\" value=\"%s\">%s", label, i, value, extra  );
    formSettings += String(html );
  }
  formSettings += "<br><input type=\"submit\" value=\"Update\"></form>" ;
  formSettings += "<hr><p><a href=\"/reset\">RESET Modulo</a></p>" ;
  // Serial.println("formSettings " + String(formSettings.length()));
  // Serial.println("formSettings " + formSettings);
}
void clear_memory() {
  // Serial.println("clear_memory");
  int16_t i = 0;
  for ( i = 0; i < ARRAY_NUM; i++) {
    tmp_data[i][0] = 0;
  }
  for (i = 0; i < MAX_REC_LEN; i++) {
    EEPROM.write(i, 0x00);
  }
  EEPROM.commit();
}

boolean restoreConfig() {
  // Serial.println("restoreConfig");
  if (EEPROM.read(0) != 0 ) {
    int16_t i = 0;
    int16_t j = 0;
    for (i = 0; i < ARRAY_NUM; ++i) {
      tmp_data[i][0] = 0;
    }
    for (i = 0; i < ARRAY_NUM; ++i) {
      //  Serial.print("Info [" + String(i) + "] Initial [" + String(tmp_data[i]) + "]");
      for (j = 0; j < ARRAY_DAT_NUM; ++j) {
        tmp_data[i][j] = EEPROM.read(i * ARRAY_DAT_NUM + j );
      }
      //  Serial.println("After Read [" + String(tmp_data[i]) + "]");
    }
    //  Serial.println("Reading SSID [" + String( tmp_data[SSID_POS] ) + "] PASS [" + String( tmp_data[PASSWORD_POS] ) + "]...");
    if ( String(tmp_data[SSID_POS])  != "" && String(tmp_data[PASSWORD_POS])  != ""
         && String(tmp_data[SSID_POS]).length() <= 50 && String(tmp_data[PASSWORD_POS]).length() <= 50) {
      WiFi.begin(tmp_data[SSID_POS], tmp_data[PASSWORD_POS]);
      return true;
    } else if ( String(tmp_data[SSID_POS]).length() > 50 && String(tmp_data[PASSWORD_POS]).length() > 50) {
      clear_memory();
      Serial.println("Try again");
      //  return restoreConfig();
    }
  }
  Serial.println("Config not found.");
  return false;
}

boolean checkConnection() {
  uint8_t count = 0;
  Serial.print("Waiting for Wi-Fi connection ");
  while ( count < 15 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println( "Connected!");
      return (true);
    }
    delay(500);
    Serial.print(" . ");
    count++;
  }
  Serial.println("Timed out.");
  return false;
}

void startWebServer() {
  if (settingMode == WIFI_OFF) {
    Serial.print("Starting Web Server at [") ;
    Serial.print( WiFi.softAPIP());
    Serial.println("]");

    webServer.onNotFound([]() {
      String s = "<p><a href=\"/settings\">Wi-Fi Settings</a></p>";
      webServer.send(200, "text/html", makePage( " AP mode", s, false));
    });

  } else {

    Serial.print("Starting Web Server at [" ) ;
    Serial.print(WiFi.localIP()) ;
    Serial.println("]");

    webServer.on("/", []() {
      digitalWrite(LED_BUILTIN, LOW);
      String s = "<p>Sinric API [" + String(tmp_data[APIKEY_POS]).substring(0, 12) + "xxxx] Status ";
      if ( isWebSocketConnected ) {
        s += "[Connected]";
      } else {
        s += "[Disonnected]";
      }
      s += "</p>";
      s += initialForm ;

      s += "<p><a href=\"/settings\">Configure Relays</a></p>"; // Comment if want make it acess typing /settings only
      webServer.send(200, "text/html", makePage(" STA mode", s, false) );
      //  String str = String(htmlBody);
      digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off by making the voltage HIGH
      //  webServer.send(200, "text/html", makePage(" STA mode", str, false) );
    });

    webServer.on("/turnOnOff", []() {
      // Serial.println("Load Page '/relay' ");
      String value = urlDecode(webServer.arg("value"));
      String command = "";
      digitalWrite(LED_BUILTIN, LOW);  // Turn the LED off by making the voltage HIGH
      if (  value == "AON" ) {
        command = String(ON_ALL);
      } else if (  value == "AOF" ) {
        command = String(OFF_ALL);
      } else {
        String relay = urlDecode(webServer.arg("relay"));
        if (  value == "ON" ) {
          command = String(SET_RELAY) + relay + String(DELIM) + String(ON);
        } else {
          command = String(SET_RELAY) + relay + String(DELIM) + String(OFF) ;
        }
      }

      String s = "<p>Sinric API [" + String(tmp_data[APIKEY_POS]).substring(0, 12) + "xxxx] Status ";
      if ( isWebSocketConnected ) {
        s += "[Connected]";
      } else {
        s += "[Disonnected]";
      }
      s += "</p>";
      s += initialForm ;

      s += "<p>Esp send command [" + command + "].</p>";
      webServer.send(200, "text/html", makePage( " Test Relay", s, true));
      Serial.println(command);
      delay(200);
      digitalWrite(LED_BUILTIN, HIGH);   // Turn the LED off by making the voltage HIGH
    });
  }

  webServer.on("/settings", []() {
    // Serial.println("Load Page '/settings' ");
    digitalWrite(LED_BUILTIN, LOW);
    webServer.send(200, "text/html", makePage("Wi-Fi Settings", formSettings, settingMode));
    digitalWrite(LED_BUILTIN, HIGH);
  });

  webServer.on("/setap", []() {
    digitalWrite(LED_BUILTIN, LOW);  // Turn the LED off by making the voltage HIGH
    // Serial.println("Load Page '/setap' ");
    for (uint8_t i = 0; i < webServer.args(); ++i) {
      String value = urlDecode(webServer.arg(i));
      String param = webServer.argName(i);
      // Serial.println("Write EEPROM Arg [" + String(i) + "] Value [" + value + "]");
      if ( value == tmp_data[ param.toInt() ]) {
        continue;
      }
      for (uint8_t j = 0; j < ARRAY_DAT_NUM ; ++j) {
        if ( j > value.length()) {
          EEPROM.write(j + (ARRAY_DAT_NUM * param.toInt() ), 0x00);
        } else {
          EEPROM.write(j + (ARRAY_DAT_NUM * param.toInt() ), value[j]);
        }
      }
    }
    EEPROM.commit();
    // Serial.println("Write EEPROM done!");
    String s = "<p>Device will be connected to WIFI after the restart.";
    webServer.send(200, "text/html", makePage( " Wi-Fi Setup complete", s, true));
    WiFi.softAPdisconnect (true);
    digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
  });

  webServer.on("/reset", []() {
    //  Serial.println("Load Page '/reset' ");
    digitalWrite(LED_BUILTIN, LOW);
    String s = "<p>Esp will reset device.</p>";
    webServer.send(200, "text/html", makePage( " Reset Wi-Fi Settings", s, true));
    delay(1000);
    digitalWrite(LED_BUILTIN, HIGH);
    ESP.restart();
  });

  webServer.on("/resetWifi", []() {
    // Serial.println("Load Page '/resetWifi' ");
    digitalWrite(LED_BUILTIN, LOW);
    for (uint8_t i = 0; i < (2 * ARRAY_DAT_NUM); ++i) {
      EEPROM.write(i, 0x00);
    }
    EEPROM.commit();
    String s = "<p> Wi-Fi settings was reset. Esp will reset device.</p>";
    webServer.send(200, "text/html", makePage( " Reset Wi-Fi Settings", s, true));
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
    ESP.restart();
  });

  webServer.begin();
}

void startWebSocket() {
  // Serial.println("startWebSocket [" + String(tmp_data[APIKEY_POS]) + "]");
  if (String(tmp_data[APIKEY_POS]) != "" && String(tmp_data[APIKEY_POS]).length() == 36) {
    // server address, port and URL
    webSocket.begin("iot.sinric.com", 80, "/");
    // event handler
    webSocket.onEvent(webSocketEvent);
    webSocket.setAuthorization("apikey", tmp_data[APIKEY_POS]);
    // try again every 5000ms if connection has failed
    webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
  }
}

void setupMode() {
  // Serial.println("setupMode");
  //  WiFi.mode(WIFI_STA);
  //  WiFi.disconnect();
  //  delay(200);
  //  uint8_t n = WiFi.scanNetworks();
  //  delay(100);
  // Serial.println("... scanNetworks ...");
  //  for (uint8_t i = 0; i < n; ++i) {
  //  String wifi = WiFi.SSID(i) ;
  //  sprintf(ssidList, "%s<option value=\"%s\">%s</option>" , ssidList, wifi.c_str(), wifi.c_str() );
  //  }
  //  delay(200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  String APName = appName + String( millis() ).substring(0, 5) ;
  WiFi.softAP(APName);
  dnsServer.start(53, "*", apIP);
  startWebServer();
  Serial.println("Starting Access Point at [" + String(appName) +  String( millis() ).substring(0, 5) + "]");
}

String makePage(String title, String contents, boolean backButton) {
  // Serial.println("makePage [" + title + "] [" + String(contents.length() ) + "]" );
  char backButtonHTML[66];
  if ( backButton ) {
    strcpy( backButtonHTML , "<hr><p><a href=\"/\">Back</a></p>" );
  }

  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  char horario[50] = "";
  sprintf(horario, "<p>Uptime: %02d:%02d:%02d</p>" , hr, min % 60, sec % 60 );

  String titleFull  = String(appName) + " " + title;
  String s = "<!DOCTYPE html><html><head><style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style>\
            <meta name=\"viewport\" content=\"width=device-width,user-scalable=0\">\
            <title>"  + titleFull + "</title></head><body>"
             + "<h1>" + titleFull + "</h1>"
             + contents
             + String(backButtonHTML)
             + String( horario )
             + "</body></html>";
  // Serial.println("makePage [" + title + "] END " + s );
  return  s  ;
}

String urlDecode(String input) {
  String s = input;
  s.replace("%20", " ");
  s.replace("+", " ");
  s.replace("%21", "!");
  s.replace("%22", "\"");
  s.replace("%23", "#");
  s.replace("%24", "$");
  s.replace("%25", "%");
  s.replace("%26", "&");
  s.replace("%27", "\'");
  s.replace("%28", "(");
  s.replace("%29", ")");
  s.replace("%30", "*");
  s.replace("%31", "+");
  s.replace("%2C", ",");
  s.replace("%2E", ".");
  s.replace("%2F", "/");
  s.replace("%2C", ",");
  s.replace("%3A", ":");
  s.replace("%3A", ";");
  s.replace("%3C", "<");
  s.replace("%3D", "=");
  s.replace("%3E", ">");
  s.replace("%3F", "?");
  s.replace("%40", "@");
  s.replace("%5B", "[");
  s.replace("%5C", "\\");
  s.replace("%5D", "]");
  s.replace("%5E", "^");
  s.replace("%5F", "-");
  s.replace("%60", "`");
  return s;
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case WStype_DISCONNECTED:
      isWebSocketConnected = false;
      //Serial.printf("[WSc] Webservice disconnected from sinric.com!\n");
      break;
    case WStype_CONNECTED: {
        isWebSocketConnected = true;
        //  Serial.printf("[WSc] Service connected to sinric.com at url: %s\n", payload);
        //  Serial.printf("Waiting for commands from sinric.com ...\n");
      }
      break;
    case WStype_TEXT: {
        //  Serial.printf("[WSc] get text: %s\n", payload);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
        String deviceId = json ["deviceId"];
        String action = json ["action"];
        //  Serial.println("DeviceID [" + deviceId + "] action[" + action + "]");
        if (action == "setPowerState") { // Switch or Light
          // For Switch or Light device types
          // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html
          String value = json ["value"];
          // Serial.println("value [" + value + "]");
          if (0 == turnOnOff(deviceId, value)) {
            Serial.println("deviceId is not found");
          }
        } else  if (action == "action.devices.commands.OnOff") { // Switch
          // Example payloads
          // For Switch  types
          // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":true}} // https://developers.google.com/actions/smarthome/traits/onoff
          // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":false}}
          String value = json["value"]["on"];
          // Serial.println("value [" + value + "]");
          if (0 == turnOnOff(deviceId, value)) {
            Serial.println("deviceId is not found");
          }
        }
        else if (action == "test") {
          Serial.println("[WSc] received test command from sinric.com");
        }
      }
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
}


int32_t turnOnOff(String deviceId, String value) {
  int32_t i = 0;
  int32_t ret = 0;
  String command = "";
  digitalWrite(LED_BUILTIN, LOW);  // Turn the LED off by making the voltage HIGH
  for (i = ALL_ON_POS; i <= ARRAY_NUM; ++i) {
    if (ID_EQUAL == strcmp((char*)deviceId.c_str(), (char*)tmp_data[i] ) ) {
      int relay = i - RELAY_POS + 1;

      //  Serial.println("checkDeviceID: " + String( relay ));
      if ( value == "ON" || value == "true" ) { //Check if is to ON;
        command = String(SET_RELAY) + String(relay) + String(DELIM) + String(ON);
        if ( i == ALL_ON_POS ) {
          command = String(ON_ALL);
          // Serial.println("checkDeviceID: ON_ALL" );
        }
        if ( i == ALL_OFF_POS ) {
          command = "";
        }
      } else {
        command = String(SET_RELAY) + String(relay) + String(DELIM) + String(OFF) ;
        if ( i == ALL_OFF_POS ) {
          command = String(OFF_ALL);
          // Serial.println("checkDeviceID: OFF_ALL" );
        }
        if ( i == ALL_ON_POS ) {
          command = "";
        }
      }
      Serial.println(command);
      delay(500);
      digitalWrite(LED_BUILTIN, HIGH);  // Turn the LED off by making the voltage HIGH
      ret = 1;
      break;
    }
  }
  return ret;
}
