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
  Display coordinates helper class
*/

#pragma once
#include "config_common.hpp"

namespace Display {
struct Coords {
  int x,y;

  Coords& operator+=(const Coords& other)
  {
    this->x += other.x;
    this->y += other.y;
    return *this;
  }
  Coords& operator-=(const Coords& other)
  {
    this->x -= other.x;
    this->y -= other.y;
    return *this;
  }

  friend Coords operator+(Coords lhs, const Coords& rhs)
  {
    lhs += rhs;
    return lhs;
  }
  friend Coords operator-(Coords lhs, const Coords& rhs)
  {
    lhs -= rhs;
    return lhs;
  }
};
}
