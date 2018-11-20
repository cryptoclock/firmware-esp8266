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
  Class to handle basic WiFi functions and interaction with WiFiManager
*/

#pragma once
#include "config_common.hpp"
#include <WiFiManager.h>
#include <vector>
#include "display.hpp"
#include "aplist.hpp"

class WiFiCore
{
public:
  WiFiCore(DisplayT *display);

  void setAPCallback(void (*func)(WiFiManager*));
  void startAP(const String& ssid_name, unsigned long timeout = 120);
  void connectToWiFiOrFallbackToAP(void);
  void resetSettings(void);

  WiFiManager* getWiFiManager(void) { return &m_wifimanager; }

  static void saveCallback(void);
  static void onConnect(WiFiEventStationModeConnected event_info);
  static void onDisconnect(WiFiEventStationModeDisconnected event_info);
  static void onGotIP(WiFiEventStationModeGotIP ipInfo);

  void updateParametersFromAP(WiFiManager *manager); // reads parameters from AP config page and updates g_parameters
private:
  void addParametersFromGlobal();

  WiFiManager m_wifimanager;
  DisplayT *m_display;
  std::vector<WiFiManagerParameter*> m_parameters;

  WiFiEventHandler m_ev_conn, m_ev_disconn, m_ev_gotip;
};
