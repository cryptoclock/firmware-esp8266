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

#include <Arduino.h>
#include "display_tm1637.hpp"

namespace Display {
void TM1637::setBrightness(const uint8_t brightness)
{
  m_display->setBrightness(brightness / 32);
}

void TM1637::displayNumber(const int number, const int length, const int position, const bool zero_fill)
{
  m_display->showNumberDecEx(number, 0 , zero_fill, length == 0 ? 4 : length, position);
}
}
