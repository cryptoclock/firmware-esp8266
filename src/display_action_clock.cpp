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
#include "display_action_clock.hpp"
#include "ntp.hpp"

extern NTP g_NTP;

namespace Display {
namespace Action {
void Clock::tick(DisplayT *display, double elapsed_time)
{
  updateTime();
  m_elapsed_time += elapsed_time;

  if (m_always_on)
    return;

  if (m_duration <= 0.0) return;
  if (m_time=="" || m_elapsed_time > m_duration) {
    setFinished();
    m_elapsed_time = 0;
  }
}

void Clock::draw(DisplayT *display, Coords coords)
{
  if (m_time=="")
    return;

  display->displayText(m_time, m_coords + coords + display->centerTextOffset(m_time));
}

void Clock::updateTime()
{
  String t = g_NTP.getTime();

  if (t=="" || t.length()<16) {
    m_time = "--:--";
    m_is_time_set = false;
  } else {
    //"Thu Dec 13 01:46:24 2018"
    m_time = t.substring(11,16);
    m_is_time_set = true;
  }
}

void Clock::setAlwaysOn(bool always_on)
{
  m_always_on = always_on;
}

bool Clock::isAlwaysOn()
{
  return m_always_on;
}


}}
