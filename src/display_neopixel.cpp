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

#include "config_common.hpp"
#include "display_neopixel.hpp"

namespace Display {
void Neopixel::displayText(const String& value, const Coords& coords)
{
  for (int i=0;i<m_width*m_height;++i) {
    // m_leds[i].green = i*4;
    // m_leds[i].red = 256-(i*4);
    m_leds[i] = CRGB::Green;
  }
  FastLED.show();
  DEBUG_SERIAL.println(F("SHOWING"));
}

void Neopixel::setBrightness(const uint8_t brightness)
{
  FastLED.setBrightness(brightness);
}

// void LixieNumeric::setContrast(uint8_t contrast)
// {
//   m_display->brightness(contrast);
// }
}
