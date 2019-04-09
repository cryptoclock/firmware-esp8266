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

#include "layout.hpp"

void LayoutScreen::fromJSON(JsonObjectConst &doc)
{
    m_id = doc["id"].as<String>();
    m_displayName = doc["displayName"].as<String>(); 
    m_source = doc["source"].as<String>();
    // TODO: format
}

int Layout::findScreenIndexByUUID(const String& uuid)
{
    for (int i=0;i<c_max_screens;++i)
        if (m_screens[i].m_id == uuid)
            return i;

    return 0;
}

const LayoutScreen& Layout::findScreenByIndex(int index)
{
    if (index<0 || index>c_max_screens)
        index = 0;

    return m_screens[0];
}

void Layout::fromJSON(JsonArrayConst &data)
{
    for (JsonObjectConst screen: data) {
        int index = screen["index"];
        if (index<0 || index >=c_max_screens)
            continue;
        m_screens[index].fromJSON(screen);    
    }
}

