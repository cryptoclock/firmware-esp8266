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

#include "LED.hpp"

uint8_t LED::charTo7seg(char ch)
{
  switch(tolower(ch)) {
    case '0': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F; break;
    case '1': return SEG_B|SEG_C; break;
    case '2': return SEG_A|SEG_B|SEG_D|SEG_E|SEG_G; break;
    case '3': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_G; break;
    case '4': return SEG_B|SEG_C|SEG_F|SEG_G; break;
    case '5': return SEG_A|SEG_C|SEG_D|SEG_F|SEG_G; break;
    case '6': return SEG_A|SEG_C|SEG_D|SEG_F|SEG_G; break;
    case '7': return SEG_A|SEG_B|SEG_C; break;
    case '8': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_E|SEG_F|SEG_G; break;
    case '9': return SEG_A|SEG_B|SEG_C|SEG_D|SEG_F|SEG_G; break;
    case ' ': return 0; break;
    case '-': return SEG_G; break;
    case '=': return SEG_D|SEG_G; break;
    case '.': return SEG_DP; break;
    case 'a': return SEG_A|SEG_B|SEG_C|SEG_E|SEG_F|SEG_G; break;
    case 'b': return SEG_C|SEG_D|SEG_E|SEG_F|SEG_G; break;
    case 'c': return SEG_D|SEG_E|SEG_G; break;
    case 'd': return SEG_B|SEG_C|SEG_D|SEG_E|SEG_G; break;
    case 'e': return SEG_A|SEG_D|SEG_E|SEG_F|SEG_G; break;
    case 'f': return SEG_A|SEG_E|SEG_F|SEG_G; break;
    case 'h': return SEG_C|SEG_E|SEG_F|SEG_G; break;
    case 'i': return SEG_E|SEG_F; break;
    case 'j': return SEG_B|SEG_C|SEG_E|SEG_F|SEG_G; break;
    case 'l': return SEG_D|SEG_E|SEG_F; break;
    case 'n': return SEG_C|SEG_E|SEG_G; break;
    case 'o': return SEG_C|SEG_D|SEG_E|SEG_G; break;
    case 'p': return SEG_A|SEG_B|SEG_E|SEG_F|SEG_G; break;
    case 'r': return SEG_E|SEG_G; break;
    case 's': return SEG_A|SEG_C|SEG_D|SEG_F|SEG_G; break;
    case 't': return SEG_A|SEG_E|SEG_F; break;
    case 'u': return SEG_B|SEG_C|SEG_D|SEG_E|SEG_F; break;
    case 'x': return SEG_B|SEG_C|SEG_E|SEG_F|SEG_G; break;
    case 'y': return SEG_B|SEG_C|SEG_D|SEG_F|SEG_G; break;
    case 'z': return SEG_A|SEG_B|SEG_D|SEG_E|SEG_G; break;
    default: break;
  }
  return 0;
}

uint8_t LED::charToLixie(char ch)
{
  switch(tolower(ch)) {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      return (ch-'0'); break;
    default: break;
  }
  return 255;
}
