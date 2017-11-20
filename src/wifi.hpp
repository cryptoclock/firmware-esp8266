#include "config_common.hpp"
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "display.hpp"
#include "aplist.hpp"

class WiFiCore
{
public:
  WiFiCore(Display *display, AP_list *ap_list);

  void setAPCallback(void (*func)(WiFiManager*));
  void startAP(const char *ssid_name, unsigned long timeout = 120);
  void connectToWiFiOrFallbackToAP(void);
  void resetSettings(void);

  WiFiManager* getWiFiManager(void) { return &m_wifimanager; }

  static void saveCallback(void);

  void updateParametersFromAP(WiFiManager *manager); // reads parameters from AP config page and updates g_parameters
private:
  WiFiManager m_wifimanager;
  Display *m_display;
  AP_list *m_ap_list;
  WiFiManagerParameter **m_parameters;
  int m_parameters_size;
};
