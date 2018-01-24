#include <Arduino.h>
#include "display_u8g2.hpp"

namespace Display {
Coords U8G2Matrix::correctOffsetForRotation(const Coords& coords)
{
  Coords updated_coords = coords;
  if (m_rotation)
    updated_coords.y += 8;
  return updated_coords;
}

void U8G2Matrix::displayText(const String& value, const Coords& coords)
{
  useFont();
  Coords u_coords = correctOffsetForRotation(coords);
  m_display->drawStr(u_coords.x,u_coords.y + m_height,value.c_str());
}

void U8G2Matrix::displayNumber(const int number, const int length, const int position, const bool zero_fill)
{
  clearBuffer();
  displayText(String(number), {0,0});
  sendBuffer();
}

void U8G2Matrix::displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h)
{
  auto u_coords = correctOffsetForRotation(coords);
  m_display->drawXBM(u_coords.x,u_coords.y,w,h, bitmap);
}

void U8G2Matrix::displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h)
{
  auto u_coords = correctOffsetForRotation(coords);
  m_display->drawXBMP(u_coords.x,u_coords.y,w,h, bitmap);
}

void U8G2Matrix::drawGlyph(const uint16_t glyph, const Coords& coords)
{
  useFont();
  Coords u_coords = correctOffsetForRotation(coords);
  m_display->drawGlyph(u_coords.x, u_coords.y + m_height, glyph);
}

void U8G2Matrix::fill(const Coords& coords, const int color)
{
  setDrawColor(color);
  auto u_coords = correctOffsetForRotation(coords);
  m_display->drawBox(u_coords.x, u_coords.y, m_width, m_height);
  setDrawColor(1);
}

void U8G2Matrix::drawLine(const Coords& start, const Coords& end)
{
  auto u_start = correctOffsetForRotation(start);
  auto u_end = correctOffsetForRotation(end);
  m_display->drawLine(u_start.x, u_start.y, u_end.x, u_end.y);
}

void U8G2Matrix::drawPixel(const Coords& coords)
{
  auto u_coords = correctOffsetForRotation(coords);
  m_display->drawPixel(u_coords.x, u_coords.y);
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
