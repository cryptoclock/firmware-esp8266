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
#include "display_action_menu.hpp"

namespace Display {
namespace Action {
void MenuWrapper::tick(DisplayT *display, double elapsed_time)
{
  if (m_menu->isFinished())
    setFinished(true);
//  m_elapsed_time += elapsed_time;
}

void MenuWrapper::draw(DisplayT *display, Coords coords)
{
  m_menu->draw(display, m_coords + coords);
}

}
}
