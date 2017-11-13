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

void WiFiCore::resetSettings()
{
  m_wifimanager.resetSettings();
}

extern AP_list *APs;
extern WiFiCore *wifi;

void WiFiCore::saveCallback(void) {
  DEBUG_SERIAL.println("Save callback called");

  auto manager = wifi->getWiFiManager();
  for (int i=0;;++i) {
    auto credentials = manager->getAP(i);
    if(credentials == NULL)
      break;

    DEBUG_SERIAL.printf("credentials: %s - %s",
      credentials->ssid.c_str(), credentials->pass.c_str() );

    APs->addToTop(credentials->ssid, credentials->pass);
  }
  APs->storeToEEPROM();
}
