#include <Arduino.h>
#include "display_tm1637.hpp"

namespace Display {
void TM1637::setBrightness(uint8_t brightness)
{
//  DisplayT::m_contrast = contrast;
  m_display->setBrightness(brightness * ( 7.0 / 100.0));
}

void TM1637::displayNumber(int number, int length, int position, bool zero_fill)
{
  m_display->showNumberDecEx(number, 0 , zero_fill, length == 0 ? 4 : length, position);
}
}
