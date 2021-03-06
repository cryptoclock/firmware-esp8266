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
  Display driver for TM1637 7-segment driver
*/

#pragma once
#include "config_common.hpp"
#include <TM1637Display.h>
#include "display.hpp"

namespace Display {
class TM1637 : public DisplayT
{
public:
  TM1637(TM1637Display* display, const int milis_per_tick, const int num_digits) :
    DisplayT(milis_per_tick), m_display(display), m_num_digits(num_digits)
    {}

  void displayNumber(const int number, const int length, const int position, const bool zero_fill);

  void setBrightness(const uint8_t brightness);

  int getDisplayWidth() { return m_num_digits; }
  int getDisplayHeight() { return 1; }
  bool isGraphic(void) { return false; }
private:
  TM1637Display* m_display;
  const int m_num_digits;
};
}
