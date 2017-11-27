#include "parameter_store.hpp"
#include "wifi.hpp"
#include <EEPROM.h>

extern ParameterStore g_parameters;
extern AP_list *g_APs;
extern WiFiCore *g_wifi;

WiFiCore::WiFiCore(Display *display, AP_list *ap_list) :
  m_display(display), m_ap_list(ap_list), m_wifimanager(WiFiManager())
{
  m_parameters_size = g_parameters.size();
  m_parameters = new WiFiManagerParameter*[m_parameters_size];

  for(int i=0;i<m_parameters_size;++i) {
    auto *param = g_parameters[i];
    m_parameters[i] = new WiFiManagerParameter(param->name.c_str(), param->description.c_str(), param->value.c_str(), param->field_length);
    m_wifimanager.addParameter(m_parameters[i]);
  }

  m_wifimanager.setSaveConfigCallback(&saveCallback);

  m_ap_list->readFromEEPROM();
  m_ap_list->addAPsToWiFiManager(&m_wifimanager);

  m_ev_conn = WiFi.onStationModeConnected(onConnect);
  m_ev_disconn = WiFi.onStationModeDisconnected(onDisconnect);
  m_ev_gotip = WiFi.onStationModeGotIP(onGotIP);
}

void WiFiCore::setAPCallback(void (*func)(WiFiManager*))
{
  m_wifimanager.setAPCallback(func);
}

void WiFiCore::connectToWiFiOrFallbackToAP(void)
{
  if (!m_wifimanager.autoConnect()) {
    DEBUG_SERIAL.println("[WiFiCore] Failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
}

void WiFiCore::startAP(const char *ssid_name, unsigned long timeout)
{
  m_wifimanager.setTimeout(timeout);
  if (!m_wifimanager.startConfigPortal(ssid_name)) {
    DEBUG_SERIAL.println("[WiFiCore] Failed to start AP and hit timeout");
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

void WiFiCore::updateParametersFromAP(WiFiManager *manager)
{
  // save parameters
  for(int i=0;i<m_parameters_size;++i) {
    String id = m_parameters[i]->getID();
    String new_value = m_parameters[i]->getValue();
    auto *param = g_parameters.findByName(id);
    if (param)
      param->value = new_value;
  }
}

void WiFiCore::saveCallback(void)
{
  DEBUG_SERIAL.println("[WiFiCore] Save callback called");

  EEPROM.begin(2048);

  // save APs
  auto manager = g_wifi->getWiFiManager();
  for (int i=0;;++i) {
    auto credentials = manager->getAP(i);
    if(credentials == NULL)
      break;

    g_APs->addToTop(credentials->ssid, credentials->pass);
  }
  g_APs->storeToEEPROM();

  // save parameters
  g_wifi->updateParametersFromAP(manager);
  g_parameters.storeToEEPROM();

  EEPROM.commit();
  EEPROM.end();
}

void WiFiCore::onConnect(WiFiEventStationModeConnected event_info)
{
  DEBUG_SERIAL.printf("[WiFiCore] Connected to SSID: %s channel %i\n",
    event_info.ssid.c_str(), event_info.channel);
}

void WiFiCore::onDisconnect(WiFiEventStationModeDisconnected event_info)
{
  DEBUG_SERIAL.printf("[WiFiCore] Disconnected from SSID: %s\n", event_info.ssid.c_str());
  DEBUG_SERIAL.printf("[WiFiCore] Reason: %d\n", event_info.reason);
}

void WiFiCore::onGotIP(WiFiEventStationModeGotIP ipInfo)
{
  DEBUG_SERIAL.printf("[WiFiCore] Got IP: %s Gateway: %s, Mask: %s\n",
    ipInfo.ip.toString().c_str(), ipInfo.gw.toString().c_str(), ipInfo.mask.toString().c_str()
  );
}
