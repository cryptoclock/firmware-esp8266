#include "parameter_store.hpp"
#include "wifi.hpp"
#include <EEPROM.h>

extern ParameterStore g_parameters;
extern AP_list *APs;
extern WiFiCore *wifi;

WiFiCore::WiFiCore(Display &display, AP_list *ap_list) :
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
}

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
  DEBUG_SERIAL.println("Save callback called");

  // save APs
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

  // save parameters
  wifi->updateParametersFromAP(manager);
  g_parameters.storeToEEPROM();

  EEPROM.commit();
}
