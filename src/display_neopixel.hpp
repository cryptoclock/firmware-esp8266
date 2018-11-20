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
#include "FastLED.h"
// #define NUM_LEDS 60
// CRGB leds[NUM_LEDS];
// void setup() { FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS); }
// void loop() {
// 	leds[0] = CRGB::White; FastLED.show(); delay(30);
// 	leds[0] = CRGB::Black; FastLED.show(); delay(30);
// }
#include "display.hpp"

namespace Display {
class Neopixel : public DisplayT
{
public:
  Neopixel(const int milis_per_tick, const int width, const int height) :
    DisplayT(milis_per_tick), m_width(width), m_height(height),
    m_leds(new CRGB[width*height])
  {
//    m_display->begin();
//    m_display->max_power(5,450); // 5V, 450mA
  }
  template<uint8_t DataPin>
  void initialize()
  {
    FastLED.addLeds<NEOPIXEL, DataPin>(m_leds, m_width*m_height);
  }

  void displayText(const String& value, const Coords& coords);
  void displayNumber(const int number, const int length, const int position, const bool zero_fill) {}
  void displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h) {}
  void displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h) {}

  void fill(const Coords& coords, const int color) {}

  void drawGlyph(const uint16_t glyph, const Coords& coords) {}
  void drawLine(const Coords& start, const Coords& end) {}
  void drawPixel(const Coords& coords) {}

  void clearBuffer() {}
  void sendBuffer() {}
  int getTextWidth(const String& text) { return 0; }
  void setBrightness(const uint8_t brightness);
  void setDrawColor(const uint8_t color) {};
  void setRotation(const bool rotation) {};
  int getDisplayWidth() { return m_width; }
  int getDisplayHeight() { return m_height; }
  int getCurrentFontHeight() { return 1; }
  bool isNumeric(void) { return false; }
  bool isGraphic(void) { return true; }

private:
  const int m_width;
  const int m_height;
  CRGB *m_leds;
};
}
