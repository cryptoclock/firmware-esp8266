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
  Action for displaying NTP clock
*/

#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
namespace Action {
class Clock : public ActionT
{
public:
  Clock(double duration, const Coords& coords=Coords{0,0})
    : ActionT(duration, coords), m_time(""), m_always_on(false), m_is_time_set(false)
    {}
  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
  void setAlwaysOn(bool always_on);
  bool isAlwaysOn();
  bool isTimeSet() { return m_is_time_set;}
private:
  void updateTime();
  String m_time;
  bool m_always_on;
  bool m_is_time_set;
};
}
}
