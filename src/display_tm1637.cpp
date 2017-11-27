#include <Arduino.h>
#include "display_tm1637.hpp"

//void DisplayTM1637::displayPrice(const String& value)
//{
//  m_display->showNumberDec(value.toInt(), false, 4, 0);
//}

//void DisplayTM1637::displayTime(const String& datetime)
//{
//  int time = (datetime.substring(0,2).toInt() * 100) + datetime.substring(3,5).toInt();
//  if (time==0) return;
//  m_display->showNumberDecEx(time, 0xFF, true, 4, 0);
//}

void DisplayTM1637::setContrast(uint8_t contrast)
{
  Display::m_contrast = contrast;
  m_display->setBrightness(std::min<int>(Display::m_contrast,0x7));
}
