#include "config_common.hpp"
#include <U8g2lib.h>
#include "display.hpp"

namespace Display {
  class U8G2Matrix : public DisplayT
  {
  public:
      U8G2Matrix(U8G2 *display, const u8g2_cb_t* rotation, const int width, const int height, font_t default_font) :
        DisplayT(true), m_display(display), m_rotation(rotation), m_width(width), m_height(height), m_default_font(default_font)
        {
          m_display->begin();
          m_display->setFont(u8g2_font_micro_tr);
          m_display->setDisplayRotation(m_rotation);
        }

    void displayText(const String& value, int x = 0, int y = 16, bool immediate=true);
    void drawGlyph(uint16_t glyph, Coords coords);
    void clearBuffer(void);
    void sendBuffer(void);
    int getTextWidth(const String& text);
    int getDisplayWidth(void);
    int getDisplayHeight(void);
    int getCurrentFontHeight(void);
    void setContrast(uint8_t contrast);
    void setFont(const uint8_t* font);
    font_t getDefaultFont();

  private:
    U8G2* m_display;
    const u8g2_cb_t* m_rotation;
    const int m_width;
    const int m_height;
    font_t m_default_font;
  };
}
