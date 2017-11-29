#include "config_common.hpp"

#include <Arduino.h>
#include <Hash.h>
#include <Wire.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <WebSocketsClient.h>
#undef NETWORK_W5100 // To fix WebSockets and NTPClientLib #define conflict
#undef NETWORK_ENC28J60
#undef NETWORK_ESP8266

//#include <Hash.h>
//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <vector>

#include "display.hpp"
#include "config.hpp"

#include "firmware.hpp"
#include "wifi.hpp"

#include <EEPROM.h>

// NTP
#include <TimeLib.h>
#include <Time.h>
#include <NtpClientLib.h>

#include <Ticker.h>

Ticker g_ticker_clock;
Ticker g_ticker_blink;

WebSocketsClient webSocket;

Display *g_display;
shared_ptr<PriceAction> g_price_action;
shared_ptr<ClockAction> g_clock_action;
AP_list *g_APs;
WiFiCore *g_wifi;

//DisplayNG *g_display_ng;

long lastUpdateMillis = 0;

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

void clock_callback()
{
  auto time = NTP.getTimeDateString();
  DEBUG_SERIAL.printf("[NTP] Displaying time %s\n",  time.c_str());
  g_clock_action->updateTime(time);
  g_display->prependAction(g_clock_action);
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
        g_price_action->updatePrice(currentPrice);
        DEBUG_SERIAL.printf("[WSc] get tick: %i\n", currentPrice);
//        g_display->blinkDot(); // FIXME
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
  g_ticker_blink.detach();
  g_ticker_blink.attach(0.2, blink_callback);

  g_display->prependAction(make_shared<RotatingTextAction>(
    String("PLEASE CONNECT TO AP ") + ap_ssid,
    Coords{32,0}, 32, -1)
  );
}


void setupSerial()
{
  DEBUG_SERIAL.begin(115200);
  DEBUG_SERIAL.setDebugOutput(true);
  delay(1000);
  DEBUG_SERIAL.printf("Free memory: %i\n",ESP.getFreeHeap());
  DEBUG_SERIAL.printf("Last reset reason: %s\n",ESP.getResetReason().c_str());
  DEBUG_SERIAL.printf("Last reset info: %s\n",ESP.getResetInfo().c_str());
}

void setupDisplay()
{
#if defined(X_DISPLAY_U8G2)
  g_display = new DisplayU8G2(&g_display_hw, X_DISPLAY_DEFAULT_ROTATION);
#elif defined(X_DISPLAY_TM1637)
  g_display = new DisplayTM1637(&g_display_hw);
#else
  #error error
#endif
  g_display->setContrast(g_contrast);
  g_display->setupTickCallback([&]() { g_display->tick(); }); // can't be moved to class declaration because of lambda capture
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
  // start ticker with 0.6s because we start in AP mode and try to connect
  g_ticker_blink.attach(0.6, blink_callback);
}

void connectToWiFi()
{
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  g_wifi->setAPCallback(configModeCallback);
  g_wifi->connectToWiFiOrFallbackToAP();

  //if you get here you have connected to the WiFi
  DEBUG_SERIAL.println("connected...yeey :)");
  g_ticker_blink.detach();
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

  g_clock_action = make_shared<ClockAction>(Coords{4,-1}, 2.0, u8g2_font_profont10_tf); // display clock for 2 secs
  g_ticker_clock.attach(30.0, clock_callback);
}

void setup() {
  setupSerial();
  setupTicker();
  setupDisplay();
  loadParameters();

  g_price_action = make_shared<PriceAction>(Coords{0,-1}, u8g2_font_profont10_tf);

  g_display->queueAction(make_shared<StaticTextAction>("CRYPTOCLOCK", Coords{0,0}, 1.0, u8g2_font_profont10_tf));
//  g_display->queueAction(make_shared<StaticTextAction>(app_version, Coords{0,0}, 1.0, u8g2_font_profont10_tf));
//  g_display->queueAction(make_shared<RotatingTextAction>(ESP.getSketchMD5(), Coords{16,0}, 22, 2.0, u8g2_font_5x7_mf));

  g_display->queueAction(make_shared<RotatingTextAction>("--> WiFi ", Coords{0,0}, 32, -1, u8g2_font_profont10_tf));
  connectToWiFi();
  g_display->replaceAction(make_shared<StaticTextAction>(WiFi.SSID(), Coords{0,0}, 1.0, u8g2_font_profont10_tf));

  g_display->queueAction(make_shared<RotatingTextAction>("UPDATING... ", Coords{0,0}, 32, -1, u8g2_font_profont10_tf));
  updateFirmware();
  g_display->replaceAction(g_price_action);

  setupNTP();

  //keep LED off
  digitalWrite(BUILTIN_LED, false);

  connectWebSocket();

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
        g_ticker_blink.detach();
        g_ticker_blink.attach(0.2, blink_callback);
        g_wifi->startAP(String("OnDemandAP_"+String(ESP.getChipId())).c_str(), 120);
        ESP.restart();
      }
      buttonActive = false;
    }
  }
  delay(10);
  // TODO: check if not connected and display message
  // TODO: periodically display status of wifi
}
