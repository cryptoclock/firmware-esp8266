#include "config_common.hpp"
#include <U8g2lib.h>
#include "display.hpp"

class DisplayU8G2 : public Display
{
public:
    DisplayU8G2(U8G2 *display, const u8g2_cb_t* rotation) :
      Display(true), m_display(display), m_rotation(rotation)
      {
        m_display->begin();
        m_display->setFont(u8g2_font_micro_tr);
        m_display->setDisplayRotation(m_rotation);
      }

  void displayText(const String& value, int x = 0, int y = 16, bool immediate=true);
  void clearBuffer(void);
  void sendBuffer(void);
  int getTextWidth(const String& text);
  int getDisplayWidth();
  int getDisplayHeight();
  int getCurrentFontHeight();
  void setContrast(uint8_t contrast);
  void setFont(const uint8_t* font);

private:
  U8G2* m_display;
  const u8g2_cb_t* m_rotation;
};
