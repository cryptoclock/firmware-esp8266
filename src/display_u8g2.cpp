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
#include "display_u8g2.hpp"

namespace Display {
void U8G2Matrix::displayText(const String& value, const Coords& coords)
{
  useFont();
  m_display->drawStr(coords.x,coords.y + m_height,value.c_str());
}

void U8G2Matrix::displayNumber(const int number, const int length, const int position, const bool zero_fill)
{
  clearBuffer();
  displayText(String(number), {0,0});
  sendBuffer();
}

void U8G2Matrix::displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h)
{
  m_display->drawXBM(coords.x,coords.y,w,h, bitmap);
}

void U8G2Matrix::displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h)
{
  m_display->drawXBMP(coords.x,coords.y,w,h, bitmap);
}

void U8G2Matrix::drawGlyph(const uint16_t glyph, const Coords& coords)
{
  useFont();
  m_display->drawGlyph(coords.x, coords.y + m_height, glyph);
}

void U8G2Matrix::fill(const Coords& coords, const int color)
{
  setDrawColor(color);
  m_display->drawBox(coords.x, coords.y, m_width, m_height);
  setDrawColor(1);
}

void U8G2Matrix::drawLine(const Coords& start, const Coords& end)
{
  m_display->drawLine(start.x, start.y, end.x, end.y);
}

void U8G2Matrix::drawPixel(const Coords& coords)
{
  m_display->drawPixel(coords.x, coords.y);
}

void U8G2Matrix::clearBuffer(void)
{
  m_display->clearBuffer();
}

void U8G2Matrix::sendBuffer(void)
{
  m_display->sendBuffer();
}

int U8G2Matrix::getTextWidth(const String& text)
{
  return m_display->getStrWidth(text.c_str());
}

void U8G2Matrix::setBrightness(const uint8_t brightness)
{
  m_display->setContrast(brightness);
}

void U8G2Matrix::setDrawColor(const uint8_t color)
{
  m_display->setDrawColor(color);
}

void U8G2Matrix::setRotation(const bool rotation)
{
  m_rotation = rotation;

  if (m_rotation)
    m_display->setDisplayRotation(U8G2_R2);
  else
    m_display->setDisplayRotation(U8G2_R0);
}


int U8G2Matrix::getDisplayWidth() { return m_width; }
int U8G2Matrix::getDisplayHeight() { return m_height; }
int U8G2Matrix::getCurrentFontHeight()
{
  useFont();
  return m_display->getAscent();
}

void U8G2Matrix::useFont()
{
  if (m_current_font>=m_fonts.size())
    m_current_font = 0;

  m_display->setFont(m_fonts[m_current_font]);
}

}
