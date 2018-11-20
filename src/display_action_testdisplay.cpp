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

#include "config_common.hpp"
#include "display_action_testdisplay.hpp"

#include "display.hpp"
#include "display_u8g2.hpp"

namespace Display {
namespace Action {
void TestDisplay::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;
}

void TestDisplay::draw(DisplayT *display, Coords coords)
{
  display->setDrawColor(1);

  int w = display->getDisplayWidth();
  int h = display->getDisplayHeight();

  int offset = (int) (m_elapsed_time*8) % 8;
  int brightness = (int) (m_elapsed_time*8) % 16;

  // modes
  switch(m_current_mode % 6) {
    case 0:
      display->setDisplayBrightness(255);
      display->fill({0,0});
      break;
    case 1: // walking columns
      display->setDisplayBrightness(255);
      for (int x=offset; x<w; x+=8)
        display->drawLine({x,0},{x,h});
      break;
    case 2: // walking rows
      display->setDisplayBrightness(255);
      for (int y=offset; y<h; y+=8)
        display->drawLine({0,y},{w,y});
      break;
    case 3: // walking diagonals
      display->setDisplayBrightness(255);
      for (int x=offset; x<w; x+=8)
        display->drawLine({x+8,0},{x,h});
      display->drawLine({offset,0},{0,offset});
      break;
    case 4: // brightness
      {
        display->setDisplayBrightness(brightness*16);
        Coords coords{1,-2};
        String text("Bright " + String(brightness));
        display->displayText(text, coords);
      }
      break;
    case 5: // numbers
      {
        display->setDisplayBrightness(255);
        Coords coords{4,-1};
        int n = ((int)(m_elapsed_time) % 50) / 5;

        String text(String(n) + String(n) + String(n) + String(n) + String(n));
        display->displayText(text, coords);
      }
      break;
    default:
      break;
  }
}

void TestDisplay::nextMode() { ++m_current_mode; }

}
}
