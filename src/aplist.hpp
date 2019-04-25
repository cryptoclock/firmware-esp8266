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
  This class manages Access Points we should be connecting to (ssid + password). 
  They are passed to/read from WiFiManager and stored in Flash.
*/

#pragma once
#include "config_common.hpp"
#include <WiFiManager.h>

using std::vector;
using std::pair;

class AP_list {
public:
  AP_list(WiFiManager *manager) : m_manager(manager) {}
  AP_list() = delete;
  void addAPsToWiFiManager();
  void saveAPsToEEPROM();
  void addAP(const String& ssid, const String& password);
  void removeAP(const String& ssid);
  vector<pair<String,String>> getAPs();

  WiFiManager *m_manager;
};
