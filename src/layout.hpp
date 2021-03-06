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
    Screen layouts
*/
#pragma once
#include <Arduino.h>
#include "ArduinoJson.h"

struct LayoutScreen
{
    void fromJSON(JsonObjectConst &doc);
    String m_id;
    String m_displayName;
    String m_source;
};

class Layout
{
public:
    int findScreenIndexByUUID(const String& uuid); 
    const LayoutScreen& findScreenByIndex(int index);
    void fromJSON(JsonArrayConst &data);
private:
    static const int c_max_screens = 8;
    LayoutScreen m_screens[c_max_screens];
};
