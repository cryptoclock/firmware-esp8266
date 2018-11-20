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
  Action for displaying static/rotating text message
*/

#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
namespace Action {
class StaticText : public ActionT
{
public:
  StaticText(const String& text, const double duration, const Coords& coords = Coords{0,0}, action_callback_t onfinished_cb = nullptr)
    : ActionT(duration, coords, onfinished_cb), m_text(text)
  {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
protected:
  String m_text;
};

class RotatingText : public StaticText
{
public:
  RotatingText(const String& text, const double duration, const int speed, const Coords& coords = Coords{0,0}, action_callback_t onfinished_cb = nullptr)
    : StaticText(text, duration, coords, onfinished_cb), m_speed(speed)
  {}

  void draw(DisplayT *display, Coords coords) override;

private:
  int m_speed;
};

class RotatingTextOnce : public StaticText
{
public:
  RotatingTextOnce(const String& text, const int speed, const Coords& coords = Coords{0,0}, action_callback_t onfinished_cb = nullptr)
    : StaticText(text, -1, coords, onfinished_cb), m_speed(speed)
  {
  }

  void draw(DisplayT *display, Coords coords) override;
  void tick(DisplayT *display, double elapsed_time) override;
private:
  int m_speed;
};

}
}
