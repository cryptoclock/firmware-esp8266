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
  Neopixel display driver (WIP)
*/

#pragma once
#include "config_common.hpp"
#define FASTLED_INTERNAL
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#include "display.hpp"

namespace Display {
class Neopixel : public DisplayT
{
public:
  Neopixel(const int milis_per_tick, const int width, const int height);

  template<uint8_t DataPin>
  void initialize()
  {
    FastLED.addLeds<NEOPIXEL, DataPin>(m_leds, m_width*m_height);
  }

  void displayNumber(const int number, const int length, const int position, const bool zero_fill);

  void setBrightness(const uint8_t brightness);

  int getDisplayWidth() { return m_width; }
  int getDisplayHeight() { return m_height; }
  bool isGraphic(void) { return false; }
private:
  const int m_width;
  const int m_height;
  CRGB *m_leds;
};
}
