#include <Arduino.h>
#include "display.hpp"

inline Display::~Display() {}

void Display::refreshPrice(int currentPrice)
{
  DEBUG_SERIAL.printf("[DISPLAY] REFRESHED\n");
  if (Display::m_enabled) {
    if (currentPrice == -1 || m_last_price == currentPrice) {
      DEBUG_SERIAL.printf("[DISPLAY] REPRINTING PRICE\n");
      displayPrice(String(m_last_price));
    } else if (m_last_price == -1) {
      DEBUG_SERIAL.printf("[DISPLAY] INIT PRICE\n");
      displayPrice(String(currentPrice));
    } else if (m_last_price < currentPrice) {
      DEBUG_SERIAL.printf("[DISPLAY] PRICE UP\n");
      for (int i=m_last_price; i <= currentPrice; i++) {
        displayPrice(String(i));
        delay(100);
      }
    } else {
      DEBUG_SERIAL.printf("[DISPLAY] PRICE DOWN\n");
      for (int i=m_last_price; i >= currentPrice; i--) {
        displayPrice(String(i));
        delay(100);
      }
    }
    //m_display->drawUTF8(0, 16, "B"); //""₿");
  }
  if (currentPrice != -1)
    m_last_price = currentPrice;
}

// ----

void DisplayU8G2::displayText(const String& value, int x, int y)
{
  DEBUG_SERIAL.printf("[DISPLAY] PRINTING %s to %i, %i\n", value.c_str(), x, y);
  m_display->clearBuffer();
  m_display->drawStr(x,y,value.c_str());
  //m_display->drawStr(1,position+8,value.c_str());
  m_display->sendBuffer();
}

void DisplayU8G2::displayPrice(const String& value)
{
  // TODO: fonty do tridy
  m_display->setFont(u8g2_font_artosserif8_8n);
  displayText(value);
}

void DisplayU8G2::displayTime(const String& value)
{
  // TODO: upravit na pouzivani displayText
  m_display->setFont(u8g2_font_profont10_tn);
  m_display->clearBuffer();
  m_display->drawStr(4,15,value.substring(0,5).c_str());
  m_display->sendBuffer();
}

void DisplayU8G2::displayRotate(const String& text, int pix_length)
{
  // TODO upravit na async
  for (int r=1; r <= pix_length; r++){
    displayText(text, 32-r);
    delay(40);
  }
}

void DisplayU8G2::blinkDot()
{
  // TODO upravit na async
  m_display->setDrawColor(1);
  m_display->drawPixel(31, 15);
  m_display->sendBuffer();
  delay(500);
  m_display->setDrawColor(0);
  m_display->drawPixel(31, 15);
  m_display->sendBuffer();
  m_display->setDrawColor(1);
}

void DisplayU8G2::setContrast(uint8_t contrast)
{
  Display::m_contrast = contrast;
  m_display->setContrast(Display::m_contrast);
}



// ----


void DisplayTM1637::displayPrice(const String& value)
{
  m_display->showNumberDec(value.toInt(), false, 4, 0);
}

void DisplayTM1637::displayTime(const String& datetime)
{
  int time = (datetime.substring(0,2).toInt() * 100) + datetime.substring(3,5).toInt();
  if (time==0) return;
  m_display->showNumberDecEx(time, 0xFF, true, 4, 0);
}

void DisplayTM1637::setContrast(uint8_t contrast)
{
  Display::m_contrast = contrast;
  m_display->setBrightness(min(Display::m_contrast,0x7));
}
