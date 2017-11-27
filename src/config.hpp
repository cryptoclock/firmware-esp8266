const int g_contrast = 64;

const char* app_version = "0.1.5";
#define NO_OTA_FIRMWARE_UPDATE 1

#include "parameter_store.hpp"

// stored parameters
ParameterItem _parameters[] = {
  ParameterItem{"currency_pair","Currency pair","BTCUSD", 10},
  ParameterItem{"update_url","Update server","update.cryptoclock.net", 50},
  ParameterItem{"ticker_server_host","Ticker server hostname","ticker.cryptoclock.net", 50},
  ParameterItem{"ticker_server_port","Ticker server port","443", 5},
  ParameterItem{"ticker_path","Ticker server url","/", 50},
  ParameterItem{"","","", 0} // EOL
};

ParameterStore g_parameters(_parameters);

// display
//#define X_DISPLAY_TM1637 1
#define X_DISPLAY_U8G2 1

#if defined(X_DISPLAY_U8G2)
#include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R2, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_DISPLAY_TM1637)
#include "display_tm1637.hpp"
  TM1637Display g_display_hw(/* clock=*/ D4, /* data=*/ D5);
#else
  #error Please configure display type in config.hpp.
#endif
