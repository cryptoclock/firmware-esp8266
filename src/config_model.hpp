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
  #define X_DISPLAY_SMARTLED
  #define X_DISPLAY_WIDTH 6 // digits
  #define X_DISPLAY_LEDS 64
  #define X_DISPLAY_FASTLED_DRIVER NEOPIXEL
  #define X_DISPLAY_DATA_PIN 5 // !! for neopixel just use 'X' instead of 'DX', eg. if it's connected to D5(GPIO14), just use 5
  #define X_DISPLAY_LIXIE 1
  #include "LED.hpp"
  LED g_LED_layout[] = { // for each led, which digit(0-width) and number(0-9) it lights up
    {0,0},{0,1},{0,2},{0,3},{0,4},{0,5},{0,6},{0,7},{0,8},{0,9},
    {1,0},{1,1},{1,2},{1,3},{1,4},{1,5},{1,6},{1,7},{1,8},{1,9},
    {2,0},{2,1},{2,2},{2,3},{2,4},{2,5},{2,6},{2,7},{2,8},{2,9},
    {3,0},{3,1},{3,2},{3,3},{3,4},{3,5},{3,6},{3,7},{3,8},{3,9},
    {4,0},{4,1},{4,2},{4,3},{4,4},{4,5},{4,6},{4,7},{4,8},{4,9},
    {5,0},{5,1},{5,2},{5,3},{5,4},{5,5},{5,6},{5,7},{5,8},{5,9},
    {255,0},{255,1},{255,2},{255,3}
  };
  #include "display_smartled.hpp"
#elif defined(X_MODEL_1NA0100) // generic smartled output
  #define X_DISPLAY_SMARTLED
  #define X_DISPLAY_WIDTH 6 // digits
  #define X_DISPLAY_LEDS 64
  #define X_DISPLAY_FASTLED_DRIVER NEOPIXEL
  #define X_DISPLAY_DATA_PIN 5 // !! for neopixel just use 'X' instead of 'DX', eg. if it's connected to D5(GPIO14), just use 5
  #define X_DISPLAY_LIXIE 0
  #include "LED.hpp"
  LED g_LED_layout[] = { // for each led, which digit(0-width) and segment(A-G, DP) it lights up
    {0, SEG_A}, {0, SEG_B}, {0, SEG_C}, {0, SEG_D}, {0, SEG_E}, {0, SEG_F}, {0, SEG_G}, {0, SEG_DP},
    {1, SEG_A}, {1, SEG_B}, {1, SEG_C}, {1, SEG_D}, {1, SEG_E}, {1, SEG_F}, {1, SEG_G}, {1, SEG_DP},
    {2, SEG_A}, {2, SEG_B}, {2, SEG_C}, {2, SEG_D}, {2, SEG_E}, {2, SEG_F}, {2, SEG_G}, {2, SEG_DP},
    {3, SEG_A}, {3, SEG_B}, {3, SEG_C}, {3, SEG_D}, {3, SEG_E}, {3, SEG_F}, {3, SEG_G}, {3, SEG_DP},
    {4, SEG_A}, {4, SEG_B}, {4, SEG_C}, {4, SEG_D}, {4, SEG_E}, {4, SEG_F}, {4, SEG_G}, {4, SEG_DP},
    {5, SEG_A}, {5, SEG_B}, {5, SEG_C}, {5, SEG_D}, {5, SEG_E}, {5, SEG_F}, {5, SEG_G}, {5, SEG_DP},
    {6, SEG_A}, {6, SEG_B}, {6, SEG_C}, {6, SEG_D}, {6, SEG_E}, {6, SEG_F}, {6, SEG_G}, {6, SEG_DP},
    {7, SEG_A}, {7, SEG_B}, {7, SEG_C}, {7, SEG_D}, {7, SEG_E}, {7, SEG_F}, {7, SEG_G}, {7, SEG_DP},
  };
  #include "display_smartled.hpp"
#else
  #error Hardware model not defined!
#endif
