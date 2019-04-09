/* 
  Cryptoclock ESP8266
  Copyright (C) 2018 www.cryptoclock.net <info@cryptoclock.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "config_common.hpp"

#include <Arduino.h>
#include <Hash.h>
#include <Wire.h>

#include <ESP8266WiFi.h>

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <vector>

#include "display.hpp"
#include "display_action_text.hpp"
#include "display_action_bitmap.hpp"
#include "display_action_price.hpp"
#include "display_action_clock.hpp"
#include "display_action_testdisplay.hpp"
#include "display_action_menu.hpp"
#include "display_action_multi.hpp"
#include "display_action_countdown.hpp"
using Display::Coords;
using Display::Action::ActionPtr_t;

#include "config.hpp"

#include "firmware.hpp"
#include "wifi.hpp"
#include "utils.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "data_source.hpp"
#include "bitmaps.hpp"
#include "gyro.hpp"
#include "ntp.hpp"
#include "sound.hpp"

#include <EEPROM.h>


#include <Ticker.h>

#include <ESP8266TrueRandom.h>

using std::make_shared;
using std::queue;

ParameterStore g_parameters;

Ticker g_ticker_clock;

DisplayT *g_display = nullptr;
shared_ptr<Display::PriceAction> g_price_action;
shared_ptr<Display::Action::Clock> g_clock_action;
WiFiCore *g_wifi = nullptr;
DataSource *g_data_source = nullptr;
Sound *g_sound = nullptr;

shared_ptr<Button> g_flash_button;

bool g_start_ondemand_ap = false;
bool g_force_wipe = false;
bool g_entered_ap_mode = false;
bool g_reset_price_on_next_tick = false;

enum class MODE { TICKER, MENU, ANNOUNCEMENT, OTP, AP, UPDATE, CONNECTING, COUNTDOWN};
MODE g_current_mode(MODE::CONNECTING);

shared_ptr<Menu> g_menu = nullptr;

// FIXME: class
String g_announcement="";
bool g_announcement_static = false;
int g_announcement_time = 0;
shared_ptr<Display::ActionT> g_announcement_action;

NTP g_NTP;

void clock_callback()
{
  if (g_current_mode != MODE::TICKER)
    return;

  if (g_parameters["clock_mode"].toInt() == 0) // Off
    return;

  if (g_clock_action->isAlwaysOn()) {
    if (g_display->getTopAction() == g_clock_action) // clock is already top action
      return;

    g_display->cleanQueue();
    g_display->prependAction(g_price_action);
    g_display->prependAction(g_clock_action);
    return;
  }

// don't switch display to clock if time wasn't yet set by NTP
#if !defined(X_CLOCK_ONLY_DISPLAY) 
  g_clock_action->updateTime();
  if (!g_clock_action->isTimeSet())
    return;
#endif

  DEBUG_SERIAL.println(F("[NTP] Displaying clock"));
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(g_clock_action, 1, g_price_action, 2, 0.5, Coords{0,+1})
  );
  g_display->prependAction(g_clock_action);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(g_price_action, 2, g_clock_action, 1, 0.5, Coords{0,-1})
  );
}

// filter out special commands from announcement message
String parseAnnouncment(const String &message, /* out */ String& sound_message)
{
  String filtered_msg;
  for (int i=0;i<(int)message.length();++i) {
    char c = message[i];
    if (c=='\\' && ++i < (int)message.length()) {
      switch (message[i]) {
        case '\\': filtered_msg += '\\'; break;
        case 's': // "\s....$" plays music in RTTTL format
        {
          int begin = i+1;
          int end = message.indexOf('$',begin);
          if (end==-1 || begin >= (int)message.length() || end >= (int)message.length()) {
            DEBUG_SERIAL.printf("Malformed sound message: '%s'\n",message.c_str());
            return filtered_msg;
          }
          sound_message = message.substring(begin,end);
          i = end;
        }
        break;
        default: break;
      }
    } else {
      filtered_msg += c;
    }
  }
  return filtered_msg;
}

void setAnnouncement(const String& announcement, const bool static_msg, const int display_time, action_callback_t onfinished_cb)
{
  g_current_mode = MODE::ANNOUNCEMENT;

  String sound = "";
  String message = parseAnnouncment(announcement, sound);

  if(sound!="" && g_sound)
    g_sound->playMusicRTTTL(sound);

  queue<ActionPtr_t> q;

  auto current_action = g_display->getTopAction();
  if (static_msg) {
    g_announcement_action = make_shared<Display::Action::StaticText>(message,display_time,Coords{0,0},onfinished_cb);
  } else {
    g_announcement_action = make_shared<Display::Action::RotatingTextOnce>(message,20,Coords{0,0},onfinished_cb);
  }
  q.push(make_shared<Display::Action::SlideTransition>(current_action, 1, nullptr, 1, 0.5, Coords{-1,0}));
  q.push(g_announcement_action);
  q.push(make_shared<Display::Action::SlideTransition>(nullptr, 1, current_action, 1, 0.5, Coords{-1,0}));

  g_display->prependAction(make_shared<Display::Action::MultiRepeat>(q, 0, false, nullptr));
}

void replaceAnnouncement(const String& message, const bool static_msg, const int display_time, action_callback_t onfinished_cb)
{
  queue<ActionPtr_t> q;

  g_display->removeTopAction();

  if (static_msg)
    g_announcement_action = make_shared<Display::Action::StaticText>(message,display_time,Coords{0,0},onfinished_cb);
  else
    g_announcement_action = make_shared<Display::Action::RotatingTextOnce>(message,20,Coords{0,0},onfinished_cb);

  q.push(g_announcement_action);
  q.push(make_shared<Display::Action::SlideTransition>(nullptr, 1, g_display->getTopAction(), 1, 0.5, Coords{-1,0}));

  g_display->prependAction(make_shared<Display::Action::MultiRepeat>(q, 0, false, nullptr));
}

void setupDefaultButtons();

void setCountDown(int countdown, int post)
{
  g_current_mode = MODE::COUNTDOWN;

  auto current_action = g_display->getTopAction();
  auto countdown_action = make_shared<Display::Action::CountDown>(countdown,post,Coords{0,0},[&](){ 
    setupDefaultButtons();
    g_current_mode = MODE::TICKER; 
  });

  g_flash_button->onShortPress([&]() {
    countdown_action->setFinished();
  });

  g_display->prependAction(make_shared<Display::Action::SlideTransition>(countdown_action, 1, current_action, 1, 0.5, Coords{-1,0}));
  g_display->prependAction(countdown_action);
  g_display->prependAction(make_shared<Display::Action::SlideTransition>(current_action, 1, countdown_action, 1, 0.5, Coords{-1,0}));
}

void configModeCallback (WiFiManager *myWiFiManager)
{
  g_current_mode = MODE::AP;
  
  g_entered_ap_mode = true;
  DEBUG_SERIAL.println(F("Entered config mode"));
  DEBUG_SERIAL.println(WiFi.softAPIP());
  String ap_ssid = myWiFiManager->getConfigPortalSSID();
  DEBUG_SERIAL.println(ap_ssid);

  g_display->prependAction(make_shared<Display::Action::RotatingText>(
    "PLEASE CONNECT TO AP " + ap_ssid + "  ", -1, 20, Coords{0,0}
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
#elif defined(X_DISPLAY_MAX7219)
  g_display = new Display::MAX7219(&g_display_hw, X_DISPLAY_MILIS_PER_TICK, X_DISPLAY_WIDTH);
#elif defined(X_DISPLAY_SMARTLED)
  auto display = new Display::SmartLED_Numeric(X_DISPLAY_MILIS_PER_TICK, X_DISPLAY_WIDTH, X_DISPLAY_LEDS, g_layout, X_DISPLAY_LIXIE);
  FastLED.addLeds<X_DISPLAY_FASTLED_DRIVER, X_DISPLAY_DATA_PIN>(display->getLEDdata(), X_DISPLAY_LEDS);
  g_display = display;
#else
  #error error
#endif
  g_display->setupTickCallback([&]() { 
    g_display->tick(); 
#ifdef HAS_GYROSCOPE
    MPUtick(); 
#endif
  }); // can't be moved to class declaration because of lambda capture
}

void setupClock()
{
#if !defined(X_CLOCK_ONLY_DISPLAY)
  g_clock_action = make_shared<Display::Action::Clock>(3.0, Coords{0,0}); // display clock for 3 secs
  g_ticker_clock.attach(30.0, clock_callback);
#endif
}

void setupParameters()
{
  g_parameters.addItem({"__LEGACY_ticker_server_host","","", 0, nullptr});
  g_parameters.addItem({"__LEGACY_ticker_server_port","","", 0, nullptr});
  g_parameters.addItem({"__LEGACY_ticker_path","","", 0, nullptr});
  g_parameters.addItem({"__LEGACY_currency_pair","","", 0, nullptr});
  g_parameters.addItem({"__device_uuid","","", 0, nullptr}); // new uuid will be generated on every device wipe
  g_parameters.addItem({"update_url","Update server","update.cryptoclock.net", 50, nullptr});
  g_parameters.addItem({"ticker_url","Ticker server","wss://ticker.cryptoclock.net:443/", 100, [](ParameterItem& item, bool init, bool final_change)
  {
    if (init==false) {
      g_data_source->reconnect();
      g_price_action->reset();
      g_announcement = " ";
    }
  }});
  g_parameters.addItem({"brightness","Brightness (1-5)","3", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    int brightness = std::min(std::max(item.value.toInt(),1L),5L); // sanitize
    item.value = String(brightness);
    int actual_brightness = 0;
    switch (brightness) {
      case 1: actual_brightness = 0; break;
      case 2: actual_brightness = 16; break;
      case 3: actual_brightness = 32; break;
      case 4: actual_brightness = 64; break;
      case 5: default: actual_brightness = 128; break;
    }
    g_display->setDisplayBrightness(actual_brightness);
  }});
  g_parameters.addItem({"font","Font (0-2)","0", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    int font = std::min(std::max(item.value.toInt(),0L),2L);
    item.value = String(font);
    g_display->setFont(font);
  }});

  g_parameters.addItem({"rotate_display",
#ifdef HAS_GYROSCOPE
    "Rotate Display (0,1, 2=auto)",
#else
    "Rotate Display (0,1)",
#endif
  "0", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    int rotate = std::min(std::max(item.value.toInt(),0L),2L);
    item.value = String(rotate);
    if (rotate==0 || rotate==1)
      g_display->setRotation(rotate);
  }});
  g_parameters.addItem({"clock_mode","Show Clock (0-2)","1", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    int clock_mode = std::min(std::max(item.value.toInt(),0L),2L);
    item.value = String(clock_mode);
    if (clock_mode == 2)
      g_clock_action->setAlwaysOn(true);
    else
      g_clock_action->setAlwaysOn(false);
//    g_clock_mode
  }});
  g_parameters.addItem({"clock_interval","Clock display interval (secs)","30", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    if (init || final_change) {
      int interval = std::max(item.value.toInt(),5L);
      item.value = String(interval);
      g_ticker_clock.attach(interval, clock_callback);
    }
  }});
  g_parameters.addItem({"timezone","Timezone (-11..+13)","1", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    if (final_change) {
      int timezone = std::min(std::max(item.value.toInt(),-11L),13L);
      item.value = String(timezone);
      g_NTP.setTimezone(timezone);
      g_NTP.init();
    }
  }});
}

void loadParameters()
{
  Utils::eeprom_BEGIN();
  g_parameters.loadFromEEPROMwithoutInit();
  g_wifi = new WiFiCore(g_display);

  // no uuid set, generate new one
  if (g_parameters["__device_uuid"] == "") {
    uint8_t uuid[16];
    ESP8266TrueRandom.uuid(uuid);
    const String uuid_str = ESP8266TrueRandom.uuidToString(uuid);
    g_parameters.setValue("__device_uuid", uuid_str);
    g_parameters.storeToEEPROMwithoutInit();
  }

  // handle legacy parameters
  if (g_parameters["__LEGACY_ticker_server_host"]!="")
  {
    g_parameters.setValue("ticker_url", "wss://" + g_parameters["__LEGACY_ticker_server_host"] + ":" +
      g_parameters["__LEGACY_ticker_server_port"] + g_parameters["__LEGACY_ticker_path"] + g_parameters["__LEGACY_currency_pair"]);
  }
  Utils::eeprom_END();
}


void setupHW()
{
  //set led pin as output
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, true); // high = off

#ifdef HAS_GYROSCOPE
  MPUsetup();
#endif
#ifdef HAS_SOUND
  g_sound = new Sound(SOUND_PIN);
#endif
}

void forceSetTickerMode();

void setupDataSource()
{
  g_data_source = new DataSource;

  g_data_source->setOnUpdateRequest([&]() {
    DEBUG_SERIAL.println(F("Update request received, updating"));
    g_display->prependAction(make_shared<Display::Action::RotatingText>("UPDATING... ", -1, 20));
    g_current_mode = MODE::UPDATE;
    Firmware::update(g_parameters["update_url"]);
    g_current_mode = MODE::TICKER;
    ESP.restart();
  });

  g_data_source->setOnAnnouncement([&](const String& msg, bool static_msg, int display_time){
    if (g_announcement=="") {
      g_announcement_static = static_msg;
      g_announcement_time = display_time;
      g_announcement = msg;
    } // ignore otherwise
  });

  g_data_source->setOnCountdown([&](const String& data) {
    DEBUG_SERIAL.printf_P(PSTR("Countdown received: '%s'"),data.c_str());
    int t = data.toInt();
    setCountDown(t, 5);
  });

  g_data_source->setOnSound([&](const String& data) {
    DEBUG_SERIAL.printf_P(PSTR("Sound received: '%s'"),data.c_str());
    g_sound->playMusicRTTTL(data);
  });

  g_data_source->setOnPriceATH([&](const String& price, int screen_idx){
    if (screen_idx!=0)
      return;

    g_price_action->setATHPrice(price);
  });

  g_data_source->setOnPriceTimeoutSet([&](const String& timeout){
    g_price_action->setPriceTimeout(timeout.toFloat());
  });

  g_data_source->setOnPriceChange([&](const String& price, int screen_idx){
    if (screen_idx!=0)
      return;

    auto currentPrice = Price(price);
    currentPrice.debug_print();

    if (g_reset_price_on_next_tick) {
      g_price_action->reset();
      g_reset_price_on_next_tick = false;
    }

    g_price_action->updatePrice(price);
    DEBUG_SERIAL.printf_P(PSTR("[SYSTEM] Free heap: %i, fragmentation: %.2f%%\n"),ESP.getFreeHeap(), Utils::getMemoryFragmentation());
  });

  g_data_source->setOnNewSettings([&](){
    g_reset_price_on_next_tick = true;
//    g_price_action->reset();
  });

  static const double otp_timeout = 180.0;
  g_data_source->setOnOTP([](const String& otp){
    auto multi = Display::Action::createRepeatedSlide({-1,0}, otp_timeout, 1.0,
      make_shared<Display::Action::StaticText>("OTP:", 0.8),
      make_shared<Display::Action::StaticText>(otp, 5.0),
      []() { forceSetTickerMode(); }
    );
    g_display->prependAction(multi);
    g_current_mode = MODE::OTP;
  });

  // on receiving OTP web confirmation
  g_data_source->setOnOTPack([](){
    if (g_current_mode == MODE::OTP)
      forceSetTickerMode();
  });

  g_data_source->connect();
}

void setupLogo()
{
  auto logo_top = make_shared<Display::Action::StaticBitmap>(s_crypto2_bits, 32, 8, 1.5);
  auto logo_bottom = make_shared<Display::Action::StaticBitmap>(s_clock_inverted_bits, 32, 8, 1.5);
  auto version = make_shared<Display::Action::StaticText>(String("v") + FIRMWARE_VERSION, 1.5);

  g_display->queueAction(logo_top);
  g_display->queueAction(make_shared<Display::Action::SlideTransition>(logo_top, 1, logo_bottom, 1, 0.5, Coords{0,-1}));
  g_display->queueAction(logo_bottom);
  g_display->queueAction(make_shared<Display::Action::SlideTransition>(logo_bottom, 1, version, 1, 0.5, Coords{0,-1}));
  g_display->queueAction(version);
}

void connectToWiFi()
{
  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  g_wifi->setAPCallback(configModeCallback);
  g_wifi->connectToWiFiOrFallbackToAP();

  DEBUG_SERIAL.println(F("[WiFi] connected to WiFi"));
  if (g_entered_ap_mode) {
    DEBUG_SERIAL.println(F("[WiFi] AP mode ended, restarting"));
    ESP.restart();
  }
}

void setupNTP()
{
  DEBUG_SERIAL.println(F("Starting NTP.."));
  int timezone = g_parameters["timezone"].toInt();
  g_NTP.setServer(NTP_SERVER);
  g_NTP.setTimezone(timezone);
  g_NTP.init();

  if (g_clock_action->isAlwaysOn())
    clock_callback();
}

void factoryReset()
{
  g_display->prependAction(make_shared<Display::Action::RotatingText>("RESET... ", -1, 20));
  DEBUG_SERIAL.println(F("Reseting settings"));
  g_wifi->resetSettings();

  Utils::eeprom_WIPE();

  delay(1000);
  g_wifi->resetSettings();
  ESP.reset();
}

void startOnDemandAP()
{
  DEBUG_SERIAL.println(F("ODA"));
  g_data_source->disconnect();
  DEBUG_SERIAL.println(F("Starting portal"));
  g_wifi->resetSettings();
  g_wifi->startAP("OnDemandAP_"+String(ESP.getChipId()), 120);
  g_wifi->resetSettings();
  ESP.restart();
}

void switchMenu();
void setupDefaultButtons()
{
  g_flash_button->onShortPress(switchMenu);
  g_flash_button->onLongPress([]() { 
    if (g_current_mode != MODE::AP) {
      g_start_ondemand_ap = true; 
      g_wifi->abortConnectionAttempt(); 
      g_display->prependAction(
        make_shared<Display::Action::StaticText>("APmode",5.0)
      );
    }
  });

  g_flash_button->onSuperLongPress([]() { g_force_wipe = true;} );
  g_flash_button->setupTickCallback([]() { g_flash_button->tick(); });
}

void switchMenu()
{
  if (g_current_mode != MODE::MENU) {
    auto last_mode = g_current_mode;
    g_current_mode = MODE::MENU;
    g_menu->start(g_display, [=](){ // set callback when menu is finished
      g_current_mode = last_mode;
      setupDefaultButtons();
      if (g_clock_action->isAlwaysOn()) {
        clock_callback();
      }
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

void forceSetTickerMode()
{
  g_current_mode = MODE::TICKER;
  g_display->cleanQueue();
  if (g_clock_action->isAlwaysOn()) {
    clock_callback();
  } else {
    g_display->prependAction(g_price_action);
  }
  setupDefaultButtons();
}

void sendOTPRequest()
{
  bool result = g_data_source->sendOTPRequest();
  g_display->prependAction(
    make_shared<Display::Action::StaticText>((result ? "-OK-" : "Failed"),2.0)
  );
  g_menu->end();

  g_flash_button->onShortPress([](){
    if (g_current_mode == MODE::OTP)
      forceSetTickerMode();
    else
      setupDefaultButtons();
  });
}

void displayDeviceInfo()
{
  setAnnouncement(Utils::getDeviceInfo(), false, 0, [](){g_menu->end();});
}

void setupMenu()
{
  const menu_items_t items({
    std::make_shared<MenuItemAction>("__otp","OTP","OTP", sendOTPRequest),
    std::make_shared<MenuItemAction>("__info","Info","Info", displayDeviceInfo),
    std::make_shared<MenuItemNumericRange>("font","Font", "Font",0,2, 0, nullptr),
    std::make_shared<MenuItemNumericRange>("brightness","Bright", "Bri",1,5, 0, nullptr),
#ifdef HAS_GYROSCOPE
    std::make_shared<MenuItemEnum>("rotate_display","Rotate", "R", false, vector<String>{"Off","On","auto"}, nullptr),
#else
    std::make_shared<MenuItemEnum>("rotate_display","Rotate", "R", false, vector<String>{"Off","On"}, nullptr),
#endif
    std::make_shared<MenuItemEnum>("clock_mode","Clock", "C", false, vector<String>{"Off","On","Only"}, nullptr),
    std::make_shared<MenuItemNumericRange>("timezone","Tzone", "Tz",-11,+13, 0, nullptr),
    std::make_shared<MenuItemAction>("__erase","ERASE","ERASE", factoryReset)
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
  setupButton();
  setupSerial();
  setupHW();
  setupDisplay();
  setupClock();

  setupParameters();
  loadParameters();
  setupMenu();

  setupLogo();

  DEBUG_SERIAL.printf("[Firmware] %s\n",Utils::getDeviceInfo("\n[Firmware] ").c_str());

  /* WiFi */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("--> WiFi ", -1, 20));
  connectToWiFi();
  g_current_mode = MODE::TICKER;

  g_display->cleanQueue();
  g_display->queueAction(make_shared<Display::Action::StaticText>(WiFi.SSID(), 1.0));

  /* Update */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("UPDATING... ", -1, 20));
  g_current_mode = MODE::UPDATE;
  Firmware::update(g_parameters["update_url"]);
  g_current_mode = MODE::TICKER;

  g_price_action = make_shared<Display::PriceAction>(10.0); // animation speed, in digits per second
  g_display->replaceAction(g_price_action);

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
      setAnnouncement(g_announcement, g_announcement_static, g_announcement_time, [](){ g_current_mode = MODE::TICKER; });
      g_announcement = "";
    } else if (g_current_mode==MODE::ANNOUNCEMENT) {
      if (g_announcement_static)
        replaceAnnouncement(g_announcement, g_announcement_static, g_announcement_time, [](){ g_current_mode = MODE::TICKER; });

      // ignore another message if current (non-static) message is in progress
      g_announcement = "";
    }
  }

  g_data_source->loop();
#ifdef HAS_SOUND
  g_sound->tick();
#endif
  delay(10);
}

#endif
