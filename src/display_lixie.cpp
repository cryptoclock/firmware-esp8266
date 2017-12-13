#include "config_common.hpp"
#include "display_lixie.hpp"

namespace Display {
void LixieNumeric::displayNumber(const int number, const int length, const int position, const bool zero_fill)
{
  m_display->write(number);
}

void LixieNumeric::setBrightness(const uint8_t brightness)
{
  m_display->brightness(brightness);
}
}
