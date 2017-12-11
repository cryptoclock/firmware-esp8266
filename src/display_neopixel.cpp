#include "config_common.hpp"
#include "display_neopixel.hpp"

namespace Display {
void Neopixel::displayText(const String& value, int x, int y, bool immediate)
{
  for (int i=0;i<m_num_leds;++i) {
    // m_leds[i].green = i*4;
    // m_leds[i].red = 256-(i*4);
    m_leds[i] = CRGB::Green;
  }
  FastLED.show();
  DEBUG_SERIAL.println("SHOWING");
}

void Neopixel::setBrightness(uint8_t brightness)
{
  FastLED.setBrightness(brightness * (255.0 / 100.0));
}

// void LixieNumeric::setContrast(uint8_t contrast)
// {
//   m_display->brightness(contrast);
// }
}
