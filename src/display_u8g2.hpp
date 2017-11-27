#include "config_common.hpp"
#include <U8g2lib.h>
#include "display.hpp"

class DisplayU8G2 : public Display
{
public:
    DisplayU8G2(U8G2 *display) :
      Display(true), m_display(display)
      {
        m_display->begin();
        m_display->setFont(u8g2_font_micro_tr);
      }

  // for U8G2_R0 y=8
  // for U8G2_R2 y=16
  void displayText(const String& value, int x = 0, int y = 16);
//  void displayTime(const String& value);
  void setContrast(uint8_t contrast);
//  void displayRotate(const String& text, int pix_length);
//  void blinkDot();

private:
//  void displayPrice(const String& value);

  U8G2* m_display;
};
