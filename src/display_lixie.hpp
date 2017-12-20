#include "config_common.hpp"
#define FASTLED_INTERNAL
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include <Lixie.h>
#include "display.hpp"

namespace Display {
class LixieNumeric : public DisplayT
{
public:
  LixieNumeric(Lixie* display, const int num_digits) :
    DisplayT(true), m_display(display), m_num_digits(num_digits)
  {
    m_display->begin();
    m_display->max_power(5,450); // 5V, 450mA
  }

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
  font_t getDefaultFont() { return nullptr; }
  void setBrightness(const uint8_t brightness);
  void setDrawColor(const uint8_t color) {};
  void setRotation(const bool rotation) {};
  int getDisplayWidth() { return m_num_digits; }
  int getDisplayHeight() { return 1; }
  int getCurrentFontHeight() { return 1; }
  bool isNumeric(void) { return true; }
  bool isGraphic(void) { return false; }

private:
  Lixie* m_display;
  const int m_num_digits;
};
}
