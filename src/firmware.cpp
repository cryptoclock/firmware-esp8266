#include "config_common.hpp"
#include <Arduino.h>
#include <ESP8266httpUpdate.h>
#include "firmware.hpp"

void Firmware::update(const String &update_url)
{
  if (update_url.length() < 3 || update_url=="") {
    DEBUG_SERIAL.println("[Update] Update url empty, skipping");
    return;
  }

  String url = "http://" + update_url + "/esp/update?md5=" + ESP.getSketchMD5();
  //t_httpUpdate_return ret = ESPhttpUpdate.update(url,"","AF B9 78 3B E6 1D 70 AE E7 97 0A 50 D8 7B 1C 89 83 90 32 30");
  DEBUG_SERIAL.printf("Update URL: '%s'\n",url.c_str());
  t_httpUpdate_return ret = ESPhttpUpdate.update(url);

  switch(ret) {
    case HTTP_UPDATE_FAILED:
      DEBUG_SERIAL.println("[update] Update failed.");
      DEBUG_SERIAL.printf("[update] HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
      break;
    case HTTP_UPDATE_NO_UPDATES:
      DEBUG_SERIAL.println("[update] Update no Update.");
      break;
    case HTTP_UPDATE_OK:
      DEBUG_SERIAL.println("[update] Update ok."); // may not called we reboot the ESP
      break;
  }
}
