#pragma once

#include <U8g2lib.h>
#include <TM1637Display.h>

#include "config_common.hpp"

class Display
{
public:
  Display(bool enabled = true) :
    m_enabled(enabled), m_contrast(0)
  {}
  virtual ~Display() = 0;

  void refreshPrice(int lastPrice, int currentPrice);

  virtual void displayText(const String& value, int x = 0, int y = 16) {}
  virtual void displayTime(const String& value) {}
  virtual void setContrast(uint8_t contrast) {}
  virtual void displayRotate(const String& text, int pix_length) {}
  virtual void blinkDot() {}

protected:
  virtual void displayPrice(const String &value) {}

  bool m_enabled;
  uint8_t m_contrast;
};

class DisplayU8G2 : public Display
{
public:
    DisplayU8G2(U8G2& display) :
      Display(true), m_display(display)
      {
        m_display.begin();
        m_display.setFont(u8g2_font_micro_tr);
      }

  // for U8G2_R0 y=8
  // for U8G2_R2 y=16
  void displayText(const String& value, int x = 0, int y = 16);
  void displayTime(const String& value);
  void setContrast(uint8_t contrast);
  void displayRotate(const String& text, int pix_length);
  void blinkDot();

private:
  void displayPrice(const String& value);

  U8G2& m_display;
};

class DisplayTM1637 : public Display
{
public:
    DisplayTM1637(TM1637Display& display) :
      Display(true), m_display(display)
      {}

  void displayText(const String& value, int x = 0, int y = 16) {}
  void displayTime(const String& value);
  void setContrast(uint8_t contrast);
  void displayRotate(const String& text, int pix_length) {}
  void blinkDot() {}

private:
  void displayPrice(const String& value);

  TM1637Display& m_display;
};
