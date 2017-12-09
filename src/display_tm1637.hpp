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

  void displayText(const String& value, int x = 0, int y = 16, bool immediate=true) {}
  void displayNumber(int number, int length, int position, bool zero_fill);
  void displayBitmap(const unsigned char *bitmap, const int x, const int y, const int w, const int h) {}
  void displayBitmapP(const unsigned char *bitmap, const int x, const int y, const int w, const int h) {}
  void drawGlyph(uint16_t glyph, Coords coords) {}
  void clearBuffer() {}
  void sendBuffer() {}
  int getTextWidth(const String& text) { return 0; }
  void setFont(const uint8_t* font) {};
  font_t getDefaultFont() { return nullptr; }
  void setContrast(uint8_t contrast);
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
