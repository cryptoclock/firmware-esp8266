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
  Utility functions
*/

#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <EEPROM.h>

namespace Utils {
// case-independent (ci) compare_less binary function for STL containers
struct CaseInsensitiveCompare
{
  struct NocaseCompare
  {
    bool operator() (const unsigned char& c1, const unsigned char& c2) const {
      return tolower (c1) < tolower (c2); 
    }
  };
  bool operator() (const String& s1, const String& s2) const {
    return s1.equalsIgnoreCase(s2);
  }
};

void eeprom_BEGIN();
void eeprom_END();
void eeprom_WIPE();
void eeprom_WriteString(int& offset, const String &s);
String eeprom_ReadString(int& offset, const int max_chars = 255);
void eeprom_Erase(const int offset, const int size);
void parseURL(String url, String &server, int &port, String& path, String& protocol);
String urlChangePath(String url, const String& new_path);
String shortenText(const String& text, const int lead);
String uint64ToString(uint64_t input); 
float getMemoryFragmentation();
String getDeviceInfo(const String& sep=" ");
}
