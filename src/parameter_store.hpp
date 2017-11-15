#include "config_common.hpp"
#include <Arduino.h>

struct ParameterItem {
  String name;
  String description;
  String value;
  int field_length;
};

class ParameterStore
{
public:
  ParameterStore(ParameterItem *items);

  void loadFromEEPROM(void);
  void storeToEEPROM(void);

  void debug_print(void);

  int size(void) { return m_size; }

  ParameterItem* operator[] (int index) const;
  String& operator[] (const char *name) const;
  ParameterItem* findByName(const String& name) const;

private:
  ParameterItem *m_items;
  int m_size;

  static int const c_eeprom_offset = 1024;
};
