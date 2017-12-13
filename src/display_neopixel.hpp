#include "config_common.hpp"
#define FASTLED_INTERNAL
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#include "FastLED.h"
// #define NUM_LEDS 60
// CRGB leds[NUM_LEDS];
// void setup() { FastLED.addLeds<NEOPIXEL, 6>(leds, NUM_LEDS); }
// void loop() {
// 	leds[0] = CRGB::White; FastLED.show(); delay(30);
// 	leds[0] = CRGB::Black; FastLED.show(); delay(30);
// }
#include "display.hpp"

namespace Display {
class Neopixel : public DisplayT
{
public:
  Neopixel(const int num_leds) :
    DisplayT(true), m_num_leds(num_leds),
    m_leds(new CRGB[num_leds])
  {
//    m_display->begin();
//    m_display->max_power(5,450); // 5V, 450mA
  }
  template<uint8_t DataPin>
  void initialize()
  {
    FastLED.addLeds<NEOPIXEL, DataPin>(m_leds, m_num_leds);
  }

  void displayText(const String& value, const Coords& coords);
  void displayNumber(const int number, const int length, const int position, const bool zero_fill) {}
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
  int getDisplayWidth() { return m_num_leds; }
  int getDisplayHeight() { return 1; }
  int getCurrentFontHeight() { return 1; }
  bool isNumeric(void) { return false; }
  bool isGraphic(void) { return true; }

private:
  const int m_num_leds;
  CRGB *m_leds;
};
}
