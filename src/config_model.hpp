/* models */
#if defined(X_MODEL_3DA0100)
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
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
