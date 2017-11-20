#include "config_common.hpp"

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

#include "display.hpp"
#include "config.hpp"

#include "firmware.hpp"
#include "wifi.hpp"

//for LED status
#include <Ticker.h>
Ticker g_blink_ticker;
Ticker g_time_ticker;

#include <EEPROM.h>

// NTP
#include <TimeLib.h>
#include <Time.h>
#include <NtpClientLib.h>

WebSocketsClient webSocket;

Display *g_display;
AP_list *g_APs;
WiFiCore *g_wifi;

long lastUpdateMillis = 0;
int lastPrice = -1;

long buttonTimer = 0;
long longPressTime = 3000;
boolean buttonActive = false;
boolean longPressActive = false;

void blink_callback()
{
  //toggle state
  int state = digitalRead(BUILTIN_LED);  // get the current state of GPIO1 pin
  digitalWrite(BUILTIN_LED, !state);     // set pin to the opposite state
}

void time_callback()
{
  g_display->displayTime(NTP.getTimeDateString());
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
          g_display->blinkDot();
        } else {
          g_display->refreshPrice(lastPrice, currentPrice);
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
  auto ap_ssid = myWiFiManager->getConfigPortalSSID();
  DEBUG_SERIAL.println(ap_ssid);

  //entered config mode, make led toggle faster
  g_blink_ticker.attach(0.2, blink_callback);

  g_display->displayRotate(String("PLEASE CONNECT TO AP ") + ap_ssid,200);
  g_display->displayText("WIFI", 4);
}


void setupSerial()
{
  DEBUG_SERIAL.begin(115200);
  DEBUG_SERIAL.setDebugOutput(true);
  DEBUG_SERIAL.printf("Free memory: %i\n",ESP.getFreeHeap());
}

void setupDisplay()
{
#ifdef X_DISPLAY_U8G2
  g_display = new DisplayU8G2(g_display_hw);
#endif
#ifdef X_DISPLAY_TM1637
  g_display = new DisplayTM1637(g_display_hw);
#endif
  g_display->setContrast(g_contrast);
}

void loadParameters()
{
  EEPROM.begin(2048);
  g_parameters.loadFromEEPROM();
  g_APs = new AP_list();
  g_wifi = new WiFiCore(g_display, g_APs);
  EEPROM.end();
}

void setupTicker()
{
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  // start ticker with 0.6 because we start in AP mode and try to connect
  g_blink_ticker.attach(0.6, blink_callback);
}

void connectToWiFi()
{
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  g_wifi->setAPCallback(configModeCallback);
  g_wifi->connectToWiFiOrFallbackToAP();

  //if you get here you have connected to the WiFi
  DEBUG_SERIAL.println("connected...yeey :)");
  g_blink_ticker.detach();
}

void connectWebSocket()
{
  webSocket.beginSSL(
    g_parameters["ticker_server_host"],
    g_parameters["ticker_server_port"].toInt(),
    g_parameters["ticker_path"] + g_parameters["currency_pair"]
  );

  webSocket.onEvent(webSocketEvent);
}

void setupNTP()
{
  DEBUG_SERIAL.println("Starting NTP..");
  NTP.begin("ntp.nic.cz", 1, true);
  NTP.setInterval(1800);

  g_time_ticker.attach(10, time_callback);
}

void setup() {
  setupSerial();
  setupDisplay();
  loadParameters();

  g_display->displayText(g_parameters["currency_pair"]);

  setupTicker();
  connectToWiFi();

  g_display->displayText(WiFi.SSID());
  delay(1000);

  g_display->displayText("UPDATING");
  updateFirmware();

  //keep LED off
  digitalWrite(BUILTIN_LED, false);

  g_display->displayText(app_version);
  delay(3000);
  g_display->displayText(ESP.getSketchMD5());
  delay(3000);
  g_display->displayText(g_parameters["currency_pair"]);
  delay(3000);

  connectWebSocket();

  pinMode(PORTAL_TRIGGER_PIN, INPUT);

  setupNTP();
}

void loop() {
  webSocket.loop();
  if (digitalRead(PORTAL_TRIGGER_PIN) == LOW) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      // TODO: clear EEPROM?
      longPressActive = true;
      DEBUG_SERIAL.println("Reseting settings");
      g_wifi->resetSettings();
      delay(500);
      ESP.restart();
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        webSocket.disconnect();
        DEBUG_SERIAL.println("Starting portal");
        g_wifi->startAP(String("OnDemandAP_"+String(ESP.getChipId())).c_str(), 120);
        ESP.restart();
      }
      buttonActive = false;
    }
  }
  // TODO: check if not connected and display message
  // TODO: periodically display status of wifi
}
