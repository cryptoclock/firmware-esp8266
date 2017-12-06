#include "config_common.hpp"
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include <vector>
#include "display.hpp"
#include "aplist.hpp"

class WiFiCore
{
public:
  WiFiCore(DisplayT *display, AP_list *ap_list);

  void setAPCallback(void (*func)(WiFiManager*));
  void startAP(const String& ssid_name, unsigned long timeout = 120);
  void connectToWiFiOrFallbackToAP(void);
  void resetSettings(void);

  WiFiManager* getWiFiManager(void) { return &m_wifimanager; }

  static void saveCallback(void);
  static void onConnect(WiFiEventStationModeConnected event_info);
  static void onDisconnect(WiFiEventStationModeDisconnected event_info);
  static void onGotIP(WiFiEventStationModeGotIP ipInfo);

  void updateParametersFromAP(WiFiManager *manager); // reads parameters from AP config page and updates g_parameters
private:
  WiFiManager m_wifimanager;
  DisplayT *m_display;
  AP_list *m_ap_list;
  std::vector<WiFiManagerParameter*> m_parameters;

  WiFiEventHandler m_ev_conn, m_ev_disconn, m_ev_gotip;
};
