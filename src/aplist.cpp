#include "aplist.hpp"
#include <EEPROM.h>
#include "utils.hpp"

const int AP_SSID_MAX_LENGTH = 32+1;
const int AP_PASSWORD_MAX_LENGTH = 64+1;

const int AP_EEPROM_OFFSET = 0;
const int AP_MAX_STORED_APS = 10;

struct AP {
  char ssid[AP_SSID_MAX_LENGTH];
  char password[AP_PASSWORD_MAX_LENGTH];
};

void AP_list::addAPsToWiFiManager(WiFiManager *manager)
{
  DEBUG_SERIAL.println(F("[APs] Adding APs to WiFiManager"));

  int offset = AP_EEPROM_OFFSET;

  char header[3] = {0};
  EEPROM.get(offset,header);
  offset += sizeof(header);
  if (memcmp(header,"APs",sizeof(header))!=0) {
    DEBUG_SERIAL.println(F("[APs] Invalid EEPROM header, ignoring content"));
    return;
  }

  for(int i=0;i<AP_MAX_STORED_APS;++i) {
    AP ap = {0};
    EEPROM.get(offset, ap);
    offset += sizeof(ap);
    if(ap.ssid[0]=='\0')
      break;
    DEBUG_SERIAL.printf_P(PSTR("[APs] SSID: %s Password: [redacted]\n"), ap.ssid, ap.password);
    manager->addAP(strdup(ap.ssid), strdup(ap.password));
  }
}

void AP_list::saveAPsToEEPROM(WiFiManager *manager)
{
  DEBUG_SERIAL.println(F("[APs] Storing APs to EEPROM"));

  int offset = AP_EEPROM_OFFSET;

  Utils::eeprom_Erase(offset, (AP_MAX_STORED_APS+1) * sizeof(AP));

  EEPROM.put(offset, "APs");
  offset += 3;

  for (int i=0;i<AP_MAX_STORED_APS;++i) {
    auto credentials = manager->getAP(i);
    if(credentials == NULL)
      break;
    AP ap = {0};
    strncpy(ap.ssid, credentials->ssid.c_str(), AP_SSID_MAX_LENGTH-1);
    strncpy(ap.password, credentials->pass.c_str(), AP_PASSWORD_MAX_LENGTH-1);

    DEBUG_SERIAL.printf_P(PSTR("[APs] Writing SSID: %s Password: [redacted]\n"), ap.ssid);
    EEPROM.put(offset, ap);
    offset += sizeof(ap);
  }
}
