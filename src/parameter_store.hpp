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

/* 
  Class for Flash parameter storage
*/

#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <map>
#include <functional>

struct ParameterItem;

typedef std::function<void(ParameterItem& item, bool init, bool final_change)> parameter_onchange_t;

struct ParameterItem {
  String name;
  String description;
  String value;
  int field_length;
  parameter_onchange_t on_change;
};

typedef std::map<String, ParameterItem> ParameterMap_t;
typedef std::function<void(const ParameterItem*)> parameter_iterate_func_t;

class ParameterStore
{
public:
  ParameterStore() {}

  void addItem(ParameterItem item);

  void loadFromEEPROMwithoutInit(void);
  void storeToEEPROMwithoutInit(void);
  void storeToEEPROM(void);

  void debug_print(void);

  ParameterMap_t& all_items(void);

  bool setValue(const String& name, const String& value);
  String& operator[] (const char *name);
  ParameterItem* findByName(const String& name);
  void setIfExistsAndTriggerCallback(const String& name, const String& value, bool final_change);

  void iterateAllParameters(parameter_iterate_func_t func);
private:
  ParameterMap_t m_items;

  static int const c_eeprom_offset = 1024;
};
