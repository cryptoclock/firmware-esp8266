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

// 7-segment segments
#define SEG_A 1 << 0
#define SEG_B 1 << 1
#define SEG_C 1 << 2
#define SEG_D 1 << 3
#define SEG_E 1 << 4
#define SEG_F 1 << 5
#define SEG_G 1 << 6
#define SEG_DP 1 << 7

uint8_t digitToSegments[11]
{
  /* 0 */ SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F,
  /* 1 */ SEG_B|SEG_C, 
  /* 2 */ SEG_A|SEG_B|SEG_D|SEG_E|SEG_G,
  /* 3 */ SEG_A|SEG_B|SEG_C|SEG_D|SEG_G,
  /* 4 */ SEG_B|SEG_C|SEG_F|SEG_G,
  /* 5 */ SEG_A|SEG_C|SEG_D|SEG_F|SEG_G,
  /* 6 */ SEG_A|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,
  /* 7 */ SEG_A|SEG_B|SEG_C,
  /* 8 */ SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G,
  /* 9 */ SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G,
  /* - */ SEG_G
};

struct LEDlayout
{
  LEDlayout(uint8_t digit, uint8_t segment) : digit(digit), segment(segment), index(0) {}
  LEDlayout(void) : digit(0), segment(0), index(0) {}

  uint8_t digit;
  uint8_t segment;

  // if multiple leds are used for same segment, this index is used as indicator of their relative position for color-cycling effects,
  // with zero being leftmost/topmost led of the pack
  uint8_t index; 
};

LEDlayout g_led_config[64] =
{
  {0, SEG_A}, {0, SEG_B}, {0, SEG_C}, {0, SEG_D}, {0, SEG_E}, {0, SEG_F}, {0, SEG_G}, {0, SEG_DP},
  {1, SEG_A}, {1, SEG_B}, {1, SEG_C}, {1, SEG_D}, {1, SEG_E}, {1, SEG_F}, {1, SEG_G}, {1, SEG_DP},
  {2, SEG_A}, {2, SEG_B}, {2, SEG_C}, {2, SEG_D}, {2, SEG_E}, {2, SEG_F}, {2, SEG_G}, {2, SEG_DP},
  {3, SEG_A}, {3, SEG_B}, {3, SEG_C}, {3, SEG_D}, {3, SEG_E}, {3, SEG_F}, {3, SEG_G}, {3, SEG_DP},
  {4, SEG_A}, {4, SEG_B}, {4, SEG_C}, {4, SEG_D}, {4, SEG_E}, {4, SEG_F}, {4, SEG_G}, {4, SEG_DP},
  {5, SEG_A}, {5, SEG_B}, {5, SEG_C}, {5, SEG_D}, {5, SEG_E}, {5, SEG_F}, {5, SEG_G}, {5, SEG_DP},
  {6, SEG_A}, {6, SEG_B}, {6, SEG_C}, {6, SEG_D}, {6, SEG_E}, {6, SEG_F}, {6, SEG_G}, {6, SEG_DP},
  {7, SEG_A}, {7, SEG_B}, {7, SEG_C}, {7, SEG_D}, {7, SEG_E}, {7, SEG_F}, {7, SEG_G}, {7, SEG_DP},
};

namespace Display {

Neopixel::Neopixel(const int milis_per_tick, const int width, const int height) :
    DisplayT(milis_per_tick), m_width(width), m_height(height),
    m_leds(new CRGB[width*height])
{
}

void Neopixel::displayNumber(const int number, const int length, const int position, const bool zero_fill)
{
  DEBUG_SERIAL.printf("DISPLAYING NUMBER: %i\n", number);
  // FIXME: position, add leading spaces
  String number_s(number);
  uint8_t segments[16] = {};
  for (int i=0;i<(int)number_s.length();++i)
    segments[i] = digitToSegments[number_s[i]-'0'];

  // setup leds
  for (int i=0;i<m_width*m_height;++i)
  {
    auto& led = g_led_config[i];
    if (segments[led.digit] & led.segment)
      m_leds[i] = CRGB::DarkGreen;
    else 
      m_leds[i] = CRGB::Black;
  }
  FastLED.show();
}

void Neopixel::displayText(const String& value, const Coords& coords)
{
}

void Neopixel::setBrightness(const uint8_t brightness)
{
  FastLED.setBrightness(brightness);
}

}
