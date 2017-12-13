#include "config_common.hpp"
#include "display_neopixel.hpp"

namespace Display {
void Neopixel::displayText(const String& value, const Coords& coords)
{
  for (int i=0;i<m_num_leds;++i) {
    // m_leds[i].green = i*4;
    // m_leds[i].red = 256-(i*4);
    m_leds[i] = CRGB::Green;
  }
  FastLED.show();
  DEBUG_SERIAL.println("SHOWING");
}

void Neopixel::setBrightness(const uint8_t brightness)
{
  FastLED.setBrightness(brightness);
}

// void LixieNumeric::setContrast(uint8_t contrast)
// {
//   m_display->brightness(contrast);
// }
}
