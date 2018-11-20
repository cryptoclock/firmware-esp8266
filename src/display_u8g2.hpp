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
  Display driver for displays supported by U8G2 library
*/

#pragma once
#include "config_common.hpp"
#include <U8g2lib.h>
#include "display.hpp"

namespace Display {
class U8G2Matrix : public DisplayT
{
public:
  U8G2Matrix(U8G2 *display, const int milis_per_tick, const bool rotation, const int width, const int height) :
    DisplayT(milis_per_tick), m_display(display), m_rotation(rotation), m_width(width), m_height(height)
  {
    m_display->begin();
#if X_DISPLAY_HEIGHT<16
    m_fonts.push_back(u8g2_font_profont10_tr);
    m_fonts.push_back(u8g2_font_micro_tr);
    m_fonts.push_back(u8g2_font_u8glib_4_tr);
#elif X_DISPLAY_HEIGHT<32
    m_fonts.push_back(u8g2_font_profont15_mr);
    m_fonts.push_back(u8g2_font_profont12_mr);
    m_fonts.push_back(u8g2_font_profont10_mr);
#else
    m_fonts.push_back(u8g2_font_profont29_mr);
    m_fonts.push_back(u8g2_font_profont22_mr);
    m_fonts.push_back(u8g2_font_10x20_tr);
#endif

    m_display->setFont(m_fonts[0]);
    m_display->setFontMode(0);
    m_display->setContrast(0);
    setRotation(m_rotation);
  }

  void displayNumber(int number, int length, int position, bool zero_fill);
  void displayText(const String& value, const Coords& coords);
  void displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h);
  void displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h);

  void fill(const Coords& coords, const int color=1);
  void drawLine(const Coords& start, const Coords& end);
  void drawGlyph(const uint16_t glyph, const Coords& coords);
  void drawPixel(const Coords& coords);

  void clearBuffer(void);
  void sendBuffer(void);
  int getTextWidth(const String& text);
  int getDisplayWidth(void);
  int getDisplayHeight(void);
  int getCurrentFontHeight(void);
  void setBrightness(const uint8_t brightness);
  void setDrawColor(const uint8_t color);
  void setRotation(const bool rotation);
  bool isNumeric(void) { return false; }
  bool isGraphic(void) { return true; }
private:
  void useFont();
  Coords correctOffsetForRotation(const Coords& coords);

  U8G2* m_display;
  bool m_rotation;
  const int m_width;
  const int m_height;
  vector<const uint8_t *> m_fonts;
};
}
