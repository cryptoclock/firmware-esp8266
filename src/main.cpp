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
#include "display_action_multi.hpp"
using Display::Coords;

#include "config.hpp"

#include "firmware.hpp"
#include "wifi.hpp"
#include "utils.hpp"
#include "button.hpp"
#include "menu.hpp"
#include "data_source.hpp"
#include "bitmaps.hpp"

#include <EEPROM.h>

// NTP
#include <TimeLib.h>
#include <Time.h>
#include <NtpClientLib.h>

#include <Ticker.h>

#include <ESP8266TrueRandom.h>


ParameterStore g_parameters;

Ticker g_ticker_clock;

DisplayT *g_display;
shared_ptr<Display::PriceAction> g_price_action;
shared_ptr<Display::Action::Clock> g_clock_action;
WiFiCore *g_wifi;
DataSource *g_data_source;

shared_ptr<Button> g_flash_button;

bool g_start_ondemand_ap = false;
bool g_force_wipe = false;

enum class MODE { TICKER, MENU, ANNOUNCEMENT, OTP};
MODE g_current_mode(MODE::TICKER);

shared_ptr<Menu> g_menu = nullptr;

String g_announcement="";
shared_ptr<Display::ActionT> g_announcement_action;

void clock_callback()
{
  if (g_current_mode != MODE::TICKER)
    return;

  if (g_parameters["clock_mode"].toInt() == 0) // Off
    return;

  auto time = NTP.getTimeDateString();

  if (g_clock_action->isAlwaysOn()) {
    g_clock_action->updateTime(time);
    g_display->prependAction(g_clock_action);
    return;
  }

#if !defined(X_CLOCK_ONLY_DISPLAY)
  if (time=="Time not set")
    return;
#endif

  DEBUG_SERIAL.printf_P(PSTR("[NTP] Displaying time %s\n"),  time.c_str());
  g_clock_action->updateTime(time);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(g_clock_action, 1, g_price_action, 2, 0.5, Coords{0,+1})
  );
  g_display->prependAction(g_clock_action);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(g_price_action, 2, g_clock_action, 1, 0.5, Coords{0,-1})
  );
}

void setAnnouncement(const String& message, action_callback_t onfinished_cb)
{
  g_current_mode = MODE::ANNOUNCEMENT;

  auto current_action = g_display->getTopAction();
  g_announcement_action = make_shared<Display::Action::RotatingTextOnce>(message,20,Coords{0,0},onfinished_cb);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(nullptr, 1, current_action, 1, 0.5, Coords{-1,0})
  );
  g_display->prependAction(g_announcement_action);
  g_display->prependAction(
    make_shared<Display::Action::SlideTransition>(current_action, 1, nullptr, 1, 0.5, Coords{-1,0})
  );
}

void configModeCallback (WiFiManager *myWiFiManager)
{
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
  g_parameters.addItem({"brightness","Brightness (0-15)","15", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    int brightness = std::min(std::max(item.value.toInt(),0L),15L); // sanitize
    item.value = String(brightness);
    g_display->setDisplayBrightness(brightness * 16);
  }});
  g_parameters.addItem({"font","Font (0-2)","0", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    int font = std::min(std::max(item.value.toInt(),0L),2L);
    item.value = String(font);
    g_display->setFont(font);
  }});
  g_parameters.addItem({"rotate_display","Rotate Display (0,1)","0", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    int rotate = std::min(std::max(item.value.toInt(),0L),1L);
    item.value = String(rotate);
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
  g_parameters.addItem({"timezone","Timezone (-11..+13)","1", 5, [](ParameterItem& item, bool init, bool final_change)
  {
    if (final_change) {
      int timezone = std::min(std::max(item.value.toInt(),-11L),13L);
      item.value = String(timezone);
      NTP.stop();
      NTP.begin(NTP_SERVER, timezone, true);
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
  pinMode(BUILTIN_LED, OUTPUT);
  digitalWrite(BUILTIN_LED, true); // high = off
}

void setupDataSource()
{
  g_data_source = new DataSource;

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
    g_price_action->setATHPrice(price);
  });

  g_data_source->setOnPriceChange([&](const String& price){
    auto currentPrice = Price(price);
    currentPrice.debug_print();
    g_price_action->updatePrice(price);
    DEBUG_SERIAL.printf_P(PSTR("[SYSTEM] Free Heap: %i\n"), ESP.getFreeHeap());
  });

  g_data_source->connect();
}

void setupLogo()
{
  auto logo_top = make_shared<Display::Action::StaticBitmap>(s_crypto2_bits, 32, 8, 1.5);
  auto logo_bottom = make_shared<Display::Action::StaticBitmap>(s_clock_inverted_bits, 32, 8, 3.5);
  g_display->queueAction(logo_top);
  g_display->queueAction(make_shared<Display::Action::SlideTransition>(logo_top, 1, logo_bottom, 1, 0.5, Coords{0,-1}));
  g_display->queueAction(logo_bottom);
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
  int timezone = g_parameters["timezone"].toInt();
  NTP.begin(NTP_SERVER, timezone, true);
  NTP.setInterval(1800);

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
  g_wifi->startAP("OnDemandAP_"+String(ESP.getChipId()), 120);
  ESP.restart();
}

void switchMenu();
void setupDefaultButtons()
{
  g_flash_button->onShortPress(switchMenu);
  g_flash_button->onLongPress([]() { g_start_ondemand_ap = true; });
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
  static const double otp_timeout = 180.0;
  bool result = g_data_source->sendOTPRequest();
  g_display->prependAction(
    make_shared<Display::Action::StaticText>((result ? "-OK-" : "Failed"),2.0)
  );
  g_menu->end();

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

  g_flash_button->onShortPress([](){
    if (g_current_mode == MODE::OTP)
      forceSetTickerMode();
    else
      setupDefaultButtons();
  });
}

void displayDeviceInfo()
{
  String info= "v" FIRMWARE_VERSION;
  info += " " __DATE__ " " __TIME__;
  info += " MD5: " + ESP.getSketchMD5().substring(0,6);
  info += " UUID: " + g_parameters["__device_uuid"].substring(0,6);
  info += " SDK: " + String(ESP.getSdkVersion());
  setAnnouncement(info, [](){g_menu->end();});
}

void setupMenu()
{
  const menu_items_t items({
    std::make_shared<MenuItemAction>("__otp","OTP","OTP", sendOTPRequest),
    std::make_shared<MenuItemAction>("__info","Info","Info", displayDeviceInfo),
    std::make_shared<MenuItemNumericRange>("font","Font", "Font",0,2, 0, nullptr),
    std::make_shared<MenuItemNumericRange>("brightness","Bright", "Bri",0,15, 0, nullptr),
    std::make_shared<MenuItemBoolean>("rotate_display","Rotate", "Rot", false, "^^^","^^^", nullptr),
    std::make_shared<MenuItemEnum>("clock_mode","Clock", "C", false, vector<String>{"Off","On","Only"}, nullptr),
    std::make_shared<MenuItemNumericRange>("timezone","Tzone", "Tz",-11,+13, 0, nullptr)
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
  setupHW();
  setupDisplay();
  setupClock();

  setupParameters();
  loadParameters();
  setupMenu();

  setupLogo();

  /* WiFi */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("--> WiFi ", -1, 20));
  connectToWiFi();
  g_display->removeBottomAction();
  g_display->queueAction(make_shared<Display::Action::StaticText>(WiFi.SSID(), 1.0));

  /* Update */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("UPDATING... ", -1, 20));
  Firmware::update(g_parameters["update_url"]);

  g_price_action = make_shared<Display::PriceAction>(10.0); // animation speed, in digits per second
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
      setAnnouncement(g_announcement, [](){ g_current_mode = MODE::TICKER; });
      g_announcement = "";
    }
  }

  g_data_source->loop();
  delay(10);
}

#endif
