#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <map>
#include <functional>

struct ParameterItem;

typedef std::function<void(ParameterItem& item, bool init)> parameter_onchange_t;

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
  void setIfExistsAndTriggerCallback(const String& name, const String& value);

  void iterateAllParameters(parameter_iterate_func_t func);
private:
  ParameterMap_t m_items;

  static int const c_eeprom_offset = 1024;
};
