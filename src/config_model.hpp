/* 
  Cryptoclock ESP8266
  Copyright (C) 2018 www.cryptoclock.net <info@cryptoclock.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/* 
  Configuration for models with different hardware capabilities (mostly different displays).
  Because of the limited available RAM in ESP8266, this usually has to be configured at compile time, 
  via environment variable X_MODEL_NUMBER passed through script tools/model_number.py
*/

#define X_DISPLAY_MILIS_PER_TICK 33

#if defined(X_MODEL_3DA0100) || defined(X_MODEL_3DA0105) // default model, 4 max7219 8x8 led modules for total of 32x8 display
  #if defined(X_MODEL_3DA0105) // model with gyroscope module for automatic display orientation
    #define HAS_GYROSCOPE
    #define HAS_SOUND
    #define SOUND_PIN D8
  #endif
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_ALTAIR01)
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D5, /* data=*/ D7, /* cs=*/ D8, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_3DX0100) // as above, 6 modules
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 48
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_48X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_3DB0100) // as above, 8 modules
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 64
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_64X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_3DC0100) // model with SSD1306 (or variants) 128x64 oled display, experimental support
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
#elif defined(X_MODEL_3DT0100) // model to be used as ntp clock only (no websocket/ticker information)
  #define X_CLOCK_ONLY_DISPLAY
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_TESTER) // model for testing display LEDs
  #define X_TEST_DISPLAY
  #define X_DISPLAY_U8G2
  #define X_DISPLAY_WIDTH 32
  #define X_DISPLAY_HEIGHT 8
  #include "display_u8g2.hpp"
  U8G2_MAX7219_32X8_F_4W_SW_SPI g_display_hw(U8G2_R0, /* clock=*/ D7, /* data=*/ D5, /* cs=*/ D6, /* dc=*/ U8X8_PIN_NONE, /* reset=*/ U8X8_PIN_NONE);
#elif defined(X_MODEL_7LA0100) // model with 7-segment led display module with TM1637 driver
  #define X_DISPLAY_TM1637
  #define X_DISPLAY_WIDTH 4 // digits
  #define X_DISPLAY_HEIGHT 1
  #include "display_tm1637.hpp"
  TM1637Display g_display_hw(/* clock=*/ D4, /* data=*/ D5);
#elif defined(X_MODEL_7LB0100) // model with 7-segment led display module with MAX7219 driver
  #define X_DISPLAY_MAX7219
  #define X_DISPLAY_WIDTH 8 // digits
  #define X_DISPLAY_HEIGHT 1
  #include "display_max7219.hpp"
  DigitLedDisplay g_display_hw(/* data=*/ D5, /* cs=*/ D6, /* clock=*/ D7);
#elif defined(X_MODEL_1LA0100) // model with lixie (LED plexiglass nixie) display
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
  #define X_DISPLAY_DATA_PIN 5 // !! for neopixel just use 'X' instead of 'DX', eg. if it's connected to D5(GPIO14), just use 5
#else
  #error Hardware model not defined!
#endif
