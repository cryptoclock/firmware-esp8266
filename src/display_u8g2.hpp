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
    m_display->setFontMode(0);
    m_display->setDisplayRotation(m_rotation);
    m_display->setContrast(0);
  }

  void displayNumber(int number, int length, int position, bool zero_fill);
  void displayText(const String& value, const Coords& coords);
  void displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h);
  void displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h);

  void fill(const Coords& coords);
  void drawLine(const Coords& start, const Coords& end);
  void drawGlyph(const uint16_t glyph, const Coords& coords);
  void drawPixel(const Coords& coords);

  void clearBuffer(void);
  void sendBuffer(void);
  int getTextWidth(const String& text);
  int getDisplayWidth(void);
  int getDisplayHeight(void);
  int getCurrentFontHeight(void);
  void setBrightness(const uint8_t brightness);
  void setFont(font_t font);
  void setDrawColor(const uint8_t color);
  void setRotation(const bool rotation);
  font_t getDefaultFont();
  bool isNumeric(void) { return false; }
  bool isGraphic(void) { return true; }
private:
  Coords correctOffsetForRotation(const Coords& coords);

  U8G2* m_display;
  const u8g2_cb_t* m_rotation;
  const int m_width;
  const int m_height;
  font_t m_default_font;
};
}
