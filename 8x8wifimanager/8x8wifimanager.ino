#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <WebSocketsClient.h>
//#include <Hash.h>
#include <U8g2lib.h>

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <ESP8266httpUpdate.h>

//for LED status
#include <Ticker.h>
Ticker ticker;

#define USE_SERIAL Serial

#define DEBUG
#define DEBUG_ESP_PORT Serial

const char* app_version = "0.0.3";

WebSocketsClient webSocket;
//U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ D2, /* data=*/ D4, /* cs=*/ D3, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);

int contrast = 64;
boolean isButtonPressed = false;
boolean displayOn = true;
long lastUpdateMillis = 0;
int lastPrice = -1;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void displayPrice(String value, int position) {
  USE_SERIAL.printf("[DISPLAY] PRINTING %s to %i\n", value.c_str(), position);
  u8g2.clearBuffer();
  u8g2.drawStr(4,position,value.c_str());
  //u8g2.drawStr(1,position+8,value.c_str());
  u8g2.sendBuffer();
}

void displayRefresh(int last) {
  USE_SERIAL.printf("[DISPLAY] REFRESHED\n");
  if (displayOn) {

    if (lastPrice == -1) {
      USE_SERIAL.printf("[DISPLAY] INIT PRICE\n");
      displayPrice(String(last).c_str(),8);
    } else { if (lastPrice < last) {
      USE_SERIAL.printf("[DISPLAY] PRICE UP\n");
      for (int i=lastPrice; i <= last; i++){
        //for (int r=1; r <= 8; r++){
         // USE_SERIAL.printf("[DISPLAY] DISPLAY PRICE UP\n");
          displayPrice(String(i), 8);
          delay(100);
        //}
       }
    } else {
      USE_SERIAL.printf("[DISPLAY] PRICE DOWN\n");
      for (int i=lastPrice; i >= last; i--){
        //for (int r=8; r >= 8; r--){
          //USE_SERIAL.printf("[DISPLAY] DISPLAY PRICE DOWN\n");
          displayPrice(String(i), 8);
          delay(100);
        //}
      }
    }
    //u8g2.drawUTF8(0, 16, "B"); //""₿");
    }
  }
}

void displayClear() {
  USE_SERIAL.printf("[DISPLAY] CLEARED\n");
  u8g2.clearBuffer();
  u8g2.sendBuffer();
}

void displayFlash() {
  USE_SERIAL.printf("[DISPLAY] FLASHING\n");
  u8g2.setContrast(100);
  delay(100);
  u8g2.setContrast(255);
  delay(100);
  u8g2.setContrast(1007);
  delay(100);
  u8g2.setContrast(contrast);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {


    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            {
                USE_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
            }
            break;
        case WStype_TEXT:
            {
              USE_SERIAL.printf("[WSc] get text: %s\n", payload);
              String str = (char*)payload;
              int last = str.toInt();
              USE_SERIAL.printf("[WSc] get tick: %s\n", String(last).c_str());
              boolean up_direction;
              if (lastPrice!=last) {
                displayRefresh(last);
                lastPrice = last;
                //displayFlash();
                tick();delay(20);tick();
              }
            }
            break;
        case WStype_BIN:
            USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
            hexdump(payload, length);
            break;
    }

}

void update_firmware() {
  t_httpUpdate_return ret = ESPhttpUpdate.update("10.0.1.48", 4567, "/esp/update", app_version);
switch(ret) {
    case HTTP_UPDATE_FAILED:
        Serial.println("[update] Update failed.");
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  USE_SERIAL.println("Entered config mode");
  USE_SERIAL.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  USE_SERIAL.println(myWiFiManager->getConfigPortalSSID());

  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);

  for (int r=1; r <= 250; r++){
    u8g2.clearBuffer();
    u8g2.drawStr(32-r,8,"PLEASE CONNECT TO AP");
    u8g2.sendBuffer();
    delay(40);
  }

    u8g2.clearBuffer();
    u8g2.drawStr(4,8,"WIFI");
    u8g2.sendBuffer();
}

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_profont12_mf);
  u8g2.setContrast(contrast);
        u8g2.clearBuffer();
    //u8g2.drawUTF8(0, 16, "B"); //""₿");
      u8g2.drawStr(1,1,"bitix");
    u8g2.sendBuffer();

  USE_SERIAL.begin(115200);
  USE_SERIAL.setDebugOutput(true);

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  //reset settings - for testing
  //wifiManager.resetSettings();

  WiFiManagerParameter pair("pair", "Currency pair", "BTCUSD", 10);
  wifiManager.addParameter(&pair);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect()) {
    USE_SERIAL.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  USE_SERIAL.println("connected...yeey :)");
  ticker.detach();

  update_firmware();

  //keep LED off
  digitalWrite(BUILTIN_LED, false);

  webSocket.begin("139.59.138.189", 8081, strcat("/",pair.getValue()));
  //webSocket.begin("10.0.1.48", 7000, "/");
  webSocket.onEvent(webSocketEvent);
}

void loop() {
    webSocket.loop();
}
