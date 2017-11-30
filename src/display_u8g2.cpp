#include <Arduino.h>
#include "display_u8g2.hpp"

void DisplayU8G2::displayText(const String& value, int x, int y, bool immediate)
{
  if (m_rotation==U8G2_R0)
    y += 8;
  else if (m_rotation==U8G2_R2)
    y += 16;

//  DEBUG_SERIAL.printf("[DISPLAY] PRINTING %s to %i, %i\n", value.c_str(), x, y);
  if (immediate) clearBuffer();
  m_display->drawStr(x,y,value.c_str());
  if (immediate) sendBuffer();
}

void DisplayU8G2::clearBuffer(void)
{
  m_display->clearBuffer();
}

void DisplayU8G2::sendBuffer(void)
{
  m_display->sendBuffer();
}

int DisplayU8G2::getTextWidth(const String& text)
{
  return m_display->getStrWidth(text.c_str());
}

void DisplayU8G2::setFont(const uint8_t* font)
{
  m_display->setFont(font);
}

void DisplayU8G2::setContrast(uint8_t contrast)
{
  Display::m_contrast = contrast;
  m_display->setContrast(Display::m_contrast);
}

int DisplayU8G2::getDisplayWidth() { return 32; } // FIXME: move to member variable
int DisplayU8G2::getDisplayHeight() { return 8; }
int DisplayU8G2::getCurrentFontHeight()
{
  return m_display->getAscent();
}
