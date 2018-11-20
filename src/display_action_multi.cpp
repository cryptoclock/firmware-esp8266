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
#include "display_action_multi.hpp"

using namespace std;

namespace Display { namespace Action {
void MultiRepeat::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;
  if (m_duration > 0 && m_elapsed_time > m_duration)
    setFinished(true);

  if (m_actions.empty()) {
    setFinished(true);
    return;
  }

  auto action = m_actions.front();
  if (action->isFinished()) {
    m_actions.pop();
    if (m_repeat)
      m_actions.push(action); // requeue
    else if (m_actions.empty()) {
      setFinished(true);
      return;
    }
    action = m_actions.front();
    action->reset();
  }
  action->tick(display, elapsed_time);
}

void MultiRepeat::draw(DisplayT *display, Coords coords)
{
  if (m_actions.empty())
    return;
  m_actions.front()->draw(display,coords);
}

ActionPtr_t createRepeatedSlide(const Coords &direction, const double duration, const double slide_duration,
  ActionPtr_t action_a, ActionPtr_t action_b, action_callback_t onfinished_cb)
{
  queue<ActionPtr_t> q;
  q.push(action_a);
  q.push(make_shared<SlideTransition>(action_a, 1, action_b, 1, slide_duration, direction));
  q.push(action_b);
  q.push(make_shared<SlideTransition>(action_b, 1, action_a, 1, slide_duration, direction));

  return make_shared<MultiRepeat>(q, duration, true, onfinished_cb);
}

}}
