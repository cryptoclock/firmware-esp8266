#include <U8g2lib.h>

class Display
{
public:
    Display(U8G2 &display, HardwareSerial &debug = Serial) :
      m_debug(debug), m_display(display), m_enabled(true)
      {
        m_display.setFont(u8g2_font_profont12_mf);
      }


  void displayText(String value, int x = 0, int y = 8);
  void refreshPrice(int lastPrice, int currentPrice);
  void setContrast(uint8_t contrast);
  void displayRotate(String text, int pix_length);

private:
  void displayPrice(String value);
  void clear();
  void flash();


  HardwareSerial &m_debug;
  U8G2 &m_display;
  bool m_enabled;
  uint8_t m_contrast;
};
