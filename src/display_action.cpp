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

#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
double ActionT::elapsedTime(void)
{
  return m_elapsed_time;
}

bool ActionT::isFinished()
{
  return m_finished;
}

void ActionT::setFinished(bool status)
{
  m_finished = status;
  if (m_finished && m_onfinished_cb)
    m_onfinished_cb();
}

void ActionT::reset()
{
  m_elapsed_time = 0.0;
  m_finished = false;
}

/* === Transition === */

void Action::SlideTransition::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;
  if (m_actionA) m_actionA->tick(display, elapsed_time);
  if (m_actionB) m_actionB->tick(display, elapsed_time);
  if (elapsedTime() > m_duration)
    setFinished();
}

double slide_function(double position)
{
  double s = 0.5 + sin((position-0.5)*M_PI)*0.5;
  return std::max(std::min(s,1.0),0.0);
}

void Action::SlideTransition::draw(DisplayT *display, Coords coords)
{
  const int width = display->getDisplayWidth() * m_direction.x;
  const int height = display->getDisplayHeight() * m_direction.y;

  const int offset_x_a = round(slide_function(elapsedTime() / m_duration) * width);
  const int offset_x_b = offset_x_a - width;

  const int offset_y_a = round(slide_function(elapsedTime() / m_duration) * height);
  const int offset_y_b = offset_y_a - height;

  if (m_draw_priorityA>=m_draw_priorityB) {
    if (m_actionA) m_actionA->draw(display, coords+Coords{offset_x_a, offset_y_a});
    display->fill(coords+Coords{offset_x_b, offset_y_b}, 0);
    if (m_actionB) m_actionB->draw(display, coords+Coords{offset_x_b, offset_y_b});
  } else {
    if (m_actionB) m_actionB->draw(display, coords+Coords{offset_x_b, offset_y_b});
    display->fill(coords+Coords{offset_x_a, offset_y_a}, 0);
    if (m_actionA) m_actionA->draw(display, coords+Coords{offset_x_a, offset_y_a});
  }
}
}
