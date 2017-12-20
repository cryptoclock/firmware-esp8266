#include "config_common.hpp"
#include <TM1637Display.h>
#include "display.hpp"

namespace Display {
class TM1637 : public DisplayT
{
public:
  TM1637(TM1637Display* display, const int num_digits) :
    DisplayT(true), m_display(display), m_num_digits(num_digits)
    {}

  void displayText(const String& value, const Coords& coords) {}
  void displayNumber(const int number, const int length, const int position, const bool zero_fill);
  void displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h) {}
  void displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h) {}

  void fill(const Coords& coords) {}

  void drawGlyph(const uint16_t glyph, const Coords& coords) {}
  void drawLine(const Coords& start, const Coords& end) {}
  void drawPixel(const Coords& coords) {}


  void clearBuffer() {}
  void sendBuffer() {}
  int getTextWidth(const String& text) { return 0; }
  void setFont(font_t font) {};
  void setDrawColor(const uint8_t color) {};
  void setRotation(const bool rotation) {};
  font_t getDefaultFont() { return nullptr; }
  void setBrightness(const uint8_t brightness);
  int getDisplayWidth() { return m_num_digits; }
  int getDisplayHeight() { return 1; }
  int getCurrentFontHeight() { return 1; }
  bool isNumeric(void) { return true; }
  bool isGraphic(void) { return false; }

private:
  TM1637Display* m_display;
  const int m_num_digits;
};
}
