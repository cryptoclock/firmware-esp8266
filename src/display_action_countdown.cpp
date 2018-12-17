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
#include "display_action_countdown.hpp"
#include "ntp.hpp"

extern NTP g_NTP;

namespace Display {
namespace Action {
void CountDown::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

  if (m_elapsed_time>(m_countdown_secs + m_post_countdown_secs)) {
    setFinished();
    m_elapsed_time = 0;
  }
}

void CountDown::draw(DisplayT *display, Coords coords)
{
  char text[16] = {};
  if (isFinished()) {
    strncpy(text,"00:00",sizeof(text));
  } else {
    int secs = m_countdown_secs - m_elapsed_time;
    if (secs<0)
      secs = 0;
    int mins = secs / 60;
    secs = secs % 60;

    snprintf(text, sizeof(text), "%02i:%02i", mins, secs);
  }

  display->displayText(text, m_coords + coords + display->centerTextOffset(text));
}

}}
