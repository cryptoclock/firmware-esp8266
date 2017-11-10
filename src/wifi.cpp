#include "wifi.hpp"

void WiFiCore::setAPCallback(void (*func)(WiFiManager*))
{
  m_wifimanager.setAPCallback(func);
}

void WiFiCore::connectToWiFiOrFallbackToAP(void)
{
  if (!m_wifimanager.autoConnect()) {
    DEBUG_SERIAL.println("Failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
}

void WiFiCore::startAP(const char *ssid_name, unsigned long timeout)
{
  m_wifimanager.setTimeout(timeout);
  if (!m_wifimanager.startConfigPortal(ssid_name)) {
    DEBUG_SERIAL.println("Failed to start AP and hit timeout");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
  m_wifimanager.setTimeout(0);
}
