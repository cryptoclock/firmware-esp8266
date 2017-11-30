#include "config_common.hpp"
#include <TM1637Display.h>
#include "display.hpp"

class DisplayTM1637 : public Display
{
public:
    DisplayTM1637(TM1637Display* display, const int num_digits) :
      Display(true), m_display(display), m_num_digits(num_digits)
      {}

  void displayText(const String& value, int x = 0, int y = 16, bool immediate=true) {}
  void clearBuffer() {}
  void sendBuffer() {}
  int getTextWidth(const String& text) { return 0; }
  void setFont(const uint8_t* font) {};
  void setContrast(uint8_t contrast);
  int getDisplayWidth() { return m_num_digits; }
  int getDisplayHeight() { return 1; }
  int getCurrentFontHeight() { return 1; }

private:
  TM1637Display* m_display;
  const int m_num_digits;
};
