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

#include "config.hpp"

#include "firmware.hpp"
#include "display.hpp"
#include "wifi.hpp"

//for LED status
#include <Ticker.h>
Ticker ticker;

#include <EEPROM.h>

WebSocketsClient webSocket;

//U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ D2, /* data=*/ D4, /* cs=*/ D3, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
U8G2_MAX7219_32X8_F_4W_SW_SPI u8g2(U8G2_R2, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
TM1637Display tm1637(/* clock=*/ D0, /* data=*/ D1);

Display display(u8g2, tm1637);
AP_list *APs;
WiFiCore *wifi;

long lastUpdateMillis = 0;
int lastPrice = -1;

long buttonTimer = 0;
long longPressTime = 3000;
boolean buttonActive = false;
boolean longPressActive = false;

void tick()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      DEBUG_SERIAL.printf("[WSc] Disconnected! %s\n",  payload);
      break;
    case WStype_CONNECTED:
      DEBUG_SERIAL.printf("[WSc] Connected to url: %s\n",  payload);
      break;
    case WStype_TEXT:
      {
        DEBUG_SERIAL.printf("[WSc] get text: %s\n", payload);
        String str = (char*)payload;
        int currentPrice = str.toInt();
        DEBUG_SERIAL.printf("[WSc] get tick: %i\n", currentPrice);
        if (lastPrice==currentPrice) {
          display.blinkDot();
        } else {
          display.refreshPrice(lastPrice, currentPrice);
          lastPrice = currentPrice;
          //tick();delay(20);tick();
        }
      }
      break;
    case WStype_BIN:
      DEBUG_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);
      break;
  }
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager) {
  DEBUG_SERIAL.println("Entered config mode");
  DEBUG_SERIAL.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  DEBUG_SERIAL.println(myWiFiManager->getConfigPortalSSID());

  //entered config mode, make led toggle faster
  ticker.attach(0.2, tick);

  display.displayRotate("PLEASE CONNECT TO AP",200);
  display.displayText("WIFI", 4);
}

void setup() {
  DEBUG_SERIAL.begin(115200);
  DEBUG_SERIAL.setDebugOutput(true);

  display.setContrast(contrast);
  // display.displayText("bitix");

  APs = new AP_list();
//  APs->storeToEEPROM(); // wipe eeprom
  wifi = new WiFiCore(display, APs);

  display.displayText(wifi->getCurrencyPair());
  // delay(5000);

  uint8_t value;

  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.5 because we start in AP mode and try to connect
  ticker.attach(0.6, tick);

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifi->setAPCallback(configModeCallback);
  wifi->connectToWiFiOrFallbackToAP();

  //if you get here you have connected to the WiFi
  DEBUG_SERIAL.println("connected...yeey :)");
  ticker.detach();

  display.displayText("UPDATING");
  update_firmware();

  //keep LED off
  digitalWrite(BUILTIN_LED, false);

  display.displayText(app_version);
  delay(3000);
  display.displayText(ESP.getSketchMD5());
  delay(3000);
  display.displayText(wifi->getCurrencyPair());
  delay(3000);

  webSocket.begin(ticker_server_ip, ticker_server_port, String(ticker_server_url) + wifi->getCurrencyPair());
  webSocket.onEvent(webSocketEvent);

  pinMode(PORTAL_TRIGGER_PIN, INPUT);
}

void loop() {
  webSocket.loop();
  if (digitalRead(PORTAL_TRIGGER_PIN) == LOW) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      longPressActive = true;
      DEBUG_SERIAL.println("Reseting settings");
      wifi->resetSettings();
      delay(500);
      ESP.restart();
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        DEBUG_SERIAL.println("Starting portal");
        wifi->startAP("OnDemandAP", 120);
      }
      buttonActive = false;
    }
  }
  // TODO: check if not connected and display message
  // TODO: periodically display status of wifi
}
