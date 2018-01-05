#include "config_common.hpp"
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

class AP_list {
public:
  static void addAPsToWiFiManager(WiFiManager *manager);
  static void saveAPsToEEPROM(WiFiManager *manager);
};
