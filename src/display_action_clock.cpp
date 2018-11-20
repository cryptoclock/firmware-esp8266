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

namespace Display {
namespace Action {
void Clock::tick(DisplayT *display, double elapsed_time)
{
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
  if (m_time=="") return;

  String text;
  // if (int(m_elapsed_time) % 2 == 0)
  text = m_time;
  // else
  //   text = m_time.substring(0,2) + " " + m_time.substring(3,5);

  display->displayText(text, m_coords + coords + display->centerTextOffset(text));
}

void Clock::updateTime(const String& time)
{
  if (time=="Time not set")
    m_time = "--:--";
  else
    m_time = time.substring(0,5);

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
