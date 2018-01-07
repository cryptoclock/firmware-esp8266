#include "config_common.hpp"

#include <Arduino.h>
#include <Hash.h>
#include <Wire.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <vector>

#include "display.hpp"
#include "display_action_text.hpp"
#include "display_action_bitmap.hpp"
#include "display_action_price.hpp"
#include "display_action_clock.hpp"
#include "display_action_testdisplay.hpp"
#include "display_action_menu.hpp"
using Display::Coords;

#include "config.hpp"

#include "firmware.hpp"
#include "wifi.hpp"
#include "utils.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "data_source.hpp"

#include <EEPROM.h>

// NTP
#include <TimeLib.h>
#include <Time.h>
#include <NtpClientLib.h>

#include <Ticker.h>

#include <ESP8266TrueRandom.h>

Ticker g_ticker_clock;

DisplayT *g_display;
shared_ptr<Display::Action::Price> g_price_action;
shared_ptr<Display::Action::Clock> g_clock_action;
WiFiCore *g_wifi;
DataSource *g_data_source;

shared_ptr<Button> g_flash_button;

bool g_start_ondemand_ap = false;
bool g_force_wipe = false;
bool g_message_mode = false;

enum class MODE { TICKER, MENU, ANNOUNCEMENT };
MODE g_current_mode(MODE::TICKER);

shared_ptr<Menu> g_menu = nullptr;

String g_announcement="";
shared_ptr<Display::ActionT> g_announcement_action;

static const unsigned char s_crypto2_bits[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x80, 0x01, 0x53, 0x4b, 0x8f, 0x71,
   0xc3, 0x48, 0xd3, 0x9b, 0xc3, 0x70, 0x93, 0x99, 0xd3, 0x40, 0x8f, 0x99,
   0xce, 0x38, 0x03, 0x71, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char s_clock_inverted_bits[] PROGMEM = {
   0xff, 0xff, 0xff, 0xff, 0xc7, 0x3c, 0x8e, 0xd9, 0xb3, 0x9c, 0x65, 0xe9,
   0xf3, 0x9c, 0xe5, 0xf1, 0xf3, 0x9c, 0xe5, 0xf1, 0xb3, 0x9c, 0x65, 0xe9,
   0xc7, 0x30, 0x8e, 0xd9, 0xff, 0xff, 0xff, 0xff
};

void clock_callback()
{
  if (g_current_mode != MODE::TICKER)
    return;

  auto time = NTP.getTimeDateString();
  DEBUG_SERIAL.printf_P(PSTR("[NTP] Displaying time %s\n"),  time.c_str());
  g_clock_action->updateTime(time);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(
      g_clock_action,
      g_price_action,
      0.5,
      Coords{0,+1}
    )
  );
  g_display->prependAction(g_clock_action);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(
      g_price_action,
      g_clock_action,
      0.5,
      Coords{0,-1}
    )
  );
}

void setAnnouncement(const String& message, action_callback_t onfinished_cb)
{
  g_announcement_action = make_shared<Display::Action::RotatingTextOnce>(message,20,Coords{0,0},onfinished_cb);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(
      nullptr,
      g_price_action,
      0.5,
      Coords{-1,0}
    )
  );
  g_display->prependAction(g_announcement_action);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(
      g_price_action,
      nullptr,
      0.5,
      Coords{-1,0}
    )
  );
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
  DEBUG_SERIAL.setDebugOutput(1);
  DEBUG_SERIAL.setDebugOutput(0);

  DEBUG_SERIAL.printf_P(PSTR("Free memory: %i\n"),ESP.getFreeHeap());
  DEBUG_SERIAL.printf_P(PSTR("Last reset reason: %s\n"),ESP.getResetReason().c_str());
  DEBUG_SERIAL.printf_P(PSTR("Last reset info: %s\n"),ESP.getResetInfo().c_str());
}

void setupDisplay()
{
#if defined(X_DISPLAY_U8G2)
  g_display = new Display::U8G2Matrix(
    &g_display_hw,
    X_DISPLAY_MILIS_PER_TICK,
    false,
    X_DISPLAY_WIDTH,
    X_DISPLAY_HEIGHT
  );
#elif defined(X_DISPLAY_TM1637)
  g_display = new Display::TM1637(&g_display_hw, X_DISPLAY_MILIS_PER_TICK, X_DISPLAY_WIDTH);
#elif defined(X_DISPLAY_LIXIE)
  g_display_hw.initialize<X_DISPLAY_DATA_PIN, X_DISPLAY_WIDTH>();
  g_display = new Display::LixieNumeric(&g_display_hw, X_DISPLAY_MILIS_PER_TICK, X_DISPLAY_WIDTH);
#elif defined(X_DISPLAY_NEOPIXEL)
  auto display = new Display::Neopixel(X_DISPLAY_MILIS_PER_TICK, X_DISPLAY_WIDTH, X_DISPLAY_HEIGHT);
  display->initialize<X_DISPLAY_DATA_PIN>();
  g_display = display;
#else
  #error error
#endif
  g_display->setupTickCallback([&]() { g_display->tick(); }); // can't be moved to class declaration because of lambda capture
}

void loadParameters()
{
  Utils::eeprom_BEGIN();
  g_parameters.loadFromEEPROM();
  g_wifi = new WiFiCore(g_display);

  // sanitize parameters
  g_parameters.setValue("brightness", String(std::min(std::max(g_parameters["brightness"].toInt(),0L),15L)) );
  g_parameters.setValue("font", String(std::min(std::max(g_parameters["font"].toInt(),0L),2L)) );

  g_display->setBrightness(g_parameters["brightness"].toInt() * 16);
  g_display->setRotation(g_parameters["rotate_display"]=="1");
  g_display->setFont(g_parameters["font"].toInt());

  // no uuid set, generate new one
  if (g_parameters["__device_uuid"] == "") {
    uint8_t uuid[16];
    ESP8266TrueRandom.uuid(uuid);
    const String uuid_str = ESP8266TrueRandom.uuidToString(uuid);
    g_parameters.setValue("__device_uuid", uuid_str);
    g_parameters.storeToEEPROM();
  }
  Utils::eeprom_END();
}

void setupTicker()
{
  //set led pin as output
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, true); // high = off
}

void setupDataSource()
{
  String ticker_url = g_parameters["ticker_path"] + g_parameters["currency_pair"] + "?uuid=" + g_parameters["__device_uuid"];
  g_data_source = new DataSource(g_parameters["ticker_server_host"], g_parameters["ticker_server_port"].toInt(), ticker_url);

  g_data_source->setOnUpdateRequest([&]() {
    DEBUG_SERIAL.println(F("Update request received, updating"));
    g_display->queueAction(make_shared<Display::Action::RotatingText>("UPDATING... ", -1, 20));
    Firmware::update(g_parameters["update_url"]);
    ESP.restart();
  });

  g_data_source->setOnAnnouncement([&](const String& msg){
    if (g_announcement=="") {
      g_announcement = msg;
    } // ignore otherwise
  });

  g_data_source->setOnPriceATH([&](const String& price){
    g_price_action->setATHPrice(price.toInt());
  });

  g_data_source->setOnPriceChange([&](const String& price){
    long currentPrice = price.toInt();
    DEBUG_SERIAL.printf_P(PSTR("[WSc] price tick: %li\n"),currentPrice);
    g_price_action->updatePrice(currentPrice);
    DEBUG_SERIAL.printf_P(PSTR("Free Heap: %i\n"), ESP.getFreeHeap());
  });

  g_data_source->connect();
}

void connectToWiFi()
{
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  g_wifi->setAPCallback(configModeCallback);
  g_wifi->connectToWiFiOrFallbackToAP();

  DEBUG_SERIAL.println(F("connected...yeey :)"));
}

void setupNTP()
{
  DEBUG_SERIAL.println(F("Starting NTP.."));
  NTP.begin("ntp.nic.cz", 1, true);
  NTP.setInterval(1800);

#if !defined(X_CLOCK_ONLY_DISPLAY)
  g_clock_action = make_shared<Display::Action::Clock>(3.0, Coords{0,0}); // display clock for 3 secs
  g_ticker_clock.attach(30.0, clock_callback);
#endif
}

void factoryReset(void)
{
  g_display->prependAction(make_shared<Display::Action::RotatingText>("RESET... ", -1, 20));
  DEBUG_SERIAL.println(F("Reseting settings"));
  g_wifi->resetSettings();

  Utils::eeprom_WIPE();

  delay(1000);
  g_wifi->resetSettings();
  ESP.reset();
}

void startOnDemandAP(void)
{
  DEBUG_SERIAL.println(F("ODA"));
  g_data_source->disconnect();
  DEBUG_SERIAL.println(F("Starting portal"));
  g_wifi->startAP("OnDemandAP_"+String(ESP.getChipId()), 120);
  ESP.restart();
}

void switchMenu(void);
void setupDefaultButtons()
{
  g_flash_button->onShortPress(switchMenu);
  g_flash_button->onLongPress([]() { g_start_ondemand_ap = true; });
  g_flash_button->onSuperLongPress([]() { g_force_wipe = true;} );
  g_flash_button->setupTickCallback([]() { g_flash_button->tick(); });
}

void switchMenu(void)
{
  if (g_current_mode != MODE::MENU) {
    auto last_mode = g_current_mode;
    g_current_mode = MODE::MENU;
    g_menu->start(g_display, [=](){ // set callback when menu is finished
      g_current_mode = last_mode;
      setupDefaultButtons();
    });

    g_flash_button->onLongPress([](){ g_menu->onLongPress(); });
    g_flash_button->onShortPress([](){ g_menu->onShortPress(); });
  }
}

void setupButton()
{
  g_flash_button = make_shared<Button>(PORTAL_TRIGGER_PIN);
  setupDefaultButtons();
}

void setupMenu()
{
  const menu_items_t items({
    std::make_shared<MenuItemNumericRange>("font","Font", "Font",0,2, 0, [](const String& value){ g_display->setFont(value.toInt()); } ),
    std::make_shared<MenuItemNumericRange>("brightness","Bright", "Bri",0,15, 0, [](const String& value){ g_display->setBrightness(value.toInt() * 16); } ),
    std::make_shared<MenuItemBoolean>("rotate_display","Rotate", "Rot", false, [](const String& value){ g_display->setRotation(value=="1"); } )
  });
  g_menu = std::make_shared<Menu>(&g_parameters, items);
}

/* --------------------- */

#ifdef X_TEST_DISPLAY
#include "main_tester.hpp"
#elif defined(X_CLOCK_ONLY_DISPLAY)
#include "main_clock_only.hpp"
#else

void setup() {
  setupSerial();
  setupTicker();
  setupDisplay();
  loadParameters();
  setupMenu();

  g_price_action = make_shared<Display::Action::Price>(10.0); // animation speed, in digits per second

  /* logo */
  auto logo_top = make_shared<Display::Action::StaticBitmap>(s_crypto2_bits, 32, 8, 1.5);
  auto logo_bottom = make_shared<Display::Action::StaticBitmap>(s_clock_inverted_bits, 32, 8, 3.5);
  g_display->queueAction(logo_top);
  g_display->queueAction(make_shared<Display::Action::SlideTransition>(logo_top, logo_bottom, 0.5, Coords{0,-1}));
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

  setupButton();
  setupNTP();
  setupDataSource();
}

void loop() {
  if (g_start_ondemand_ap && !g_flash_button->pressed())
    startOnDemandAP();

  if (g_force_wipe==true)
    factoryReset();

  if (g_announcement!="") {
    if (g_current_mode==MODE::TICKER) {
      g_current_mode = MODE::ANNOUNCEMENT;
      setAnnouncement(g_announcement, [](){ g_current_mode = MODE::TICKER; });
      g_announcement = "";
    }
  }

  g_data_source->loop();
  delay(10);
}

#endif
