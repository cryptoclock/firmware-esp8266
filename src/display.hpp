#pragma once
#include "config_common.hpp"

#include <U8g2lib.h>
#include <TM1637Display.h>


class Display
{
public:
    Display(U8G2 &display, TM1637Display &display_7seg) :
      m_display(display), m_display7segment(display_7seg), m_enabled(true)
      {
        m_display.begin();
        m_display.setFont(u8g2_font_micro_tr);
      }

  // for U8G2_R0 y=8
  // for U8G2_R2 y=16
  void displayText(String value, int x = 0, int y = 8);
  void displayTime(String value);
  void refreshPrice(int lastPrice, int currentPrice);
  void setContrast(uint8_t contrast);
  void displayRotate(String text, int pix_length);
  void blinkDot();

private:
  void displayPrice(String value);
  void clear();

  U8G2 &m_display;
  TM1637Display &m_display7segment;
  bool m_enabled;
  uint8_t m_contrast;
};
