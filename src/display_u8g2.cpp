#include <Arduino.h>
#include "display_u8g2.hpp"

namespace Display {
Coords U8G2Matrix::correctOffsetForRotation(const Coords& coords)
{
  Coords updated_coords = coords;
  if (m_rotation==U8G2_R0)
    updated_coords.y += 8;
  else if (m_rotation==U8G2_R2)
    updated_coords.y += 16;
  return updated_coords;
}

void U8G2Matrix::displayText(const String& value, const Coords& coords)
{
  Coords u_coords = correctOffsetForRotation(coords);
  m_display->drawStr(u_coords.x,u_coords.y,value.c_str());
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
  Coords u_coords = correctOffsetForRotation(coords);
  m_display->drawGlyph(u_coords.x, u_coords.y, glyph);
}

void U8G2Matrix::fill(const Coords& coords)
{
  m_display->drawBox(coords.x, coords.y, m_width, m_height);
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

void U8G2Matrix::setFont(font_t font)
{
  if (font==nullptr)
    m_display->setFont(m_default_font);
  else
    m_display->setFont(font);
}

font_t U8G2Matrix::getDefaultFont(void)
{
  return m_default_font;
}

void U8G2Matrix::setBrightness(const uint8_t brightness)
{
//  m_display->setContrast(brightness * (255.0/100.0));
  m_display->setContrast(brightness);
}

void U8G2Matrix::setDrawColor(const uint8_t color)
{
  m_display->setDrawColor(color);
}


int U8G2Matrix::getDisplayWidth() { return m_width; }
int U8G2Matrix::getDisplayHeight() { return m_height; }
int U8G2Matrix::getCurrentFontHeight()
{
  return m_display->getAscent();
}
}
