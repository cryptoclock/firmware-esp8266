const char* app_version = "0.2.2";

#include "parameter_store.hpp"

// stored parameters
ParameterItem _parameters[] = {
  ParameterItem{"currency_pair","Currency pair","BTCUSD", 10},
  ParameterItem{"update_url","Update server","update.cryptoclock.net", 50},
  ParameterItem{"ticker_server_host","Ticker server hostname","ticker.cryptoclock.net", 50},
  ParameterItem{"ticker_server_port","Ticker server port","443", 5},
  ParameterItem{"ticker_path","Ticker server url","/", 50},
  ParameterItem{"brightness","Brightness","127", 5},
  ParameterItem{"","","", 0} // EOL
};

ParameterStore g_parameters(_parameters);

// display test mode
//#define X_TEST_DISPLAY 1

// -- display setup --
//#define X_DISPLAY_TM1637 1
//#define X_DISPLAY_LIXIE 1
#define X_DISPLAY_U8G2 1
//#define X_DISPLAY_NEOPIXEL 1

#define X_DISPLAY_DEFAULT_ROTATION U8G2_R0

#if defined(X_DISPLAY_U8G2)
#include "display_u8g2.hpp"
  const int g_display_width = 32;
  const int g_display_height = 8;
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(X_DISPLAY_DEFAULT_ROTATION, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_DISPLAY_TM1637)
#include "display_tm1637.hpp"
  const int g_display_num_digits = 4;
  TM1637Display g_display_hw(/* clock=*/ D4, /* data=*/ D5);
#elif defined(X_DISPLAY_LIXIE)
#include "display_lixie.hpp"
  const int g_display_num_digits = 6;
  const uint8_t LIXIE_PIN = /* data=*/ D5;
  Lixie g_display_hw;
#elif defined(X_DISPLAY_NEOPIXEL)
#include "display_neopixel.hpp"
  const int g_display_num_leds = 64;
  const uint8_t NEOPIXEL_PIN = /* data*/ 5;
#else
  #error Please configure display type in config.hpp.
#endif
