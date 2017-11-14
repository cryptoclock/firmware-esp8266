#include <ESP8266httpUpdate.h>

extern const char *update_server_url;

void update_firmware() {
  t_httpUpdate_return ret = ESPhttpUpdate.update(String("http://") + update_server_url + "/esp/update?md5=" + ESP.getSketchMD5());
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
