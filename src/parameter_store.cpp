#include <Arduino.h>
#include <EEPROM.h>

#include "parameter_store.hpp"

#define DEBUG_SERIAL Serial // TODO: const extern

ParameterStore::ParameterStore(ParameterItem *items) : m_size(0), m_items(items)
{
  // calc size
  for (int i=0; m_items[i].name!=""; ++i,++m_size)
    ;
}

void ParameterStore::debug_print(void)
{
  for (int i=0; i<m_size; ++i)
      DEBUG_SERIAL.printf("[Parameters] index: %i, name: '%s', value: '%s', description: '%s', field_length: '%i'\n",
        i, m_items[i].name.c_str(), m_items[i].value.c_str(), m_items[i].description.c_str(), m_items[i].field_length);
}

// TODO: move to utils.cpp/hpp
void eeprom_WriteString(int& offset, const String &s)
{
  int length = s.length();
  const char *c_str = s.c_str();
  for (int i=0;i<=length;++i,++offset)
    EEPROM.write(offset, c_str[i]);
}

String eeprom_ReadString(int& offset)
{
  String s="";
  while(true) {
    char c = (char) EEPROM.read(offset++);
    if (c=='\0' || c==255) break; // NULL char or uninitialized memory
    s += c;
  }
  return s;
}

void ParameterStore::loadFromEEPROM(void)
{
  DEBUG_SERIAL.println("[Parameters] Loading from EEPROM");

  int offset = c_eeprom_offset;
  String header = eeprom_ReadString(offset);
  if (header != "PARAMS") {
    DEBUG_SERIAL.printf("[Parameters] Invalid EEPROM header, using defaults\n");
    return;
  }

  while(true) {
    String name = eeprom_ReadString(offset);
    if (name=="" || name=="ENDPARAMS")
      break;
    String value = eeprom_ReadString(offset);

    auto item = findByName(name);
    if (item==nullptr) {
      DEBUG_SERIAL.printf("[Parameters] Unknown parameter '%s', ignoring\n",name.c_str());
      continue;
    }

    item->value = value;
  }

  debug_print();
}

void ParameterStore::storeToEEPROM(void)
{
  DEBUG_SERIAL.println("[Parameters] Storing to EEPROM");
  debug_print();
  int offset = c_eeprom_offset;
  eeprom_WriteString(offset, "PARAMS");
  for (int i=0;i<m_size;++i) {
    auto item = m_items[i];
    eeprom_WriteString(offset, item.name);
    eeprom_WriteString(offset, item.value);
  }
  eeprom_WriteString(offset, "ENDPARAMS");
}

ParameterItem* ParameterStore::operator[] (int index) const
{
  if (index<0 || index>=m_size) {
    DEBUG_SERIAL.printf("ParamStore: Invalid index: %i'\n", index);
    return nullptr;
  }

  return &m_items[index];
}

ParameterItem* ParameterStore::findByName(const String& name) const
{
  for(int i=0;i<m_size;++i)
    if (m_items[i].name == name)
      return &m_items[i];

  return nullptr;
}


String& ParameterStore::operator[] (const char *name) const
{
  static String notfound("");
  ParameterItem *item = findByName(name);
  if (item != nullptr)
    return item->value;

  return notfound;
}
