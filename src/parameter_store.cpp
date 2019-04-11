/* 
  Cryptoclock ESP8266
  Copyright (C) 2018 www.cryptoclock.net <info@cryptoclock.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "parameter_store.hpp"
#include <EEPROM.h>
#include "utils.hpp"
#include "protocol.hpp"

extern Protocol *g_protocol;

void ParameterStore::addItem(ParameterItem item)
{
  m_items.emplace(item.name, item);
}

void ParameterStore::debug_print(void)
{
  for (const auto& item_pair : m_items) {
    const auto item = item_pair.second;
    DEBUG_SERIAL.printf_P(PSTR("[Parameters] name: '%s', value: '%s', description: '%s', field_length: '%i'\n"),
      item.name.c_str(), item.value.c_str(), item.description.c_str(), item.field_length);
  }
}

void ParameterStore::loadFromEEPROMwithoutInit(void)
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
    if (item->on_change!=nullptr)
      item->on_change(*item, true, true);
  }

  debug_print();
}

void ParameterStore::storeToEEPROMwithoutInit(void)
{
  DEBUG_SERIAL.println(F("[Parameters] Storing to EEPROM"));
  debug_print();
  int offset = c_eeprom_offset;
  Utils::eeprom_WriteString(offset, "PARAMS");
  for (const auto& item_pair : m_items) {
    const auto item = item_pair.second;
    if (item.name.startsWith("__LEGACY_"))
      continue;
    Utils::eeprom_WriteString(offset, item.name);
    Utils::eeprom_WriteString(offset, item.value);
  }
  Utils::eeprom_WriteString(offset, "ENDPARAMS");
}

void ParameterStore::storeToEEPROM(void)
{
  Utils::eeprom_BEGIN();
  storeToEEPROMwithoutInit();
  Utils::eeprom_END();
}

ParameterMap_t& ParameterStore::all_items(void)
{
  return m_items;
}

ParameterItem* ParameterStore::findByName(const String& name)
{
  auto item = m_items.find(name);
  if (item==m_items.end()) // not found
    return nullptr;

  return &(*item).second;
}

String& ParameterStore::operator[] (const char *name)
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
  for (auto& item_pair : m_items) {
    auto &item = item_pair.second;
    func(&item);
  }
}

void ParameterStore::setIfExistsAndTriggerCallback(const String& name, const String& value, bool final_change)
{
  auto parameter = findByName(name);
  if (parameter) {
    parameter->value = value;
    if (parameter->on_change)
      parameter->on_change(*parameter, false, final_change);
    if (final_change)
      g_protocol->sendParameter(parameter);
  }
}
