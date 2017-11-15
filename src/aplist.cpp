#include "aplist.hpp"
#include <EEPROM.h>

#define DEBUG_SERIAL Serial

// TODO: onsave -> write to AP_list

AP_list::AP_list() {
  clear();
}

void AP_list::clear(void)
{
  for(int i=0;i<c_max_stored_aps;++i)
    clearItem(i);
}

void AP_list::addToTop(const String &SSID, const String &password)
{
  int index = getIndexBySSID(SSID);
  if (index >= 0 && index <5) {
    DEBUG_SERIAL.printf("[APs] Add: SSID %s already in list\n",SSID.c_str());
    return;
  }
  if (index>=5) {
    DEBUG_SERIAL.printf("[APs] Add: SSID %s already in list, but not in top 5, bumping to top\n",SSID.c_str());
    moveToTop(index);
    return;
  }
  DEBUG_SERIAL.printf("[APs] Add: SSID %s not in list, adding\n",SSID.c_str());

  // bump down the list
  for (int i=c_max_stored_aps - 1;i>0;--i)
    m_aps[i] = m_aps[i-1];

  // add to top
  clearItem(0);
  strcpy(m_aps[0].ssid,SSID.c_str());
  strcpy(m_aps[0].password,password.c_str());
}

void AP_list::moveToTop(int index)
{
  if (index<1 || index > (c_max_stored_aps-1)) return;

  AP temp = m_aps[index];

  for (int i=index;i>0;--i)
    m_aps[i] = m_aps[i-1];

  m_aps[0] = temp;
}

void AP_list::clearItem(int index)
{
  if (index<0 || index > (c_max_stored_aps-1)) return;
  memset(&m_aps[index], sizeof(AP),0);
}

void AP_list::readFromEEPROM(void)
{
  char header[4];
  EEPROM.get(c_eeprom_offset,header);
  if (memcmp(header,"APs",3)!=0) {
    DEBUG_SERIAL.println("[APs] Invalid EEPROM header, ignoring content");
    return;
  }

  DEBUG_SERIAL.printf("[APs] Reading from EEPROM at offset %i\n",c_eeprom_offset);
  EEPROM.get(c_eeprom_offset + 3, m_aps);
  printAPs();
}

void AP_list::printAPs(void)
{
  for (int i=0;i<c_max_stored_aps;++i) {
    if (m_aps[i].ssid[0] == '\0') continue;
    DEBUG_SERIAL.printf("[APs] AP #%i -> SSID: %s Password: %s\n",
            i, m_aps[i].ssid, m_aps[i].password); // TODO: redact password
  }
}

void AP_list::storeToEEPROM(void)
{
  DEBUG_SERIAL.printf("[APs] Storing to EEPROM at offset %i\n",c_eeprom_offset);
  printAPs();
  EEPROM.put(c_eeprom_offset, "APs");
  EEPROM.put(c_eeprom_offset + 3, m_aps);
//  EEPROM.commit();
}

int AP_list::getIndexBySSID(const String &SSID)
{
  for (int i=0;i<c_max_stored_aps;++i) {
    if (m_aps[i].ssid[0] == '\0') continue;
    if (String(m_aps[i].ssid) == SSID)
      return i;
  }
  return -1;
}

void AP_list::addAPsToWiFiManager(WiFiManager *manager)
{
  DEBUG_SERIAL.printf("[APs] Adding APs to WiFiManager\n");
  for (int i=0;i<c_max_stored_aps;++i) {
    if (m_aps[i].ssid[0] == '\0') continue;
    manager->addAP(strdup(m_aps[i].ssid), strdup(m_aps[i].password));
  }
}
