#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager
#include "display.hpp"

extern const char* default_currency_pair;
extern const char* update_server_url;

class WiFiCore
{
public:
  WiFiCore(Display &display) : m_display(display), m_wifimanager(WiFiManager()),
    m_currency_pair(new WiFiManagerParameter("pair", "Currency pair", default_currency_pair, 10)),
    m_update_server(new WiFiManagerParameter("update_server", "Update server", update_server_url, 20))
  {
    m_wifimanager.addParameter(m_currency_pair);
    m_wifimanager.addParameter(m_update_server);
  }

  void setAPCallback(void (*func)(WiFiManager*));
  void startAP(const char *ssid_name, unsigned long timeout = 120);
  void connectToWiFiOrFallbackToAP(void);

  const char* getCurrencyPair(void) { return m_currency_pair->getValue(); }

private:
  WiFiManager m_wifimanager;
  Display &m_display;
  WiFiManagerParameter *m_currency_pair;
  WiFiManagerParameter *m_update_server;
};
