#include "firmware.hpp"
#include <ESP8266httpUpdate.h>

#include "parameter_store.hpp"

extern ParameterStore g_parameters;

void update_firmware(void) {
  String url = "http://" + g_parameters["update_url"] + "/esp/update?md5=" + ESP.getSketchMD5();
  t_httpUpdate_return ret = ESPhttpUpdate.update(url);
  DEBUG_SERIAL.printf("Update URL: '%s'\n",url.c_str());

  switch(ret) {
    case HTTP_UPDATE_FAILED:
        DEBUG_SERIAL.println("[update] Update failed.");
        DEBUG_SERIAL.printf("HTTP_UPDATE_FAILED Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
    case HTTP_UPDATE_NO_UPDATES:
        DEBUG_SERIAL.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:
        DEBUG_SERIAL.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}
