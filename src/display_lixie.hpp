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
  Lixie* m_display;
  const int m_num_digits;
};
}
