#include "config_common.hpp"

#include <Arduino.h>
#include <Hash.h>
#include <Wire.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <WebSocketsClient.h>
#undef NETWORK_W5100 // To fix WebSockets and NTPClientLib #define conflict
#undef NETWORK_ENC28J60
#undef NETWORK_ESP8266

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <vector>

#include "display.hpp"
#include "display_action_text.hpp"
#include "display_action_bitmap.hpp"
#include "display_action_price.hpp"
#include "display_action_clock.hpp"
using Display::Coords;

#include "config.hpp"

#include "firmware.hpp"
#include "wifi.hpp"
#include "utils.hpp"

#include <EEPROM.h>

// NTP
#include <TimeLib.h>
#include <Time.h>
#include <NtpClientLib.h>

#include <Ticker.h>

Ticker g_ticker_clock;

WebSocketsClient g_webSocket;

DisplayT *g_display;
shared_ptr<Display::Action::Price> g_price_action;
shared_ptr<Display::Action::Clock> g_clock_action;
AP_list *g_APs;
WiFiCore *g_wifi;

unsigned long lastUpdateMillis = 0;

unsigned long buttonTimer = 0;
unsigned long longPressTime = 3000;
boolean buttonActive = false;
boolean longPressActive = false;

static const unsigned char s_crypto2_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x80, 0x01, 0x53, 0x4b, 0x8f, 0x71,
   0xc3, 0x48, 0xd3, 0x9b, 0xc3, 0x70, 0x93, 0x99, 0xd3, 0x40, 0x8f, 0x99,
   0xce, 0x38, 0x03, 0x71, 0x00, 0x00, 0x00, 0x00
};

// static const unsigned char s_clock_bits[] = {
//    0x00, 0x00, 0x00, 0x00, 0x38, 0xc3, 0x71, 0x26, 0x4c, 0x63, 0x9a, 0x16,
//    0x0c, 0x63, 0x1a, 0x0e, 0x0c, 0x63, 0x1a, 0x0e, 0x4c, 0x63, 0x9a, 0x16,
//    0x38, 0xcf, 0x71, 0x26, 0x00, 0x00, 0x00, 0x00 };

static const unsigned char s_clock_inverted_bits[] = {
   0xff, 0xff, 0xff, 0xff, 0xc7, 0x3c, 0x8e, 0xd9, 0xb3, 0x9c, 0x65, 0xe9,
   0xf3, 0x9c, 0xe5, 0xf1, 0xf3, 0x9c, 0xe5, 0xf1, 0xb3, 0x9c, 0x65, 0xe9,
   0xc7, 0x30, 0x8e, 0xd9, 0xff, 0xff, 0xff, 0xff
};

void clock_callback()
{
  auto time = NTP.getTimeDateString();
  DEBUG_SERIAL.printf("[NTP] Displaying time %s\n",  time.c_str());
  g_clock_action->updateTime(time);
//  g_display->prependAction(g_clock_action);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(
      g_clock_action,
      g_price_action,
      0.5,
      +1
    )
  );
  g_display->prependAction(g_clock_action);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(
      g_price_action,
      g_clock_action,
      0.5,
      -1
    )
  );
}

void webSocketEvent_callback(WStype_t type, uint8_t * payload, size_t length) {
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
        if (str==";UPDATE") {
          g_webSocket.disconnect();
          DEBUG_SERIAL.println(F("Update request received, updating"));
          g_display->queueAction(make_shared<Display::Action::RotatingText>("UPDATING... ", -1, 20));
          Firmware::update(g_parameters["update_url"]);
          ESP.restart();
        } else if (str.startsWith(";ATH=")) { // All-Time-High
          int ATHPrice = str.substring(5).toInt();
          g_price_action->setATHPrice(ATHPrice);
        } else {
          int currentPrice = str.toInt();
          g_price_action->updatePrice(currentPrice);
          DEBUG_SERIAL.printf("[WSc] get tick: %i\n", currentPrice);
          DEBUG_SERIAL.printf("Free Heap: %i\n", ESP.getFreeHeap());
  //        g_display->blinkDot(); // FIXME
        }
      }
      break;
    case WStype_BIN:
      DEBUG_SERIAL.printf("[WSc] get binary length: %u\n", length);
      hexdump(payload, length);
      break;
    case WStype_ERROR:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

//gets called when WiFiManager enters configuration mode
void configModeCallback (WiFiManager *myWiFiManager)
{
  DEBUG_SERIAL.println(F("Entered config mode"));
  DEBUG_SERIAL.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  String ap_ssid = myWiFiManager->getConfigPortalSSID();
  DEBUG_SERIAL.println(ap_ssid);

  g_display->prependAction(make_shared<Display::Action::RotatingText>(
    "PLEASE CONNECT TO AP " + ap_ssid + "  ",
    -1, // duration
    20, // speed
    Coords{0,0}
  ));
}


void setupSerial()
{
  DEBUG_SERIAL.begin(115200);
  DEBUG_SERIAL.setDebugOutput(true);
  DEBUG_SERIAL.printf("Free memory: %i\n",ESP.getFreeHeap());
  DEBUG_SERIAL.printf("Last reset reason: %s\n",ESP.getResetReason().c_str());
  DEBUG_SERIAL.printf("Last reset info: %s\n",ESP.getResetInfo().c_str());
}

void setupDisplay()
{
#if defined(X_DISPLAY_U8G2)
  g_display = new Display::U8G2Matrix(
    &g_display_hw,
    X_DISPLAY_DEFAULT_ROTATION,
    g_display_width,
    g_display_height,
    u8g2_font_profont10_tf
  );
#elif defined(X_DISPLAY_TM1637)
  g_display = new Display::TM1637(&g_display_hw, g_display_num_digits);
#elif defined(X_DISPLAY_LIXIE)
  g_display_hw.initialize<LIXIE_PIN, g_display_num_digits>();
  g_display = new Display::LixieNumeric(&g_display_hw, g_display_num_digits);
#elif defined(X_DISPLAY_NEOPIXEL)
  auto display = new Display::Neopixel(g_display_num_leds);
  display->initialize<NEOPIXEL_PIN>();
  g_display = display;
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
  digitalWrite(BUILTIN_LED, true); // high = off
}

void connectToWiFi()
{
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  g_wifi->setAPCallback(configModeCallback);
  g_wifi->connectToWiFiOrFallbackToAP();

  //if you get here you have connected to the WiFi
  DEBUG_SERIAL.println(F("connected...yeey :)"));
}

void connectWebSocket()
{
  g_webSocket.beginSSL(
    g_parameters["ticker_server_host"],
    g_parameters["ticker_server_port"].toInt(),
    g_parameters["ticker_path"] + g_parameters["currency_pair"]
  );

  g_webSocket.onEvent(webSocketEvent_callback);
}

void setupNTP()
{
  DEBUG_SERIAL.println(F("Starting NTP.."));
  NTP.begin("ntp.nic.cz", 1, true);
  NTP.setInterval(1800);

  g_clock_action = make_shared<Display::Action::Clock>(3.0, Coords{4,-1}); // display clock for 3 secs
  g_ticker_clock.attach(30.0, clock_callback);
}

void setup() {
  setupSerial();
  setupTicker();
  setupDisplay();
  loadParameters();

  g_price_action = make_shared<Display::Action::Price>(10.0); // animation speed, in digits per second

  /* logo */
  auto logo_top = make_shared<Display::Action::StaticBitmap>(s_crypto2_bits, 32, 8, 1.5);
  auto logo_bottom = make_shared<Display::Action::StaticBitmap>(s_clock_inverted_bits, 32, 8, 3.5);
  g_display->queueAction(logo_top);
  g_display->queueAction(make_shared<Display::Action::SlideTransition>(logo_top, logo_bottom, 0.5, -1));
  g_display->queueAction(logo_bottom);

  /* WiFi */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("--> WiFi ", -1, 20));
  connectToWiFi();
  g_display->removeBottomAction();
  g_display->queueAction(make_shared<Display::Action::StaticText>(WiFi.SSID(), 1.0));

  /* Update */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("UPDATING... ", -1, 20));
  Firmware::update(g_parameters["update_url"]);
  g_display->replaceAction(g_price_action);

  /* NTP */
  setupNTP();

  /* connect to ticker server */
  connectWebSocket();

  pinMode(PORTAL_TRIGGER_PIN, INPUT);
}

void loop() {
  g_webSocket.loop();

  if (digitalRead(PORTAL_TRIGGER_PIN) == LOW) {
    if (buttonActive == false) {
      buttonActive = true;
      buttonTimer = millis();
    }
    if ((millis() - buttonTimer > longPressTime) && (longPressActive == false)) {
      g_display->prependAction(make_shared<Display::Action::RotatingText>("RESET... ", -1, 20));
      // TODO: clear EEPROM?
      longPressActive = true;
      DEBUG_SERIAL.println(F("Reseting settings"));
      g_wifi->resetSettings();
      EEPROM.begin(2048);
      Utils::eeprom_Erase(0,2048);
      EEPROM.end();

      delay(1000);
      ESP.reset();
    }
  } else {
    if (buttonActive == true) {
      if (longPressActive == true) {
        longPressActive = false;
      } else {
        g_webSocket.disconnect();
        DEBUG_SERIAL.println(F("Starting portal"));
        g_wifi->startAP("OnDemandAP_"+String(ESP.getChipId()), 120);
        ESP.restart();
      }
      buttonActive = false;
    }
  }
  delay(10);
  // TODO: check if not connected and display message
  // TODO: periodically display status of wifi
}
