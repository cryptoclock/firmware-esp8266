#include <Arduino.h>
#include "display_tm1637.hpp"

namespace Display {
  void TM1637::setContrast(uint8_t contrast)
  {
    DisplayT::m_contrast = contrast;
    m_display->setBrightness(std::min<int>(DisplayT::m_contrast,0x7));
  }
}
