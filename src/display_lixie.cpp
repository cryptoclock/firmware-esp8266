#include "config_common.hpp"
#include "display_lixie.hpp"

namespace Display {
void LixieNumeric::displayNumber(int number, int length, int position, bool zero_fill)
{
  m_display->write(number);
}

void LixieNumeric::setBrightness(uint8_t brightness)
{
  m_display->brightness(brightness);
}
}
