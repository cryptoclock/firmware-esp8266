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

/* Bitmaps for Cryptoclock logo, split to upper and lower part, 32x8 each */

#pragma once

static const unsigned char s_crypto2_bits[] PROGMEM = {
   0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x80, 0x01, 0x53, 0x4b, 0x8f, 0x71,
   0xc3, 0x48, 0xd3, 0x9b, 0xc3, 0x70, 0x93, 0x99, 0xd3, 0x40, 0x8f, 0x99,
   0xce, 0x38, 0x03, 0x71, 0x00, 0x00, 0x00, 0x00
};

static const unsigned char s_clock_inverted_bits[] PROGMEM = {
   0xff, 0xff, 0xff, 0xff, 0xc7, 0x3c, 0x8e, 0xd9, 0xb3, 0x9c, 0x65, 0xe9,
   0xf3, 0x9c, 0xe5, 0xf1, 0xf3, 0x9c, 0xe5, 0xf1, 0xb3, 0x9c, 0x65, 0xe9,
   0xc7, 0x30, 0x8e, 0xd9, 0xff, 0xff, 0xff, 0xff
};
