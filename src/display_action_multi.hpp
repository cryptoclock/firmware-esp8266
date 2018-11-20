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

/*
  Action for repeated transitioning between 2+ actions.
*/

#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

#include <queue>

namespace Display {
namespace Action {

typedef shared_ptr<ActionT> ActionPtr_t;


class MultiRepeat : public ActionT
{
public:
  MultiRepeat(const std::queue<ActionPtr_t> &actions, const double duration=-1, bool repeat=true, action_callback_t onfinished_cb = nullptr)
    : ActionT(duration, Coords{0,0}, onfinished_cb), m_actions(actions), m_repeat(repeat)
  {
  }

  // MultiRepeat(std::initializer_list<ActionPtr_t> args, const double duration=-1)
  //   : ActionT(duration, Coords{0,0})
  // {
  //   for (auto it = args.begin(); it!=args.end(); ++it)
  //     m_actions.push(*it);
  // }

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);

protected:
  std::queue<ActionPtr_t> m_actions;
  bool m_repeat;
};

ActionPtr_t createRepeatedSlide(const Coords &direction, const double duration, const double slide_duration,
  ActionPtr_t action_a, ActionPtr_t action_b, action_callback_t onfinished_cb = nullptr);

}}
