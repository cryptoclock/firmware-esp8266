#include "config_common.hpp"
#include "display_lixie.hpp"

namespace Display {
  void LixieNumeric::displayNumber(int number, int length, int position, bool zero_fill)
  {
    m_display->write(number);
  }

  void LixieNumeric::setContrast(uint8_t contrast)
  {
    m_display->brightness(contrast);
  }
}
