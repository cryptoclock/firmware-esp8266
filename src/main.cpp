#include <Arduino.h>
#include <Hash.h>
#include <Wire.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <WebSocketsClient.h>
//#include <Hash.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include "firmware.h"
#include "display.h"

//for LED status
#include <Ticker.h>
Ticker ticker;

#define PORTAL_TRIGGER_PIN 0 // Flash button
#define USE_SERIAL Serial

#define DEBUG
#define DEBUG_ESP_PORT Serial

const char* app_version = "0.1.3";

WebSocketsClient webSocket;

//U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ D2, /* data=*/ D4, /* cs=*/ D3, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
TM1637Display tm1637(/* clock=*/ D0, /* data=*/ D1);
Display display(u8g2, tm1637, USE_SERIAL);

int contrast = 64;
boolean isButtonPressed = false;
long lastUpdateMillis = 0;
int lastPrice = -1;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
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
              int currentPrice = str.toInt();
              USE_SERIAL.printf("[WSc] get tick: %i\n", currentPrice);
              boolean up_direction;
              if (lastPrice!=currentPrice) {
                display.refreshPrice(lastPrice, currentPrice);
                lastPrice = currentPrice;
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

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  USE_SERIAL.println("Entered config mode");
  USE_SERIAL.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  USE_SERIAL.println(myWiFiManager->getConfigPortalSSID());

  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);

  display.displayRotate("PLEASE CONNECT TO AP",200);
  display.displayText("WIFI", 4);
}

void setup() {
  display.setContrast(contrast);
  display.displayText("bitix");
  delay(5000);

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
  WiFiManagerParameter update_server("update_server", "Update server", "10.0.1.48:4567", 20);
  wifiManager.addParameter(&update_server);

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

  display.displayText("UPDATE");
  update_firmware();

  //keep LED off
  digitalWrite(BUILTIN_LED, false);

  display.displayText(app_version);
  delay(3000);
  display.displayText(ESP.getSketchMD5());
  delay(3000);
  display.displayText(pair.getValue());
  delay(3000);

  webSocket.begin("139.59.138.189", 8081, String("/") + pair.getValue());
  //webSocket.begin("10.0.1.48", 7000, "/");
  webSocket.onEvent(webSocketEvent);

  pinMode(PORTAL_TRIGGER_PIN, INPUT);
}

void loop() {
    webSocket.loop();
    if ( digitalRead(PORTAL_TRIGGER_PIN) == LOW ) {
      USE_SERIAL.println("Starting portal");
      WiFiManager wifiManager;
      wifiManager.setTimeout(120);

      // FIXME: DRY, values
      WiFiManagerParameter pair("pair", "Currency pair", "BTCUSD", 10);
      wifiManager.addParameter(&pair);
      WiFiManagerParameter update_server("update_server", "Update server", "10.0.1.48:4567", 20);
      wifiManager.addParameter(&update_server);

      if (!wifiManager.startConfigPortal("OnDemandAP")) {
        USE_SERIAL.println("failed to connect and hit timeout");
        delay(3000);
        //reset and try again, or maybe put it to deep sleep
        ESP.reset();
        delay(5000);
      }
      //if you get here you have connected to the WiFi
      USE_SERIAL.println("connected...yeey :)");
    }

}
