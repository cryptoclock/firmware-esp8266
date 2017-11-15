const int contrast = 64;

const char* app_version = "0.1.3";

#include "parameter_store.hpp"

ParameterItem _parameters[] = {
  ParameterItem{"currency_pair","Currency pair","BTCUSD", 10},
  ParameterItem{"update_url","Update server","update.cryptoclock.zde.cz", 50},
  ParameterItem{"ticker_server_host","Ticker server hostname","ticker.cryptoclock.zde.cz", 50},
  ParameterItem{"ticker_server_port","Ticker server port","443", 5},
  ParameterItem{"ticker_path","Ticker server url","/", 50},
  ParameterItem{"","","", 0} // EOL
};

ParameterStore g_parameters(_parameters);
