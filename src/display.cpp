#include "display.h"

void Display::displayText(String value, int x, int y) {
  m_debug.printf("[DISPLAY] PRINTING %s to %i, %i\n", value.c_str(), x, y);
  m_display.clearBuffer();
  m_display.drawStr(x,y,value.c_str());
  //m_display.drawStr(1,position+8,value.c_str());
  m_display.sendBuffer();
}

void Display::displayPrice(String value) {
  displayText(value,4);
}

void Display::displayRotate(String text, int pix_length) {
  for (int r=1; r <= pix_length; r++){
    displayText(text, 32-r);
    delay(40);
  }
}

void Display::refreshPrice(int lastPrice, int currentPrice) {
  m_debug.printf("[DISPLAY] REFRESHED\n");
  if (m_enabled) {
    if (lastPrice == -1) {
      m_debug.printf("[DISPLAY] INIT PRICE\n");
      displayPrice(String(currentPrice));
    } else if (lastPrice < currentPrice) {
      m_debug.printf("[DISPLAY] PRICE UP\n");
      for (int i=lastPrice; i <= currentPrice; i++) {
        displayPrice(String(i));
        delay(100);
      }
    } else {
      m_debug.printf("[DISPLAY] PRICE DOWN\n");
      for (int i=lastPrice; i >= currentPrice; i--) {
        displayPrice(String(i));
        delay(100);
      }
    }
    //m_display.drawUTF8(0, 16, "B"); //""₿");
  }
}

void Display::clear() {
  m_debug.printf("[DISPLAY] CLEARED\n");
  m_display.clearBuffer();
  m_display.sendBuffer();
}

void Display::setContrast(uint8_t contrast)
{
  m_contrast = contrast;
  m_display.setContrast(m_contrast);
}


void Display::flash() {
  m_debug.printf("[DISPLAY] FLASHING\n");
  m_display.setContrast(100);
  delay(100);
  m_display.setContrast(255);
  delay(100);
  m_display.setContrast(100);
  delay(100);
  m_display.setContrast(m_contrast);
}
