#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

const int AP_SSID_MAX_LENGTH = 32+1;
const int AP_PASSWORD_MAX_LENGTH = 64+1;

struct AP {
  char ssid[AP_SSID_MAX_LENGTH];
  char password[AP_PASSWORD_MAX_LENGTH];
};

class AP_list {
public:
  AP_list();

  void clear(void);
  void clearItem(int index);
  void addToTop(const String &SSID, const String &password);
  void moveToTop(int index);
  void readFromEEPROM(void);
  void storeToEEPROM(void);
  void printAPs(void);

  int getIndexBySSID(const String &SSID);
  void addAPsToWiFiManager(WiFiManager *manager);

private:
  static int const c_eeprom_offset = 0;
  static int const c_max_stored_aps = 10;

  AP m_aps[c_max_stored_aps];
};
