#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <map>

struct ParameterItem {
  String name;
  String description;
  String value;
  int field_length;
};

typedef std::map<String, ParameterItem*> ParameterMap_t;

class ParameterStore
{
public:
  ParameterStore(ParameterItem *items);

  void loadFromEEPROM(void);
  void storeToEEPROM(void);

  void debug_print(void);

//  int size(void) { return m_size; }
  ParameterMap_t& all_items(void);

  bool setValue(const String& name, const String& value);
  String& operator[] (const char *name) const;
  ParameterItem* findByName(const String& name) const;
  bool setValue(const String& name, const String& value);

private:
  ParameterMap_t m_items;

  static int const c_eeprom_offset = 1024;
};
