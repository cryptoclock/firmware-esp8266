#include <Arduino.h>
#include "display_tm1637.hpp"

namespace Display {
void TM1637::setBrightness(const uint8_t brightness)
{
  m_display->setBrightness(brightness / 32);
}

void TM1637::displayNumber(const int number, const int length, const int position, const bool zero_fill)
{
  m_display->showNumberDecEx(number, 0 , zero_fill, length == 0 ? 4 : length, position);
}
}
