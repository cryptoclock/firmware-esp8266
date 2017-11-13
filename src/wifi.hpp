#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "display.hpp"
#include "aplist.hpp"

extern const char* default_currency_pair;
extern const char* update_server_url;

class WiFiCore
{
public:
  WiFiCore(Display &display, AP_list &ap_list) : m_display(display), m_ap_list(ap_list), m_wifimanager(WiFiManager()),
    m_currency_pair(new WiFiManagerParameter("pair", "Currency pair", default_currency_pair, 10)),
    m_update_server(new WiFiManagerParameter("update_server", "Update server", update_server_url, 20))
  {
    // TODO: read/save custom data on eeprom

    m_wifimanager.addParameter(m_currency_pair);
    m_wifimanager.addParameter(m_update_server);

    m_wifimanager.setSaveConfigCallback(&saveCallback);

    m_ap_list.readFromEEPROM();
    m_ap_list.addAPsToWiFiManager(&m_wifimanager);
  }

  void setAPCallback(void (*func)(WiFiManager*));
  void startAP(const char *ssid_name, unsigned long timeout = 120);
  void connectToWiFiOrFallbackToAP(void);
  void resetSettings(void);

  const char* getCurrencyPair(void) { return m_currency_pair->getValue(); }
  WiFiManager* getWiFiManager(void) { return &m_wifimanager; }

  static void saveCallback(void);
private:
  WiFiManager m_wifimanager;
  Display &m_display;
  AP_list &m_ap_list;
  WiFiManagerParameter *m_currency_pair;
  WiFiManagerParameter *m_update_server;
};
