#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <EEPROM.h>

namespace Utils {
void eeprom_BEGIN();
void eeprom_END();
void eeprom_WIPE();
void eeprom_WriteString(int& offset, const String &s);
String eeprom_ReadString(int& offset, const int max_chars = 255);
void eeprom_Erase(const int offset, const int size);
void parseURL(String url, String &server, int &port, String& path, String& protocol);
String urlChangePath(String url, const String& new_path);
String shortenText(const String& text, const int lead);
}
