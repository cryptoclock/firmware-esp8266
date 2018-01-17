#include "parameter_store.hpp"
#include <EEPROM.h>
#include "utils.hpp"

ParameterStore::ParameterStore(ParameterItem *items)
{
  for (int i=0;items[i].name!="";++i)
    m_items.emplace(items[i].name, &items[i]);
}

void ParameterStore::debug_print(void)
{
  for (const auto& item_pair : m_items) {
    const auto item = item_pair.second;
    DEBUG_SERIAL.printf_P(PSTR("[Parameters] name: '%s', value: '%s', description: '%s', field_length: '%i'\n"),
      item->name.c_str(), item->value.c_str(), item->description.c_str(), item->field_length);
  }
}

void ParameterStore::loadFromEEPROM(void)
{
  DEBUG_SERIAL.println(F("[Parameters] Loading from EEPROM"));

  int offset = c_eeprom_offset;
  String header = Utils::eeprom_ReadString(offset);
  if (header != "PARAMS") {
    DEBUG_SERIAL.println(F("[Parameters] Invalid EEPROM header, using defaults\n"));
    return;
  }

  while(true) {
    String name = Utils::eeprom_ReadString(offset);
    if (name=="" || name=="ENDPARAMS")
      break;
    String value = Utils::eeprom_ReadString(offset);

    auto item = findByName(name);
    if (item==nullptr) {
      item = findByName("__LEGACY_"+name);
      if (item==nullptr) {
        DEBUG_SERIAL.printf_P(PSTR("[Parameters] Unknown parameter '%s', ignoring\n"),name.c_str());
        continue;
      }
      DEBUG_SERIAL.printf_P(PSTR("[Parameters] Legacy parameter '%s', value '%s'\n"),name.c_str(), value.c_str());
    }

    item->value = value;
  }

  debug_print();
}

void ParameterStore::storeToEEPROM(void)
{
  DEBUG_SERIAL.println(F("[Parameters] Storing to EEPROM"));
  debug_print();
  int offset = c_eeprom_offset;
  Utils::eeprom_WriteString(offset, "PARAMS");
  for (const auto& item_pair : m_items) {
    const auto item = item_pair.second;
    if (item->name.startsWith("__LEGACY_"))
      continue;
    Utils::eeprom_WriteString(offset, item->name);
    Utils::eeprom_WriteString(offset, item->value);
  }
  Utils::eeprom_WriteString(offset, "ENDPARAMS");
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

bool ParameterStore::setValue(const String& name, const String& value)
{
  auto parameter = findByName(name);
  if (parameter == nullptr)
    return false;

  parameter->value = value;
  return true;
}

void ParameterStore::iterateAllParameters(parameter_iterate_func_t func)
{
  for (const auto& item_pair : m_items) {
    const auto item = item_pair.second;
    func(item);
  }
}
