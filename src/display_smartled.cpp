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
#include "display_smartled.hpp"
#include "LED.hpp"


namespace Display {
SmartLED_Numeric::SmartLED_Numeric(const int milis_per_tick, const int width, const int nLEDs, LED *layout, bool is_lixie) :
    DisplayT(milis_per_tick), m_width(width), m_nLEDs(nLEDs), m_layout(layout),
    m_LEDs(new CRGB[nLEDs]),m_segments(new uint8_t[width]), m_is_lixie(is_lixie)
{
}

SmartLED_Numeric::~SmartLED_Numeric()
{
  delete[] m_LEDs;
  delete[] m_segments;
}

void SmartLED_Numeric::displayNumber(const int number, const int length, const int position, const bool zero_fill)
{
  String number_s(number);
  while ((int)number_s.length()<m_width)
    number_s = String(" ") + number_s;

  for (int i=0;i<m_width;++i)
    if (m_is_lixie)
      m_segments[i] = LED::charToLixie(number_s[i]);
    else
      m_segments[i] = LED::charTo7seg(number_s[i]);

  for (int i=0;i<m_nLEDs;++i)
  {
    auto& led = m_layout[i];
    if (led.digit>=m_width) {
      m_LEDs[i] = CRGB::Black;
      continue;
    }

    bool match = m_is_lixie ? (m_segments[led.digit] == led.segment) : (m_segments[led.digit] & led.segment);
    if (match)
      m_LEDs[i] = CRGB::DarkGreen;
    else 
      m_LEDs[i] = CRGB::Black;
  }

  FastLED.show();
}

void SmartLED_Numeric::setBrightness(const uint8_t brightness)
{
  FastLED.setBrightness(brightness);
}
}

