// stored parameters
ParameterItem _parameters[] = {
  ParameterItem{"__device_uuid","","", 0}, // new uuid will be generated on every device wipe
  ParameterItem{"update_url","Update server","update.cryptoclock.net", 50},
  ParameterItem{"ticker_url","Ticker server","wss://ticker.cryptoclock.net:443/", 100},
  ParameterItem{"brightness","Brightness (0-15)","15", 5},
  ParameterItem{"font","Font (0-2)","0", 5},
  ParameterItem{"rotate_display","Rotate Display (0,1)","0", 5},

  ParameterItem{"__LEGACY_ticker_server_host","","", 0},
  ParameterItem{"__LEGACY_ticker_server_port","","", 0},
  ParameterItem{"__LEGACY_ticker_path","","", 0},
  ParameterItem{"__LEGACY_currency_pair","","", 0},

  ParameterItem{"","","", 0} // EOL
};
ParameterStore g_parameters(_parameters);
