#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <EEPROM.h>

namespace Utils {
void eeprom_WriteString(int& offset, const String &s);
String eeprom_ReadString(int& offset, const int max_chars = 255);
void eeprom_Erase(const int offset, const int size);

}
