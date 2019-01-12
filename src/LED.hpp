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

#pragma once
#include "Arduino.h"
#include "config_common.hpp"

// 7-segment segments
#define SEG_A 1 << 0
#define SEG_B 1 << 1
#define SEG_C 1 << 2
#define SEG_D 1 << 3
#define SEG_E 1 << 4
#define SEG_F 1 << 5
#define SEG_G 1 << 6
#define SEG_DP 1 << 7

struct LED 
{
  LED(uint8_t digit, uint8_t segment) : digit(digit), segment(segment), index(0) {}
  LED(void) : digit(0), segment(0), index(0) {}

  uint8_t digit;
  uint8_t segment;

  // if multiple leds are used for same segment, this index is used as indicator of their relative position for color-cycling effects,
  // with zero being leftmost/topmost led of the pack
  uint8_t index;

  static uint8_t charTo7seg(char ch);
  static uint8_t charToLixie(char ch);
};
