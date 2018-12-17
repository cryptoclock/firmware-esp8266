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
  Action for displaying countdown timer
*/

#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
namespace Action {
class CountDown : public ActionT
{
public:
  CountDown(int countdown_secs, int post_countdown_secs, const Coords& coords=Coords{0,0}, action_callback_t onfinished_cb = nullptr)
     : ActionT(0, coords, onfinished_cb), m_countdown_secs(countdown_secs), m_post_countdown_secs(post_countdown_secs)
  {}
  void tick(DisplayT *display, double elapsed_time) override;
  void draw(DisplayT *display, Coords coords) override;
private:
  int m_countdown_secs;
  int m_post_countdown_secs;
};
}
}
