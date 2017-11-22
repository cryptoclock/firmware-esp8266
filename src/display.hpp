#pragma once

#include "config_common.hpp"

class Display
{
public:
  Display(bool enabled = true) :
    m_enabled(enabled), m_contrast(0), m_last_price(-1)
  {}
  virtual ~Display() = 0;

  void refreshPrice(int currentPrice);

  virtual void displayText(const String& value, int x = 0, int y = 16) {}
  virtual void displayTime(const String& value) {}
  virtual void setContrast(uint8_t contrast) {}
  virtual void displayRotate(const String& text, int pix_length) {}
  virtual void blinkDot() {}

protected:
  virtual void displayPrice(const String &value) {}

  bool m_enabled;
  uint8_t m_contrast;
  int m_last_price;
};
