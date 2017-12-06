#include "parameter_store.hpp"
#include <EEPROM.h>

ParameterStore::ParameterStore(ParameterItem *items)
{
  for (int i=0;items[i].name!="";++i)
    m_items.emplace(items[i].name, &items[i]);
}

void ParameterStore::debug_print(void)
{
  for (const auto& item_pair : m_items) {
    const auto item = item_pair.second;
    DEBUG_SERIAL.printf("[Parameters] name: '%s', value: '%s', description: '%s', field_length: '%i'\n",
      item->name.c_str(), item->value.c_str(), item->description.c_str(), item->field_length);
  }
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
  for (const auto& item_pair : m_items) {
    const auto item = item_pair.second;
    eeprom_WriteString(offset, item->name);
    eeprom_WriteString(offset, item->value);
  }
  eeprom_WriteString(offset, "ENDPARAMS");
}

ParameterMap_t& ParameterStore::all_items(void)
{
  return m_items;
}

ParameterItem* ParameterStore::findByName(const String& name) const
{
  auto item = m_items.find(name);
  if (item==m_items.end()) // not found
    return nullptr;

  return (*item).second;
}


String& ParameterStore::operator[] (const char *name) const
{
  static String notfound("");
  ParameterItem *item = findByName(name);
  if (item != nullptr)
    return item->value;

  return notfound;
}
