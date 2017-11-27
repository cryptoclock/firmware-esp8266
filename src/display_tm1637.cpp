#include <Arduino.h>
#include "display_tm1637.hpp"

void DisplayTM1637::setContrast(uint8_t contrast)
{
  Display::m_contrast = contrast;
  m_display->setBrightness(std::min<int>(Display::m_contrast,0x7));
}
