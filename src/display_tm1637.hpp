#include "config_common.hpp"
#include <TM1637Display.h>
#include "display.hpp"

class DisplayTM1637 : public Display
{
public:
    DisplayTM1637(TM1637Display* display) :
      Display(true), m_display(display)
      {}

  void displayText(const String& value, int x = 0, int y = 16) {}
//  void displayTime(const String& value);
  void setContrast(uint8_t contrast);
//  void displayRotate(const String& text, int pix_length) {}
//  void blinkDot() {}

private:
//  void displayPrice(const String& value);

  TM1637Display* m_display;
};
