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
  Class for serial communication
*/

#pragma once

#include "protocol.hpp"

class SerialComm
{
public:
  SerialComm(CC_Protocol *prot): m_prot(prot), m_buffer("")
  {}
 
  void setupTickCallback(Ticker::callback_function_t callback);
  void detachTicker();

  void loop();
private:
  Ticker m_ticker; // for communicating via serial during setup()
  CC_Protocol *m_prot;
  String m_buffer;
  const int c_milis_per_tick = 333;
};
