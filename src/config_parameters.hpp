// stored parameters
ParameterItem _parameters[] = {
  ParameterItem{"__device_uuid","","", 0}, // new uuid will be generated on every device wipe
  ParameterItem{"currency_pair","Currency pair","BTCUSD", 50},
  ParameterItem{"update_url","Update server","update.cryptoclock.net", 50},
  ParameterItem{"ticker_server_host","Ticker server hostname","ticker.cryptoclock.net", 50},
  ParameterItem{"ticker_server_port","Ticker server port","443", 5},
  ParameterItem{"ticker_path","Ticker server url","/", 50},
  ParameterItem{"brightness","Brightness (0-15)","15", 5},
  ParameterItem{"font","Font (0-2)","0", 5},
  ParameterItem{"rotate_display","Rotate Display (0,1)","0", 5},
  ParameterItem{"","","", 0} // EOL
};
ParameterStore g_parameters(_parameters);
