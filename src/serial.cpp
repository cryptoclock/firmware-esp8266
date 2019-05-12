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

#include "serial.hpp"
#include "log.hpp"

//static const char* LOGTAG = "Serial";

void SerialComm::loop()
{
  String line = m_prot->readyToSend();
  if (line.length()>0)
    Serial.printf_P(PSTR("|SERIALCOMM|%s|%02X|\n"),line.c_str(), Utils::calculateChecksum(line.c_str()));

  while(Serial.available()>0) {
    int ch = Serial.read();
    if (ch == -1) // no data avilable / error
      break;
    if (ch == '\r')
      continue;

    if (ch == '\n') {
//      CCLOGD("data: '%s'", m_buffer.c_str());
      m_prot->dataReceived(m_buffer.c_str(), m_buffer.length());
      m_buffer = "";
    } else {
      m_buffer += (char) ch;
    }
  }
}

void SerialComm::setupTickCallback(Ticker::callback_t callback)
{
    m_ticker.attach(c_milis_per_tick / 1000.0,callback);
}

void SerialComm::detachTicker()
{
  m_ticker.detach();
}
