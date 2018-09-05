/* models */
#define X_DISPLAY_MILIS_PER_TICK 33

#if defined(X_MODEL_3DA0100)
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_3DX0100)
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 48
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_48X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_3DB0100)
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 64
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_64X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_3DC0100)
  #undef X_DISPLAY_MILIS_PER_TICK
  #define X_DISPLAY_MILIS_PER_TICK 50
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 128
  #define X_DISPLAY_HEIGHT 64
  #include "display_u8g2.hpp"

  // U8G2_SSD1306_128X64_NONAME_F_HW_I2C
  // U8G2_SSD1306_128X64_VCOMH0_F_HW_I2C
  // U8G2_SSD1306_128X64_ALT0_F_HW_I2C
  // U8G2_SH1106_128X64_NONAME_F_HW_I2C
  // U8G2_SH1106_128X64_VCOMH0_F_HW_I2C
  // U8G2_SH1106_128X64_WINSTAR_F_HW_I2C
  // U8G2_SH1107_128X64_F_HW_I2C
  // U8G2_SSD1309_128X64_NONAME2_F_HW_I2C
  // U8G2_SSD1309_128X64_NONAME0_F_HW_I2C
  // U8G2_SSD1325_NHD_128X64_F_HW_I2C
  U8G2_SSD1306_128X64_NONAME_F_HW_I2C g_display_hw(U8G2_R0); // nodemcu HW I2C - D1 clock, D2 data
#elif defined(X_MODEL_3DT0100)
  #define X_CLOCK_ONLY_DISPLAY
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_TESTER)
  #define X_TEST_DISPLAY
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_7LA0100) // 7seg led
  #define X_DISPLAY_TM1637
  #define X_DISPLAY_WIDTH 4 // digits
  #define X_DISPLAY_HEIGHT 1
  #include "display_tm1637.hpp"
  TM1637Display g_display_hw(/* clock=*/ D4, /* data=*/ D5);
#elif defined(X_MODEL_1LA0100) // lixie
  #define X_DISPLAY_LIXIE
  #define X_DISPLAY_WIDTH 6 // digits
  #define X_DISPLAY_HEIGHT 1
  #define X_DISPLAY_DATA_PIN D5
  #include "display_lixie.hpp"
  Lixie g_display_hw;
#elif defined(X_MODEL_1NA0100) // generic neopixel output
  #define X_DISPLAY_NEOPIXEL
  #define X_DISPLAY_WIDTH 8
  #define X_DISPLAY_HEIGHT 8
  #include "display_neopixel.hpp"
  #define X_DISPLAY_DATA_PIN 5 // !! for neopixel just X instead of DX
#else
  #error Hardware model not defined!
#endif
