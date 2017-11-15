#include <ESP8266httpUpdate.h>

#include "parameter_store.hpp"

extern ParameterStore g_parameters;

void update_firmware(void) {
  t_httpUpdate_return ret = ESPhttpUpdate.update("http://" + g_parameters["update_url"] + "/esp/update?md5=" + ESP.getSketchMD5());
  switch(ret) {
    case HTTP_UPDATE_FAILED:
        Serial.println("[update] Update failed.");
        Serial.printf("HTTP_UPDATE_FAILD Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[update] Update no Update.");
        break;
    case HTTP_UPDATE_OK:
        Serial.println("[update] Update ok."); // may not called we reboot the ESP
        break;
  }
}
