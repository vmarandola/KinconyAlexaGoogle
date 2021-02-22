#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <StreamString.h> 
#include <FS.h>
#include <LittleFS.h> //Need install from tools this Lib. and need be load file 'index.html' by menu 'Tools'->'ESP2866 LittleFS Data Upload'
#include <SimpleTimer.h>  
#include <SchedulerESP8266.h> //Some changes done into this Lib so use from zip.
#include <TimeLib.h> //Install https://www.arduinolibraries.info/libraries/time TIME 1.6 


//#define DEBUG ON   // UNCOMMENT it if you want debug mode.
#ifdef DEBUG
#define DEBUG_PRINT(x) Serial.print(x)
#define DEBUG_PRINTLN(x) Serial.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif 

#define WEBSERVE_PORT   80 
#define SERIAL_RATES    115200
//#define ERROR           -1
#define ID_EQUAL        0 
#define HEARTBEAT_INTERVAL      300000 // 5 Minutes


#define DELAY      200 
#define MAX_CONFIG 20 
#define MAX_RELAY 32
#define MAX_SCENE 12
#define MAX_NAME 20
#define MAX_DEVICE_ID 36
#define MAX_SCENE_RELAYS 17
#define MAX_VALUE 40
#define MAX_ALWAYS_ONOFF 4 
#define MAX_AUTO_CONFIG 14
#define MAX_CONFIG_TIME 30 

#define WIFI_ON         true
#define WIFI_OFF        false
#define ON              1
#define OFF             0 
#define DELIM           ','  

#define LOGIN_USER_POS  0
#define LOGIN_PASS_POS  1

#define WIFI_SSID_POS   2
#define WIFI_PASS_POS   3

#define CONTROLER_ID_POS 4
#define SIRINC_API_POS  5
#define TOTAL_RELAY_POS 6
#define TOTAL_SCENE_POS 7
#define UTC_POS         8
#define TIME_CHECK_POS  9

#define ALL_ON_POS      32
#define ALL_OFF_POS     33 
#define SCENE_POS       35

#define ALL_DAY   {true, true, true, true, true, true, true}
#define MON_FRI   {false, false, false, false, false, false, false}
#define WEEKEND   {true, false, false, false, false, false, true}

#define APP_NAME        "KINCONY"
#define SET_RELAY       "RELAY-SET-255,"
#define SET_REVERSE     "RELAY-KEY-255," 
#define SET_SCENE       "RELAY-SET_ALL-255,"
#define SET_ON_ALL      "RELAY-AON-255,1,1"
#define SET_OFF_ALL     "RELAY-AOF-255,1,1"

#define RELAY_ALL_ON() Serial.println(SET_ON_ALL)
#define RELAY_ALL_OFF() Serial.println(SET_OFF_ALL)
#define RELAY_ON(relay) Serial.println(String(SET_RELAY) + String(relay) + String(DELIM) + String(ON)) 
#define RELAY_OFF(relay) Serial.println(String(SET_RELAY) + String(relay) + String(DELIM) + String(OFF)) 
#define RELAY_REVERSE(relay) Serial.println(String(SET_REVERSE) + String(relay) + String(DELIM) + String(ON))
#define RELAY_SCENE(relays) Serial.println(String(SET_SCENE) + String(relays) ) 
#define LED_ON  digitalWrite(LED_BUILTIN, LOW)
#define LED_OFF  digitalWrite(LED_BUILTIN, HIGH)

char alwaysOnOff[MAX_ALWAYS_ONOFF];
char autoConfig[MAX_AUTO_CONFIG];
char configTime_[MAX_CONFIG_TIME];
char sceneRelay[MAX_SCENE_RELAYS];
typedef struct {
  char name[MAX_NAME];
  char deviceID[MAX_DEVICE_ID];
  char sceneRelay[MAX_SCENE_RELAYS];
  char alwaysOnOff[MAX_ALWAYS_ONOFF];
  char autoConfig[MAX_AUTO_CONFIG];
  char configTime01[MAX_CONFIG_TIME];
  char configTime02[MAX_CONFIG_TIME];
  char configTime03[MAX_CONFIG_TIME];
  char configTime04[MAX_CONFIG_TIME]; 
} RelayConfig;
RelayConfig relays[MAX_RELAY + 2 + MAX_SCENE];

char name[MAX_NAME];
char deviceID[MAX_DEVICE_ID];
char value[MAX_VALUE];

typedef struct { 
  char value[MAX_VALUE]; 
} Config;
Config configs[MAX_CONFIG]; 


void RELAY_ON_01() { DEBUG_PRINTLN("RELAY_ON_01"); delay(DELAY); RELAY_ON(1); delay(DELAY); } 
void RELAY_ON_02() { DEBUG_PRINTLN("RELAY_ON_02"); delay(DELAY); RELAY_ON(2); delay(DELAY); } 
void RELAY_ON_03() { DEBUG_PRINTLN("RELAY_ON_03"); delay(DELAY); RELAY_ON(3); delay(DELAY); } 
void RELAY_ON_04() { DEBUG_PRINTLN("RELAY_ON_04"); delay(DELAY); RELAY_ON(4); delay(DELAY); } 
void RELAY_ON_05() { DEBUG_PRINTLN("RELAY_ON_05"); delay(DELAY); RELAY_ON(5); delay(DELAY); } 
void RELAY_ON_06() { DEBUG_PRINTLN("RELAY_ON_06"); delay(DELAY); RELAY_ON(6); delay(DELAY); } 
void RELAY_ON_07() { DEBUG_PRINTLN("RELAY_ON_07"); delay(DELAY); RELAY_ON(7); delay(DELAY); } 
void RELAY_ON_08() { DEBUG_PRINTLN("RELAY_ON_08"); delay(DELAY); RELAY_ON(8); delay(DELAY); } 
void RELAY_ON_09() { DEBUG_PRINTLN("RELAY_ON_09"); delay(DELAY); RELAY_ON(9); delay(DELAY); } 
void RELAY_ON_10() { DEBUG_PRINTLN("RELAY_ON_10"); delay(DELAY); RELAY_ON(10); delay(DELAY); } 
void RELAY_ON_11() { DEBUG_PRINTLN("RELAY_ON_11"); delay(DELAY); RELAY_ON(11); delay(DELAY); } 
void RELAY_ON_12() { DEBUG_PRINTLN("RELAY_ON_12"); delay(DELAY); RELAY_ON(12); delay(DELAY); } 
void RELAY_ON_13() { DEBUG_PRINTLN("RELAY_ON_13"); delay(DELAY); RELAY_ON(13); delay(DELAY); } 
void RELAY_ON_14() { DEBUG_PRINTLN("RELAY_ON_14"); delay(DELAY); RELAY_ON(14); delay(DELAY); } 
void RELAY_ON_15() { DEBUG_PRINTLN("RELAY_ON_15"); delay(DELAY); RELAY_ON(15); delay(DELAY); } 
void RELAY_ON_16() { DEBUG_PRINTLN("RELAY_ON_16"); delay(DELAY); RELAY_ON(16); delay(DELAY); } 
void RELAY_ON_17() { DEBUG_PRINTLN("RELAY_ON_17"); delay(DELAY); RELAY_ON(17); delay(DELAY); } 
void RELAY_ON_18() { DEBUG_PRINTLN("RELAY_ON_18"); delay(DELAY); RELAY_ON(18); delay(DELAY); } 
void RELAY_ON_19() { DEBUG_PRINTLN("RELAY_ON_19"); delay(DELAY); RELAY_ON(19); delay(DELAY); } 
void RELAY_ON_20() { DEBUG_PRINTLN("RELAY_ON_20"); delay(DELAY); RELAY_ON(20); delay(DELAY); } 
void RELAY_ON_21() { DEBUG_PRINTLN("RELAY_ON_21"); delay(DELAY); RELAY_ON(21); delay(DELAY); } 
void RELAY_ON_22() { DEBUG_PRINTLN("RELAY_ON_22"); delay(DELAY); RELAY_ON(22); delay(DELAY); } 
void RELAY_ON_23() { DEBUG_PRINTLN("RELAY_ON_23"); delay(DELAY); RELAY_ON(23); delay(DELAY); } 
void RELAY_ON_24() { DEBUG_PRINTLN("RELAY_ON_24"); delay(DELAY); RELAY_ON(24); delay(DELAY); } 
void RELAY_ON_25() { DEBUG_PRINTLN("RELAY_ON_25"); delay(DELAY); RELAY_ON(25); delay(DELAY); } 
void RELAY_ON_26() { DEBUG_PRINTLN("RELAY_ON_26"); delay(DELAY); RELAY_ON(26); delay(DELAY); } 
void RELAY_ON_27() { DEBUG_PRINTLN("RELAY_ON_27"); delay(DELAY); RELAY_ON(27); delay(DELAY); } 
void RELAY_ON_28() { DEBUG_PRINTLN("RELAY_ON_28"); delay(DELAY); RELAY_ON(28); delay(DELAY); } 
void RELAY_ON_29() { DEBUG_PRINTLN("RELAY_ON_29"); delay(DELAY); RELAY_ON(29); delay(DELAY); } 
void RELAY_ON_30() { DEBUG_PRINTLN("RELAY_ON_30"); delay(DELAY); RELAY_ON(30); delay(DELAY); } 
void RELAY_ON_31() { DEBUG_PRINTLN("RELAY_ON_31"); delay(DELAY); RELAY_ON(31); delay(DELAY); } 
void RELAY_ON_32() { DEBUG_PRINTLN("RELAY_ON_32"); delay(DELAY); RELAY_ON(32); delay(DELAY); } 
void RELAY_ON_ALL()  { DEBUG_PRINTLN("RELAY_ON");  delay(DELAY); RELAY_ALL_ON(); delay(DELAY); } 
void RELAY_OFF_ALL() { DEBUG_PRINTLN("RELAY_OFF"); delay(DELAY); RELAY_ALL_OFF(); delay(DELAY); } 
void RELAY_ON_35() {DEBUG_PRINTLN("RELAY_ON_35"); delay(DELAY);  RELAY_SCENE(relays[34].sceneRelay); delay(DELAY); } 
void RELAY_ON_36() {DEBUG_PRINTLN("RELAY_ON_36"); delay(DELAY);  RELAY_SCENE(relays[35].sceneRelay); delay(DELAY); } 
void RELAY_ON_37() {DEBUG_PRINTLN("RELAY_ON_37"); delay(DELAY);  RELAY_SCENE(relays[36].sceneRelay); delay(DELAY); } 
void RELAY_ON_38() {DEBUG_PRINTLN("RELAY_ON_38"); delay(DELAY);  RELAY_SCENE(relays[37].sceneRelay); delay(DELAY); } 
void RELAY_ON_39() {DEBUG_PRINTLN("RELAY_ON_39"); delay(DELAY);  RELAY_SCENE(relays[38].sceneRelay); delay(DELAY); } 
void RELAY_ON_40() {DEBUG_PRINTLN("RELAY_ON_40"); delay(DELAY);  RELAY_SCENE(relays[39].sceneRelay); delay(DELAY); } 
void RELAY_ON_41() {DEBUG_PRINTLN("RELAY_ON_41"); delay(DELAY);  RELAY_SCENE(relays[40].sceneRelay); delay(DELAY); } 
void RELAY_ON_42() {DEBUG_PRINTLN("RELAY_ON_42"); delay(DELAY);  RELAY_SCENE(relays[41].sceneRelay); delay(DELAY); } 
void RELAY_ON_43() {DEBUG_PRINTLN("RELAY_ON_43"); delay(DELAY);  RELAY_SCENE(relays[42].sceneRelay); delay(DELAY); } 
void RELAY_ON_44() {DEBUG_PRINTLN("RELAY_ON_44"); delay(DELAY);  RELAY_SCENE(relays[43].sceneRelay); delay(DELAY); } 
void RELAY_ON_45() {DEBUG_PRINTLN("RELAY_ON_45"); delay(DELAY);  RELAY_SCENE(relays[44].sceneRelay); delay(DELAY); } 
void RELAY_ON_46() {DEBUG_PRINTLN("RELAY_ON_46"); delay(DELAY);  RELAY_SCENE(relays[45].sceneRelay); delay(DELAY); } 


       
void RELAY_OFF_01() { DEBUG_PRINTLN("RELAY_OFF_01"); delay(DELAY); RELAY_OFF(1); delay(DELAY); } 
void RELAY_OFF_02() { DEBUG_PRINTLN("RELAY_OFF_02"); delay(DELAY); RELAY_OFF(2); delay(DELAY); } 
void RELAY_OFF_03() { DEBUG_PRINTLN("RELAY_OFF_03"); delay(DELAY); RELAY_OFF(3); delay(DELAY); } 
void RELAY_OFF_04() { DEBUG_PRINTLN("RELAY_OFF_04"); delay(DELAY); RELAY_OFF(4); delay(DELAY); } 
void RELAY_OFF_05() { DEBUG_PRINTLN("RELAY_OFF_05"); delay(DELAY); RELAY_OFF(5); delay(DELAY); } 
void RELAY_OFF_06() { DEBUG_PRINTLN("RELAY_OFF_06"); delay(DELAY); RELAY_OFF(6); delay(DELAY); } 
void RELAY_OFF_07() { DEBUG_PRINTLN("RELAY_OFF_07"); delay(DELAY); RELAY_OFF(7); delay(DELAY); } 
void RELAY_OFF_08() { DEBUG_PRINTLN("RELAY_OFF_08"); delay(DELAY); RELAY_OFF(8); delay(DELAY); } 
void RELAY_OFF_09() { DEBUG_PRINTLN("RELAY_OFF_09"); delay(DELAY); RELAY_OFF(9); delay(DELAY); } 
void RELAY_OFF_10() { DEBUG_PRINTLN("RELAY_OFF_10"); delay(DELAY); RELAY_OFF(10); delay(DELAY); } 
void RELAY_OFF_11() { DEBUG_PRINTLN("RELAY_OFF_11"); delay(DELAY); RELAY_OFF(11); delay(DELAY); } 
void RELAY_OFF_12() { DEBUG_PRINTLN("RELAY_OFF_12"); delay(DELAY); RELAY_OFF(12); delay(DELAY); } 
void RELAY_OFF_13() { DEBUG_PRINTLN("RELAY_OFF_13"); delay(DELAY); RELAY_OFF(13); delay(DELAY); } 
void RELAY_OFF_14() { DEBUG_PRINTLN("RELAY_OFF_14"); delay(DELAY); RELAY_OFF(14); delay(DELAY); } 
void RELAY_OFF_15() { DEBUG_PRINTLN("RELAY_OFF_15"); delay(DELAY); RELAY_OFF(15); delay(DELAY); } 
void RELAY_OFF_16() { DEBUG_PRINTLN("RELAY_OFF_16"); delay(DELAY); RELAY_OFF(16); delay(DELAY); } 
void RELAY_OFF_17() { DEBUG_PRINTLN("RELAY_OFF_17"); delay(DELAY); RELAY_OFF(17); delay(DELAY); } 
void RELAY_OFF_18() { DEBUG_PRINTLN("RELAY_OFF_18"); delay(DELAY); RELAY_OFF(18); delay(DELAY); } 
void RELAY_OFF_19() { DEBUG_PRINTLN("RELAY_OFF_19"); delay(DELAY); RELAY_OFF(19); delay(DELAY); } 
void RELAY_OFF_20() { DEBUG_PRINTLN("RELAY_OFF_20"); delay(DELAY); RELAY_OFF(20); delay(DELAY); } 
void RELAY_OFF_21() { DEBUG_PRINTLN("RELAY_OFF_21"); delay(DELAY); RELAY_OFF(21); delay(DELAY); } 
void RELAY_OFF_22() { DEBUG_PRINTLN("RELAY_OFF_22"); delay(DELAY); RELAY_OFF(22); delay(DELAY); } 
void RELAY_OFF_23() { DEBUG_PRINTLN("RELAY_OFF_23"); delay(DELAY); RELAY_OFF(23); delay(DELAY); } 
void RELAY_OFF_24() { DEBUG_PRINTLN("RELAY_OFF_24"); delay(DELAY); RELAY_OFF(24); delay(DELAY); } 
void RELAY_OFF_25() { DEBUG_PRINTLN("RELAY_OFF_25"); delay(DELAY); RELAY_OFF(25); delay(DELAY); } 
void RELAY_OFF_26() { DEBUG_PRINTLN("RELAY_OFF_26"); delay(DELAY); RELAY_OFF(26); delay(DELAY); } 
void RELAY_OFF_27() { DEBUG_PRINTLN("RELAY_OFF_27"); delay(DELAY); RELAY_OFF(27); delay(DELAY); } 
void RELAY_OFF_28() { DEBUG_PRINTLN("RELAY_OFF_28"); delay(DELAY); RELAY_OFF(28); delay(DELAY); } 
void RELAY_OFF_29() { DEBUG_PRINTLN("RELAY_OFF_29"); delay(DELAY); RELAY_OFF(29); delay(DELAY); } 
void RELAY_OFF_30() { DEBUG_PRINTLN("RELAY_OFF_30"); delay(DELAY); RELAY_OFF(30); delay(DELAY); } 
void RELAY_OFF_31() { DEBUG_PRINTLN("RELAY_OFF_31"); delay(DELAY); RELAY_OFF(31); delay(DELAY); } 
void RELAY_OFF_32() { DEBUG_PRINTLN("RELAY_OFF_32"); delay(DELAY); RELAY_OFF(32); delay(DELAY); } 

void RELAY_REVERSE_01() {DEBUG_PRINTLN("RELAY_REVERSE_01"); delay(DELAY);  RELAY_REVERSE(1); delay(DELAY); } 
void RELAY_REVERSE_02() {DEBUG_PRINTLN("RELAY_REVERSE_02"); delay(DELAY);  RELAY_REVERSE(2); delay(DELAY); } 
void RELAY_REVERSE_03() {DEBUG_PRINTLN("RELAY_REVERSE_03"); delay(DELAY);  RELAY_REVERSE(3); delay(DELAY); } 
void RELAY_REVERSE_04() {DEBUG_PRINTLN("RELAY_REVERSE_04"); delay(DELAY);  RELAY_REVERSE(4); delay(DELAY); } 
void RELAY_REVERSE_05() {DEBUG_PRINTLN("RELAY_REVERSE_05"); delay(DELAY);  RELAY_REVERSE(5); delay(DELAY); } 
void RELAY_REVERSE_06() {DEBUG_PRINTLN("RELAY_REVERSE_06"); delay(DELAY);  RELAY_REVERSE(6); delay(DELAY); } 
void RELAY_REVERSE_07() {DEBUG_PRINTLN("RELAY_REVERSE_07"); delay(DELAY);  RELAY_REVERSE(7); delay(DELAY); } 
void RELAY_REVERSE_08() {DEBUG_PRINTLN("RELAY_REVERSE_08"); delay(DELAY);  RELAY_REVERSE(8); delay(DELAY); } 
void RELAY_REVERSE_09() {DEBUG_PRINTLN("RELAY_REVERSE_09"); delay(DELAY);  RELAY_REVERSE(9); delay(DELAY); } 
void RELAY_REVERSE_10() {DEBUG_PRINTLN("RELAY_REVERSE_10"); delay(DELAY);  RELAY_REVERSE(10); delay(DELAY); } 
void RELAY_REVERSE_11() {DEBUG_PRINTLN("RELAY_REVERSE_11"); delay(DELAY);  RELAY_REVERSE(11); delay(DELAY); } 
void RELAY_REVERSE_12() {DEBUG_PRINTLN("RELAY_REVERSE_12"); delay(DELAY);  RELAY_REVERSE(12); delay(DELAY); } 
void RELAY_REVERSE_13() {DEBUG_PRINTLN("RELAY_REVERSE_13"); delay(DELAY);  RELAY_REVERSE(13); delay(DELAY); } 
void RELAY_REVERSE_14() {DEBUG_PRINTLN("RELAY_REVERSE_14"); delay(DELAY);  RELAY_REVERSE(14); delay(DELAY); } 
void RELAY_REVERSE_15() {DEBUG_PRINTLN("RELAY_REVERSE_15"); delay(DELAY);  RELAY_REVERSE(15); delay(DELAY); } 
void RELAY_REVERSE_16() {DEBUG_PRINTLN("RELAY_REVERSE_16"); delay(DELAY);  RELAY_REVERSE(16); delay(DELAY); } 
void RELAY_REVERSE_17() {DEBUG_PRINTLN("RELAY_REVERSE_17"); delay(DELAY);  RELAY_REVERSE(17); delay(DELAY); } 
void RELAY_REVERSE_18() {DEBUG_PRINTLN("RELAY_REVERSE_18"); delay(DELAY);  RELAY_REVERSE(18); delay(DELAY); } 
void RELAY_REVERSE_19() {DEBUG_PRINTLN("RELAY_REVERSE_19"); delay(DELAY);  RELAY_REVERSE(19); delay(DELAY); } 
void RELAY_REVERSE_20() {DEBUG_PRINTLN("RELAY_REVERSE_20"); delay(DELAY);  RELAY_REVERSE(20); delay(DELAY); } 
void RELAY_REVERSE_21() {DEBUG_PRINTLN("RELAY_REVERSE_21"); delay(DELAY);  RELAY_REVERSE(21); delay(DELAY); } 
void RELAY_REVERSE_22() {DEBUG_PRINTLN("RELAY_REVERSE_22"); delay(DELAY);  RELAY_REVERSE(22); delay(DELAY); } 
void RELAY_REVERSE_23() {DEBUG_PRINTLN("RELAY_REVERSE_23"); delay(DELAY);  RELAY_REVERSE(23); delay(DELAY); } 
void RELAY_REVERSE_24() {DEBUG_PRINTLN("RELAY_REVERSE_24"); delay(DELAY);  RELAY_REVERSE(24); delay(DELAY); } 
void RELAY_REVERSE_25() {DEBUG_PRINTLN("RELAY_REVERSE_25"); delay(DELAY);  RELAY_REVERSE(25); delay(DELAY); } 
void RELAY_REVERSE_26() {DEBUG_PRINTLN("RELAY_REVERSE_26"); delay(DELAY);  RELAY_REVERSE(26); delay(DELAY); } 
void RELAY_REVERSE_27() {DEBUG_PRINTLN("RELAY_REVERSE_27"); delay(DELAY);  RELAY_REVERSE(27); delay(DELAY); } 
void RELAY_REVERSE_28() {DEBUG_PRINTLN("RELAY_REVERSE_28"); delay(DELAY);  RELAY_REVERSE(28); delay(DELAY); } 
void RELAY_REVERSE_29() {DEBUG_PRINTLN("RELAY_REVERSE_29"); delay(DELAY);  RELAY_REVERSE(29); delay(DELAY); } 
void RELAY_REVERSE_30() {DEBUG_PRINTLN("RELAY_REVERSE_30"); delay(DELAY);  RELAY_REVERSE(30); delay(DELAY); } 
void RELAY_REVERSE_31() {DEBUG_PRINTLN("RELAY_REVERSE_31"); delay(DELAY);  RELAY_REVERSE(31); delay(DELAY); } 
void RELAY_REVERSE_32() {DEBUG_PRINTLN("RELAY_REVERSE_32"); delay(DELAY);  RELAY_REVERSE(32); delay(DELAY); } 
 
typedef void (*func_command)(void);
const func_command commandON[46] = {
  RELAY_ON_01,RELAY_ON_02,RELAY_ON_03,RELAY_ON_04,RELAY_ON_05,RELAY_ON_06,RELAY_ON_07,RELAY_ON_08,
  RELAY_ON_09,RELAY_ON_10,RELAY_ON_11,RELAY_ON_12,RELAY_ON_13,RELAY_ON_14,RELAY_ON_15,RELAY_ON_16,
  RELAY_ON_17,RELAY_ON_18,RELAY_ON_19,RELAY_ON_20,RELAY_ON_21,RELAY_ON_22,RELAY_ON_23,RELAY_ON_24,
  RELAY_ON_25,RELAY_ON_26,RELAY_ON_27,RELAY_ON_28,RELAY_ON_29,RELAY_ON_30,RELAY_ON_31,RELAY_ON_32,
  RELAY_ON_ALL,RELAY_OFF_ALL,
  RELAY_ON_35,RELAY_ON_36,RELAY_ON_37,RELAY_ON_38,RELAY_ON_39,RELAY_ON_40,RELAY_ON_41,
  RELAY_ON_42,RELAY_ON_43,RELAY_ON_44,RELAY_ON_45,RELAY_ON_46
}; 
const func_command commandOFF[34] = { 
  RELAY_OFF_01,RELAY_OFF_02,RELAY_OFF_03,RELAY_OFF_04,RELAY_OFF_05,RELAY_OFF_06,RELAY_OFF_07,RELAY_OFF_08,
  RELAY_OFF_09,RELAY_OFF_10,RELAY_OFF_11,RELAY_OFF_12,RELAY_OFF_13,RELAY_OFF_14,RELAY_OFF_15,RELAY_OFF_16,
  RELAY_OFF_17,RELAY_OFF_18,RELAY_OFF_19,RELAY_OFF_20,RELAY_OFF_21,RELAY_OFF_22,RELAY_OFF_23,RELAY_OFF_24,
  RELAY_OFF_25,RELAY_OFF_26,RELAY_OFF_27,RELAY_OFF_28,RELAY_OFF_29,RELAY_OFF_30,RELAY_OFF_31,RELAY_OFF_32,
  RELAY_ON_ALL,RELAY_OFF_ALL
};
const func_command commandREVERSE[34] = { 
  RELAY_REVERSE_01,RELAY_REVERSE_02,RELAY_REVERSE_03,RELAY_REVERSE_04,RELAY_REVERSE_05,RELAY_REVERSE_06,RELAY_REVERSE_07,RELAY_REVERSE_08,
  RELAY_REVERSE_09,RELAY_REVERSE_10,RELAY_REVERSE_11,RELAY_REVERSE_12,RELAY_REVERSE_13,RELAY_REVERSE_14,RELAY_REVERSE_15,RELAY_REVERSE_16,
  RELAY_REVERSE_17,RELAY_REVERSE_18,RELAY_REVERSE_19,RELAY_REVERSE_20,RELAY_REVERSE_21,RELAY_REVERSE_22,RELAY_REVERSE_23,RELAY_REVERSE_24,
  RELAY_REVERSE_25,RELAY_REVERSE_26,RELAY_REVERSE_27,RELAY_REVERSE_28,RELAY_REVERSE_29,RELAY_REVERSE_30,RELAY_REVERSE_31,RELAY_REVERSE_32,
  RELAY_ON_ALL,RELAY_OFF_ALL
};  

uint64_t heartbeatTimestamp = 0;
bool isWebSocketConnected = false;
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
const char* appName    = APP_NAME ;
const IPAddress apIP(192, 168, 1, 1);
boolean settingMode = WIFI_OFF;
boolean isLoadScheduler = false;

SimpleTimer timer;
Scheduler scheduler; 
WebSocketsClient webSocket; 
DNSServer dnsServer;
ESP8266WebServer webServer(WEBSERVE_PORT);

void setup() {
#define ROUTINE "setup"
  DEBUG_PRINTLN(ROUTINE);
  Serial.begin(SERIAL_RATES); 
  delay(500);
  LittleFS.begin();
  delay(500);   
  pinMode(LED_BUILTIN, OUTPUT);
  LED_ON;
  if (loadConfig() && checkConnection()) {
    settingMode = WIFI_ON;
    startWebServer();
    startWebSocket();
    int timerCheck = String( configs[TIME_CHECK_POS].value).toInt();
    if(  timerCheck >= 5 ) {
      DEBUG_PRINTLN(" Timer Check Always [Enabled]");
      timer.setInterval(timerCheck * 1000, checkAlwaysRelay);  
    } 
  } else {  
    settingMode = WIFI_OFF;
    setupMode();
  }
  LED_OFF;
  DEBUG_PRINTLN(" End");   
#undef ROUTINE
}
int incomingByte = 0; // for incoming serial data

void loop() {
  if (settingMode == WIFI_OFF) {
    dnsServer.processNextRequest();
  } else {
    if (Serial.available()) {
      Serial.print("I received: ");
      // read the incoming byte:
      while (Serial.available()) {
         incomingByte = Serial.read();
         Serial.print(incomingByte, HEX);
         Serial.print(",");
         if (!Serial.available()) delayMicroseconds(1200);
      }
      Serial.println();
    }
    
    if( isLoadScheduler ) {     
      scheduler.update();
    }
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
  timer.run();
}

String getValue(String data, char separator, int index) {
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
} 

boolean checkConnection() {
#define ROUTINE "checkConnection"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON;
  uint8_t count = 0;
  Serial.print("Waiting for Wi-Fi connection ");
  while ( count < 15 ) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println( "Connected!");
      return (true);
    }
    LED_OFF;
    delay(1000);
    LED_ON;
    Serial.print(" . ");
    count++;
  }
  Serial.println("Timed out.");
  DEBUG_PRINTLN(" End");
  LED_OFF;
  return false;
#undef ROUTINE
}

void startWebServer() {
#define ROUTINE "startWebServer"
  DEBUG_PRINTLN(ROUTINE);
  webServer.serveStatic("/", LittleFS, "/index.html");
  webServer.serveStatic("/dump/config.bin", LittleFS, "/config.bin");
  webServer.serveStatic("/dump/relays.bin", LittleFS, "/relays.bin");
  if (settingMode == WIFI_OFF) {
    Serial.print("Starting Web Server at [") ;
    Serial.print( WiFi.softAPIP());
    Serial.println("]");
  } else {
    Serial.print("Starting Web Server at [" ) ;
    Serial.print(WiFi.localIP()) ;
    Serial.println("]");
  }

  webServer.on("/login", HTTP_POST, onLogin);
  webServer.on("/clear", HTTP_POST, onClear); 
  webServer.on("/updateSecurity", HTTP_POST, onUpdateSecurity);
  webServer.on("/updateWifi", HTTP_POST, onUpdateWifi);
  webServer.on("/updateModule", HTTP_POST, onUpdateModule);
  webServer.on("/updateRelay", HTTP_POST, onUpdateRelay);
  webServer.on("/updateRelayName", HTTP_POST, onUpdateRelayName);
  webServer.on("/updateRelayConfig", HTTP_POST, onUpdateRelayConfig);
  webServer.on("/updateScene", HTTP_POST, onUpdateScene);
  webServer.on("/refresh", HTTP_POST, onRefresh);
  webServer.on("/restart", HTTP_POST, onRestart);
  webServer.on("/testRelayOffAll", HTTP_POST, onTestRelayOffAll);
  webServer.on("/testRelayOnAll", HTTP_POST, onTestRelayOnAll);
  webServer.on("/testRelay", HTTP_POST, onTestRelay);
  webServer.on("/testScene", HTTP_POST, onTestScene);

  webServer.onNotFound(onNotFound);
  //Inicializa o nosso servidor
  webServer.begin();
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}

void startWebSocket() {
#define ROUTINE "startWebSocket"
  DEBUG_PRINTLN(ROUTINE);
  String  SinricAPI = configs[SIRINC_API_POS].value; 
  DEBUG_PRINTLN("SinricAPI [" + SinricAPI + "]");
  if (SinricAPI != "") {
    // server address, port and URL
    webSocket.begin("iot.sinric.com", 80, "/");
    // event handler
    webSocket.onEvent(webSocketEvent);
    webSocket.setAuthorization("apikey", configs[SIRINC_API_POS].value);
    // try again every 5000ms if connection has failed
    webSocket.setReconnectInterval(5000);   // If you see 'class WebSocketsClient' has no member named 'setReconnectInterval' error update arduinoWebSockets
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
 
void setupMode() {
#define ROUTINE "setupMode"
  DEBUG_PRINTLN(ROUTINE);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  String APName = appName + String( millis() ).substring(0, 5) ;
  WiFi.softAP(APName);
  dnsServer.start(53, "*", apIP);
  startWebServer();
  Serial.println("Starting Access Point at [" + String(appName) +  String( millis() ).substring(0, 5) + "]");
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}

void checkAlwaysRelay() {
#define ROUTINE "checkAlwaysRelay"
  DEBUG_PRINTLN(ROUTINE);
    for (int relay = 0; relay < MAX_RELAY + 2 + MAX_SCENE; ++relay) { 
      if( String( relays[relay].alwaysOnOff ) == "ON" ) {
        commandON[relay]();
      } else if ( String(  relays[relay ].alwaysOnOff ) == "OFF"  ) {
        commandOFF[relay ]();
      } 
    }    
    DEBUG_PRINTLN(" End");
#undef ROUTINE
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
#define ROUTINE "webSocketEvent Receive"
        DEBUG_PRINTLN(ROUTINE);
        LED_ON;  // Turn the LED off by making the voltage HIGH
        //  Serial.printf("[WSc] get text: %s\n", payload);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject((char*)payload);
        String deviceId = json ["deviceId"];
        String action = json ["action"];
        String value = "";
        int32_t relay = 0;

#ifdef DEBUG
        DEBUG_PRINT("deviceId [");
        DEBUG_PRINT(deviceId);
        DEBUG_PRINT("] action [");
        DEBUG_PRINT(action);
#endif        
        if (action == "test") {
          DEBUG_PRINTLN("received test command from sinric.com");
          Serial.println("[WSc] received test command from sinric.com");
          break;
        } else if (action == "setPowerState") { // Switch or Light
          // For Switch or Light device types
          // {"deviceId": xxxx, "action": "setPowerState", value: "ON"} // https://developer.amazon.com/docs/device-apis/alexa-powercontroller.html
          String valueJson = json ["value"]  ; 
          value = valueJson;
#ifdef DEBUG
          DEBUG_PRINTLN("] value [" + value + "] Alexa Action . . . ");
#endif        
        } else  if (action == "action.devices.commands.OnOff") { // Switch
          // Example payloads
          // For Switch  types
          // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":true}} // https://developers.google.com/actions/smarthome/traits/onoff
          // {"deviceId":"xxx","action":"action.devices.commands.OnOff","value":{"on":false}}
          String valueJson =   json["value"]["on"]  ;
          value = valueJson;
#ifdef DEBUG
          DEBUG_PRINTLN("] value [" + value + "] Google Action . . . "); 
#endif        
        }
        relay = turnOnOff(deviceId, value); 
        if (relay != 0 && relay < SCENE_POS) {
          checkRelayConfiguration(relay, value);
        } else {
          DEBUG_PRINTLN(" deviceId is not found");
        }
        delay(500);
        LED_OFF;  // Turn the LED off by making the voltage HIGH

      }
#undef ROUTINE
      break;
    case WStype_BIN:
      Serial.printf("[WSc] get binary length: %u\n", length);
      break;
  }
} 
void checkRelayConfiguration(int32_t relay, String value) {
#define ROUTINE "checkRelayConfiguration"
  DEBUG_PRINTLN(ROUTINE);  
      DEBUG_PRINTLN(" relay [" + String(relay) + "] value [" + value + "]" ); 
  if ( value == "ON" || value == "true" ) { 
    //SET TIMEOUT TO TURN OFF.
    int timeOff = getValue( relays[relay - 1 ].autoConfig, ',', 0).toInt();
      DEBUG_PRINTLN(" autoConfig OFF [" + String(timeOff) + "]" );
    if ( timeOff > 0 ) {
      timer.setTimeout(timeOff * 1000, commandOFF[relay - 1]);  
    } 
  }else {  
    //SET TIMEOUT TO TURN ON. 
    int timeOn = getValue( relays[relay - 1 ].autoConfig, ',', 1).toInt();
      DEBUG_PRINTLN(" autoConfig ON [" + String(timeOn) + "]" ); 
    if ( timeOn > 0 ) { 
      timer.setTimeout(timeOn * 1000, commandON[relay - 1 ]); 
    }
  }
  //SET TIMEOUT TO TURN REVERSE.
  int timeReverse = getValue( relays[relay - 1 ].autoConfig, ',', 2).toInt();
    DEBUG_PRINTLN(" autoConfig REVERSE [" + String(timeReverse) + "]" ); 
  if ( timeReverse > 0 ) {
    timer.setTimeout(timeReverse * 1000, commandREVERSE[relay - 1 ]); 
  }  
  DEBUG_PRINTLN(" checkRelayConfiguration End");
#undef ROUTINE
}

int32_t turnOnOff(String deviceId, String value) {
#define ROUTINE "turnOnOff"
  DEBUG_PRINTLN(ROUTINE);
  int32_t i = 0; 
  int32_t relay = 0;
  if ( deviceId.length() <= 2 ) {
    relay = deviceId.toInt();
  } else { 
    for (i = 0; i < MAX_RELAY + 2 + MAX_SCENE; ++i) { 
      if (ID_EQUAL == strcmp((char*)deviceId.c_str(), (char*)relays[i].deviceID )) {
//        if ( i == ALL_ON_POS ) {
//          delay(DELAY);
//          RELAY_ALL_ON();
//          delay(DELAY);
//        } else if ( i == ALL_OFF_POS ) { 
//          delay(DELAY);
//          RELAY_ALL_OFF();
//          delay(DELAY);
//        }
        relay = i + 1;
        break;
      }
    }
  }
  if( relay == 0 ) {
    return relay;
  }
  
  String tipo = "relay";
  if ( relay >= SCENE_POS ) {
    tipo = "scene";
  }
#ifdef DEBUG
  DEBUG_PRINT(" deviceId found " + tipo + "value [" + value + "][" + relay + "] Send command: ... "); 
#endif 
  if ( value == "REV") {
    commandREVERSE[relay-1](); 
  } else if ( relay >= SCENE_POS || value == "ON" || value == "true" ) {
    commandON[relay-1]();
  } else { 
    commandOFF[relay-1]();
  }  
  return relay;
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}

void onGetConfig() {
#define ROUTINE "onGetConfig"
  DEBUG_PRINTLN(ROUTINE);
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  char uptime[50] = "";
  sprintf(uptime, "%02d:%02d:%02d" , hr, min % 60, sec % 60 ); 
 
  String sinricSocketStatus = "Disonnected";
  if ( isWebSocketConnected ) {
    sinricSocketStatus = "Connected";
  }

  String json = "{\"count\":" + String(MAX_CONFIG);
  json += ",\"status\":{";
  json += "\"time\":\"" +  scheduler.getTime()  + "\"" ;
  json += ",\"uptime\":\"" + String( uptime ) + "\"" ;
  json += ",\"sinricSocketStatus\":\"" + sinricSocketStatus + "\"" ;
  json += ",\"sizeSecheduler\":\"" + String( scheduler.getSize() ) + "\"" ; 
  json += "},";
  json += "\"position\":{";
  json += "\"info\":\"" + String(WIFI_SSID_POS) + "|";  
  json += String(WIFI_PASS_POS) + "|";
  json += String(CONTROLER_ID_POS) + "|";
  json += String(SIRINC_API_POS) + "|";
  json += String(TOTAL_RELAY_POS) + "|";
  json += String(TOTAL_SCENE_POS) + "|";
  json += String(UTC_POS) + "|";   
  json += String(TIME_CHECK_POS) + "|";    
  json += String(ALL_ON_POS) + "|";
  json += String(ALL_OFF_POS) + "|"; 
  json += String(SCENE_POS) + "|";    
      
  json += "\"},";

  json += "\"configs\":[";
  for (int i = 0; i < MAX_CONFIG; i++) {
    json += "{";
    json += "\"info\":\"" + String(i) + "|"; 
    if ( i != LOGIN_USER_POS && i != LOGIN_PASS_POS ) {
      json += String(configs[i].value);  
    }
    json +=   "\""; 
    json += "},"; 
  }
  json += "]!,";
  json.replace(",]!", "]");
  json += "\"configsRelay\":[";
  for (int i = 0; i < MAX_RELAY + 2 + MAX_SCENE; i++) {
    json += "{";
    json += "\"info\":\"" + String(i) + "|"; 
    json += String(relays[i].name) + "|";
    json += String(relays[i].deviceID) + "|";
    json += String(relays[i].sceneRelay) + "|"; 
    json += String(relays[i].alwaysOnOff) + "|";
    json += String(relays[i].autoConfig) + "|";
    json += String(relays[i].configTime01) + "|";
    json += String(relays[i].configTime02) + "|";
    json += String(relays[i].configTime03) + "|";
    json += String(relays[i].configTime04) + "|"; 
    json += "\"},";
  }
  json += "]}!";
  json.replace(",]}!", "]}");
  webServer.send(200, "text/json", json);
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}

void saveConfig() {
#define ROUTINE "saveConfig"
  DEBUG_PRINTLN(ROUTINE);
  File f = LittleFS.open("/config.bin", "w");
  f.write((uint8_t *)configs, sizeof(Config)*MAX_CONFIG);
  f.close();

  f = LittleFS.open("/relays.bin", "w");
  f.write((uint8_t *)relays, sizeof(RelayConfig) * (MAX_RELAY + 2 + MAX_SCENE));
  f.close();
  DEBUG_PRINTLN(" saveConfig End");
#undef ROUTINE
}
bool getBool(char c) {
    if (c == '1') {
        return true;
    } else {
        return false;
    }
}

void addScheduleRelay( String status , String name, bool days[7],tmElements_t te,int relay) {
#define ROUTINE "addScheduleRelay"
//  DEBUG_PRINTLN(ROUTINE); 
  DEBUG_PRINTLN("status [" + status + "] name[" + name + "] relay[" + String(relay) + "]"); 
  int unixTime = makeTime(te);
  unixTime += ( String(configs[UTC_POS].value).toInt() * -1 ) * 60 * 60;
  if ( relay >= SCENE_POS - 2) {
    if ( status == "ON" ) {
      ScheduleTask task = ScheduleTask(name, days, unixTime, commandON[relay] ) ;  
      scheduler.add(task); 
    } 
  } else {
    if ( status == "ON" ) {
      ScheduleTask task = ScheduleTask(name, days, unixTime, commandON[relay] ) ;  
      scheduler.add(task); 
    } else if ( status == "OFF" ) {
      ScheduleTask task = ScheduleTask(name, days, unixTime, commandOFF[relay]) ; 
      scheduler.add(task); 
    } else if ( status == "REV" ) { 
      ScheduleTask task = ScheduleTask(name, days, unixTime, commandREVERSE[relay]);
      scheduler.add(task); 
    }
  }
  DEBUG_PRINTLN("addScheduleRelay End"); 
#undef ROUTINE
}
bool addScheduler( String configTimeInfo, String nameTimeConfig, int relay ) { 
//#define ROUTINE "addScheduler"
//  DEBUG_PRINTLN(ROUTINE); 
    bool ret = false;
//  DEBUG_PRINTLN("configTimeInfo [" + configTimeInfo + "] nameTimeConfig[" + nameTimeConfig + "] relay[" + String(relay) + "]"); 
  if (  String(configTimeInfo).charAt(0) == '1' 
      ||  String(configTimeInfo).charAt(0) == '0') 
    {   
      String specifyDays = getValue( configTimeInfo, ',', 0);
      bool days[7] = { getBool(specifyDays.charAt(0)), 
                getBool(specifyDays.charAt(1)), 
                getBool(specifyDays.charAt(2)), 
                getBool(specifyDays.charAt(3)), 
                getBool(specifyDays.charAt(4)), 
                getBool(specifyDays.charAt(5)), 
                getBool(specifyDays.charAt(6)) };
      String statusInfo = getValue( configTimeInfo, ',', 1);
      
      tmElements_t te;  te.Year = 0; te.Month = 1; te.Day = 1; te.Second = 0; 
      te.Hour   = getValue( configTimeInfo , ',', 2).toInt(); 
      te.Minute = getValue( configTimeInfo, ',', 3).toInt(); 
      DEBUG_PRINTLN("specifyDays [" + specifyDays + "] statusInfo [" + getValue( statusInfo, '-', 0) + "] te.Hour[" + String(te.Hour) + "] te.Minute [" + String(te.Minute) + "]");   
      addScheduleRelay( getValue( statusInfo, '-', 0), "Start " + nameTimeConfig, days, te ,relay); 

      te.Hour   = getValue( configTimeInfo, ',', 4).toInt(); 
      te.Minute = getValue( configTimeInfo, ',', 5).toInt();
      DEBUG_PRINTLN("specifyDays [" + specifyDays + "] statusInfo [" + getValue( statusInfo, '-', 1) + "] te.Hour[" + String(te.Hour) + "] te.Minute [" + String(te.Minute) + "]");   
      addScheduleRelay( getValue( statusInfo, '-', 1), "End " + nameTimeConfig, days, te , relay);  
      ret = true; 
    }
//    DEBUG_PRINTLN("addScheduler End"); 
    return ret;
//#undef ROUTINE
}
 
void loadConfigScheduler() {
#define ROUTINE "loadConfigScheduler"
  DEBUG_PRINTLN(ROUTINE); 
  String nameTask = ""; 
  for (int relay = 0; relay < MAX_RELAY + 2 + MAX_SCENE; relay++ ) { 
    nameTask =  String( relay ) + " [" + relays[relay].name + "] "; 
    isLoadScheduler = addScheduler( relays[relay].configTime01, nameTask + "Time 01",  relay ) || isLoadScheduler ;  
    isLoadScheduler = addScheduler( relays[relay].configTime02, nameTask + "Time 02",  relay ) || isLoadScheduler ;  
    isLoadScheduler = addScheduler( relays[relay].configTime03, nameTask + "Time 03",  relay ) || isLoadScheduler ;  
    isLoadScheduler = addScheduler( relays[relay].configTime04, nameTask + "Time 04",  relay ) || isLoadScheduler ;  
  }
  if ( isLoadScheduler ) {
    scheduler.begin();
    scheduler.setUTC(String(configs[UTC_POS].value).toInt());
  }
  DEBUG_PRINTLN("loadConfigScheduler End"); 
#undef ROUTINE
}

boolean loadConfig() {
#define ROUTINE "loadConfig"
  DEBUG_PRINTLN(ROUTINE);
  DEBUG_PRINTLN("CONFIG ------- ");
  boolean ret; 
  File f;
  if ((f = LittleFS.open("/config.bin", "r")) != 0) {
    f.read((uint8_t *)configs, sizeof(Config)*MAX_CONFIG);  
    f.close();
    for (int i = 0; i < MAX_CONFIG; i++ ) {
#ifdef DEBUG
      DEBUG_PRINT("CONFIG [");
      DEBUG_PRINT(i);  
      DEBUG_PRINT("] value [");
      DEBUG_PRINT(configs[i].value);
      DEBUG_PRINTLN("]");
#endif
    }
    WiFi.begin(configs[WIFI_SSID_POS].value, configs[WIFI_PASS_POS].value); 
    ret = true;
  } else {
    DEBUG_PRINTLN(" Not Found File config.bin"); 
    ret = false;
  } 
  
  DEBUG_PRINTLN("RELAYS CONFIG");
  if ((f = LittleFS.open("/relays.bin", "r")) != 0) {
    f.read((uint8_t *)relays, sizeof(RelayConfig) * (MAX_RELAY + 2 + MAX_SCENE));
    for (int i = 0; i < MAX_RELAY + 2 + MAX_SCENE; i++ ) { 
#ifdef DEBUG
      DEBUG_PRINT("RELAYS ");
      DEBUG_PRINT(i); 
      DEBUG_PRINT(" name [");
      DEBUG_PRINT(relays[i].name); 
      DEBUG_PRINT("] deviceID [");
      DEBUG_PRINT(relays[i].deviceID);
      DEBUG_PRINT("] sceneRelay [");
      DEBUG_PRINT(relays[i].sceneRelay);
      DEBUG_PRINT("] alwaysOnOff [");
      DEBUG_PRINT(relays[i].alwaysOnOff);
      DEBUG_PRINT("] autoConfig [");
      DEBUG_PRINT(relays[i].autoConfig);
      DEBUG_PRINT("] configTime01 [");
      DEBUG_PRINT(relays[i].configTime01);
      DEBUG_PRINT("] configTime02 [");
      DEBUG_PRINT(relays[i].configTime02);
      DEBUG_PRINT("] configTime03 [");
      DEBUG_PRINT(relays[i].configTime03);
      DEBUG_PRINT("] configTime04 [");
      DEBUG_PRINT(relays[i].configTime04); 
      DEBUG_PRINTLN("]");
#endif
    }
    f.close(); 
    loadConfigScheduler();
  } else {
    DEBUG_PRINTLN(" Not Found File relays.bin");
  }
  if ( ret == false ) { 
    String("admin").toCharArray(value, MAX_VALUE); strcpy(configs[LOGIN_USER_POS].value, value);
    String("admin").toCharArray(value, MAX_VALUE); strcpy(configs[LOGIN_PASS_POS].value, value);
    String("0").toCharArray(value, MAX_VALUE); strcpy(configs[UTC_POS].value, value);
    String("2").toCharArray(value, MAX_VALUE); strcpy(configs[TOTAL_RELAY_POS].value, value);
    String("2").toCharArray(value, MAX_VALUE); strcpy(configs[TOTAL_SCENE_POS].value, value); 
    saveConfig();
    ret = false;
  }
  DEBUG_PRINTLN(" End");
  return ret;
#undef ROUTINE
}

boolean onClear() { 
#define ROUTINE "onClear"
  DEBUG_PRINTLN(ROUTINE);

  if (webServer.hasArg("1")) {
    String controllerID = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" controllerID " + controllerID); 

    if ( controllerID == String(configs[CONTROLER_ID_POS].value)) {
      LittleFS.remove("/config.bin");
      LittleFS.remove("/relays.bin");  
      webServer.send(200, "text/plain", "OK");
      delay(500);
      ESP.restart();
    } else { 
      webServer.send(401, "text/plain", "Wrong controllerID");
      DEBUG_PRINTLN(" Wrong Username/Password");    } 
  } else {
    badRequest();
  }
#undef ROUTINE
}

void onNotFound() {
#define ROUTINE "onNotFound"
  DEBUG_PRINTLN(ROUTINE);
  webServer.send(404, "text/plain", "Not Found");
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void badRequest() {
  DEBUG_PRINTLN("-Bad Request - Missing Parameters");
  webServer.send(400, "text/plain", "Bad Request - Missing Parameters");
}
void onLogin() {
#define ROUTINE "onLogin"
  DEBUG_PRINTLN(ROUTINE);
  if ( settingMode == WIFI_OFF ) {
    onGetConfig();
    DEBUG_PRINTLN(" Success");
    return ;
  }
  if (webServer.hasArg("1") && webServer.hasArg("2")) {
    String user = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" user " + user); 
    String password = urlDecode(webServer.arg("2")); DEBUG_PRINTLN(" password " + password);

    if ( ( user == "admin" && password == configs[CONTROLER_ID_POS].value )
         || ( user ==  configs[LOGIN_USER_POS].value  && password ==  configs[LOGIN_PASS_POS].value ) ) {
      onGetConfig();
      DEBUG_PRINTLN(" Success");
    } else {
      webServer.send(401, "text/plain", "Wrong Username/Password");
      DEBUG_PRINTLN(" Wrong Username/Password");
    }
  } else {
    badRequest();
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onUpdateSecurity() {
#define ROUTINE "onUpdateSecurity"
  DEBUG_PRINTLN(ROUTINE);

  if (webServer.hasArg("1") && webServer.hasArg("2")) {
    String user = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" user " + user + " SAVE -> " + String(LOGIN_USER_POS) );
    user.toCharArray(value, MAX_VALUE); strcpy(configs[LOGIN_USER_POS].value, value);

    String password = urlDecode(webServer.arg("2"));  DEBUG_PRINTLN(" password " + password + " SAVE -> " + String(LOGIN_PASS_POS ));
    password.toCharArray(value, MAX_VALUE); strcpy(configs[LOGIN_PASS_POS].value, value);

    webServer.send(200, "text/plain", "OK");
    saveConfig();
  } else {
    badRequest();
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onUpdateWifi() {
#define ROUTINE "onUpdateWifi"
  DEBUG_PRINTLN(ROUTINE);

  if (webServer.hasArg("1") && webServer.hasArg("2")) {
    String user = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" user " + user + " SAVE -> " + String(WIFI_SSID_POS) );
    user.toCharArray(value, MAX_VALUE); strcpy(configs[WIFI_SSID_POS].value, value);

    String password = urlDecode(webServer.arg("2")); DEBUG_PRINTLN(" password " + password + " SAVE -> " + String(WIFI_PASS_POS) );
    password.toCharArray(value, MAX_VALUE); strcpy(configs[WIFI_PASS_POS].value, value);
    
    webServer.send(200, "text/plain", "OK");
    saveConfig();
  } else {
    badRequest();
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onUpdateModule() {
#define ROUTINE "onUpdateModule"
  DEBUG_PRINTLN(ROUTINE);

  if (webServer.hasArg("1") && webServer.hasArg("4")) {
    String ControlerID = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" ControlerID " + ControlerID + " SAVE -> " + String(CONTROLER_ID_POS ));
    ControlerID.toCharArray(value, MAX_VALUE); strcpy(configs[CONTROLER_ID_POS].value, value);

    String Utc = urlDecode(webServer.arg("2")); DEBUG_PRINTLN(" Utc " + Utc + " SAVE -> " + String(UTC_POS ));
    Utc.toCharArray(value, MAX_VALUE); strcpy(configs[UTC_POS].value, value);
    scheduler.setUTC(String(configs[UTC_POS].value).toInt());
    
    String TimeCheck = urlDecode(webServer.arg("3")); DEBUG_PRINTLN(" TimeCheck " + Utc + " SAVE -> " + String(TIME_CHECK_POS ));
    TimeCheck.toCharArray(value, MAX_VALUE); strcpy(configs[TIME_CHECK_POS].value, value); 
     
    String SinricAPI = urlDecode(webServer.arg("4")); 
    SinricAPI.toCharArray(value, MAX_VALUE); DEBUG_PRINTLN(" SinricAPI " + SinricAPI + " SAVE -> " + String(SIRINC_API_POS ) + " [" + value + "]" );
    strcpy(configs[SIRINC_API_POS].value, value);   

    String Relays = urlDecode(webServer.arg("5")); DEBUG_PRINTLN(" Relays " + Relays + " SAVE -> " + String(TOTAL_RELAY_POS) );
    Relays.toCharArray(value, MAX_VALUE); strcpy(configs[TOTAL_RELAY_POS].value, value);

    String Scenes = urlDecode(webServer.arg("6")); DEBUG_PRINTLN(" Scenes " + Scenes + " SAVE -> " + String(TOTAL_SCENE_POS ));
    Scenes.toCharArray(value, MAX_VALUE); strcpy(configs[TOTAL_SCENE_POS].value, value); 

    webServer.send(200, "text/plain", "OK");
    saveConfig();
  } else {
    badRequest();
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onUpdateRelay() {
#define ROUTINE "onUpdateRelay"
  DEBUG_PRINTLN(ROUTINE);
  int relayPosition = 0 ;
  if (webServer.hasArg("1") && webServer.hasArg("34") && webServer.hasArg("33")) { 
    for ( int i = 1; i <= MAX_RELAY + 2; i++ ) { 
      relayPosition = i - 1 ;
      if (webServer.hasArg(String(i)) ) {
        String Relays = urlDecode(webServer.arg(String(i))); DEBUG_PRINTLN(" Relays " + String(i) + ": " + Relays  + " SAVE -> " + String(relayPosition));
        Relays.toCharArray(deviceID, MAX_DEVICE_ID); strcpy(relays[relayPosition].deviceID, deviceID);

      }
    }

    webServer.send(200, "text/plain", "OK");
    saveConfig();
  } else {
    badRequest();
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}

void onUpdateRelayName() {
#define ROUTINE "onUpdateRelay"
  DEBUG_PRINTLN(ROUTINE);
  int relayPosition = 0;
  if (webServer.hasArg("1")) { 
    for ( int i = 1; i <= 32; i++ ) {  
      relayPosition = i - 1 ;
      if (webServer.hasArg(String(i)) ) {
        String Relays = urlDecode(webServer.arg(String(i))); DEBUG_PRINTLN(" Relays Name " + String(i) + ": " + Relays  + " SAVE -> " + String(relayPosition));
        Relays.toCharArray(name, MAX_NAME); strcpy(relays[relayPosition].name, name);
      }
    }

    webServer.send(200, "text/plain", "OK");
    saveConfig();
  } else {
    badRequest();
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onUpdateScene() {
#define ROUTINE "onUpdateScene"
  DEBUG_PRINTLN(ROUTINE);

  if (webServer.hasArg("1") && webServer.hasArg("2")) {

    int scenePosition = SCENE_POS;
    int argDeviceID = -1;
    int argRelaysID = 0;
    for ( int i = 1; i <= 12 ; i++ ) {
      scenePosition = MAX_RELAY + 2 + i - 1;
      argDeviceID += 2;
      if (webServer.hasArg(String(argDeviceID)) ) {
        String SceneDeviceID = urlDecode(webServer.arg(String(argDeviceID))); DEBUG_PRINTLN(" SceneDeviceID "  + String(argDeviceID) + ": " + SceneDeviceID + " SAVE -> " + String(scenePosition) );
        SceneDeviceID.toCharArray(deviceID, MAX_DEVICE_ID); strcpy(relays[scenePosition].deviceID, deviceID);
      }

      argRelaysID += 2;
      if (webServer.hasArg(String(argRelaysID)) ) {
        String sceneRelayArgs = urlDecode(webServer.arg(String(argRelaysID))); DEBUG_PRINTLN(" SceneRelays "  + String(argRelaysID) + ": " + sceneRelayArgs + " SAVE -> " + String(scenePosition) );
        sceneRelayArgs.toCharArray(sceneRelay, MAX_VALUE); strcpy(relays[scenePosition].sceneRelay, sceneRelay); 
      }
    }

    webServer.send(200, "text/plain", "OK");
    saveConfig();
  } else {
    badRequest();
  }
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onRefresh() {
#define ROUTINE "onRefresh"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON;
  loadConfig();
  onGetConfig();
  delay(500);
  LED_OFF;
#undef ROUTINE
}
void onRestart() {
#define ROUTINE "onRestart"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON;
  webServer.send(200, "text/plain", "Restart ESP");
  delay(1000);
  LED_OFF;
  DEBUG_PRINTLN(" End");
#undef ROUTINE
  ESP.restart();
}

void onTestRelayOnAll() {
#define ROUTINE "onTestRelayOnAll"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON;
  if (webServer.hasArg("1") ) {
    Serial.println(SET_ON_ALL);
    delay(500);
    webServer.send(200, "text/plain", "OK");
  } else {
    badRequest();
  }
  LED_OFF;
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onTestRelayOffAll() {
#define ROUTINE "onTestRelayOffAll"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON;
  if (webServer.hasArg("1")) {
    Serial.println(SET_OFF_ALL);
    delay(500);
    webServer.send(200, "text/plain", "OK");
  } else {
    badRequest();
  }
  LED_OFF;
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}

void onTestRelay() {
#define ROUTINE "onTestRelay"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON; 
  if (webServer.hasArg("1") && webServer.hasArg("2")) {
    String TestRelay = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" TestRelay " + TestRelay);
    String TestStatus = urlDecode(webServer.arg("2")); DEBUG_PRINTLN(" TestStatus " + TestStatus);
    int32_t relay = 0;
    relay = turnOnOff(TestRelay, TestStatus);
    if (relay != 0 && relay < SCENE_POS) {
      checkRelayConfiguration(relay, TestStatus);
    } else {
      DEBUG_PRINTLN(" deviceId is not found");
    }  
    webServer.send(200, "text/plain", "OK");
  } else {
    badRequest();
  }
  LED_OFF;
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onTestScene() {
#define ROUTINE "onTestScene"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON;
  if (webServer.hasArg("1") ) {
    String TestScene = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" TestScene " + TestScene);

    String command_relay = String(SET_SCENE) + TestScene ;

    Serial.println(command_relay);
    delay(500);

    webServer.send(200, "text/plain", "OK");
  } else {
    badRequest();
  }
  LED_OFF;
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
void onUpdateRelayConfig() {
#define ROUTINE "onUpdateRelayConfig"
  DEBUG_PRINTLN(ROUTINE);
  LED_ON;
  if (webServer.hasArg("1") && webServer.hasArg("10") ) {

    String configRelaySelected = urlDecode(webServer.arg("1")); DEBUG_PRINTLN(" configRelaySelected " + configRelaySelected);
    String relayName = urlDecode(webServer.arg("2")); DEBUG_PRINTLN(" relayName " + relayName);
    String relayDeviceID = urlDecode(webServer.arg("3")); DEBUG_PRINTLN(" relayDeviceID " + relayDeviceID);
    String sceneRelayArgs = urlDecode(webServer.arg("4")); DEBUG_PRINTLN(" SceneRelay " + sceneRelayArgs);
    String alwaysOnOffArgs = urlDecode(webServer.arg("5")); DEBUG_PRINTLN(" AlwaysOnOff " + alwaysOnOffArgs);
    String autoConfigArgs = urlDecode(webServer.arg("6")); DEBUG_PRINTLN(" AutoConfig " + autoConfigArgs);
    String configTime01 = urlDecode(webServer.arg("7")); DEBUG_PRINTLN(" configTime01 " + configTime01);
    String configTime02 = urlDecode(webServer.arg("8")); DEBUG_PRINTLN(" configTime02 " + configTime02);
    String configTime03 = urlDecode(webServer.arg("9")); DEBUG_PRINTLN(" configTime03 " + configTime03);
    String configTime04 = urlDecode(webServer.arg("10")); DEBUG_PRINTLN(" configTime04 " + configTime04); 

    int relayPosition = configRelaySelected.toInt() - 1 ; 
    relayName.toCharArray(name, MAX_NAME); strcpy(relays[relayPosition].name, name);
    relayDeviceID.toCharArray(deviceID, MAX_DEVICE_ID); strcpy(relays[relayPosition].deviceID, deviceID);
    sceneRelayArgs.toCharArray(sceneRelay, MAX_SCENE_RELAYS); strcpy(relays[relayPosition].sceneRelay, sceneRelay);
    alwaysOnOffArgs.toCharArray(alwaysOnOff, MAX_ALWAYS_ONOFF); strcpy(relays[relayPosition].alwaysOnOff, alwaysOnOff);
    autoConfigArgs.toCharArray(autoConfig, MAX_AUTO_CONFIG); strcpy(relays[relayPosition].autoConfig, autoConfig);
 
    configTime01.toCharArray(configTime_, MAX_CONFIG_TIME); strcpy(relays[relayPosition].configTime01, configTime_);
    configTime02.toCharArray(configTime_, MAX_CONFIG_TIME); strcpy(relays[relayPosition].configTime02, configTime_);
    configTime03.toCharArray(configTime_, MAX_CONFIG_TIME); strcpy(relays[relayPosition].configTime03, configTime_);
    configTime04.toCharArray(configTime_, MAX_CONFIG_TIME); strcpy(relays[relayPosition].configTime04, configTime_);

    webServer.send(200, "text/plain", "OK");
    saveConfig();
  } else {
    badRequest();
  }
  LED_OFF;
  DEBUG_PRINTLN(" End");
#undef ROUTINE
}
