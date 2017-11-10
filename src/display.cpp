#include "display.hpp"

void Display::displayText(String value, int x, int y) {
  DEBUG_SERIAL.printf("[DISPLAY] PRINTING %s to %i, %i\n", value.c_str(), x, y);
  m_display.clearBuffer();
  m_display.drawStr(x,y,value.c_str());
  //m_display.drawStr(1,position+8,value.c_str());
  m_display.sendBuffer();
}

void Display::displayPrice(String value) {
  m_display7segment.showNumberDec(value.toInt(), false, 4, 0);
  displayText(value,4);
}

void Display::displayRotate(String text, int pix_length) {
  for (int r=1; r <= pix_length; r++){
    displayText(text, 32-r);
    delay(40);
  }
}

void Display::refreshPrice(int lastPrice, int currentPrice) {
  DEBUG_SERIAL.printf("[DISPLAY] REFRESHED\n");
  if (m_enabled) {
    if (lastPrice == -1) {
      DEBUG_SERIAL.printf("[DISPLAY] INIT PRICE\n");
      displayPrice(String(currentPrice));
    } else if (lastPrice < currentPrice) {
      DEBUG_SERIAL.printf("[DISPLAY] PRICE UP\n");
      for (int i=lastPrice; i <= currentPrice; i++) {
        displayPrice(String(i));
        delay(100);
      }
    } else {
      DEBUG_SERIAL.printf("[DISPLAY] PRICE DOWN\n");
      for (int i=lastPrice; i >= currentPrice; i--) {
        displayPrice(String(i));
        delay(100);
      }
    }
    //m_display.drawUTF8(0, 16, "B"); //""₿");
  }
}

void Display::clear() {
  DEBUG_SERIAL.printf("[DISPLAY] CLEARED\n");
  m_display.clearBuffer();
  m_display.sendBuffer();
}

void Display::setContrast(uint8_t contrast)
{
  m_contrast = contrast;
  m_display.setContrast(m_contrast);
  m_display7segment.setBrightness(min(m_contrast,0x7));
}
