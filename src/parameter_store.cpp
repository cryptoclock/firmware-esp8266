#include <Arduino.h>
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
      DEBUG_SERIAL.printf("index: %i, name: '%s', value: '%s', description: '%s', field_length: '%i'\n",
        i, m_items[i].name.c_str(), m_items[i].value.c_str(), m_items[i].description.c_str(), m_items[i].field_length);
}

void ParameterStore::loadFromEEPROM(void)
{
  debug_print();
  // TODO
}

void ParameterStore::storeToEEPROM(void)
{
  debug_print();
  // TODO
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
