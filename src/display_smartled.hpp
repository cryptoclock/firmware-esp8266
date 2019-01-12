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
  SmartLED display driver (WS281x, Neopixel etc.)
*/

#pragma once
#include "config_common.hpp"
#define FASTLED_INTERNAL
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define FASTLED_ALLOW_INTERRUPTS 0
#include "FastLED.h"
#include "display.hpp"
#include "LED.hpp"

namespace Display {
class SmartLED_Numeric : public DisplayT
{
public:
  SmartLED_Numeric(const int milis_per_tick, const int width, const int nLEDs, LED *layout, bool is_lixie=false);
  SmartLED_Numeric(const SmartLED_Numeric&) = delete;
  SmartLED_Numeric& operator=(const SmartLED_Numeric&) = delete;
  ~SmartLED_Numeric();

  CRGB* getLEDdata() { return m_LEDs;}

  void displayNumber(const int number, const int length, const int position, const bool zero_fill);

  void setBrightness(const uint8_t brightness);

  int getDisplayWidth() { return m_width; }
  int getDisplayHeight() { return 1; }
  bool isGraphic(void) { return false; }
protected:
  const int m_width;
  const int m_nLEDs;
  LED *m_layout;
  CRGB *m_LEDs;
  uint8_t *m_segments;
  bool m_is_lixie; // true if in Lixie configuration instead of 7-segment
};
}
