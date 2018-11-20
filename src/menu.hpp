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
  Configuration menu
*/

#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <memory>
#include "display.hpp"
#include "button.hpp"
#include "parameter_store.hpp"
#include "menu_item.hpp"

using Display::Coords;
typedef std::vector<shared_ptr<MenuItem>> menu_items_t;

class Menu
{
public:
  Menu(ParameterStore* parameters, const menu_items_t& items) :
    m_parameters(parameters), m_items(items), m_current(m_items.begin()),
    m_end_of_menu_callback(nullptr), m_finished(false)
  {
  }

  void start(DisplayT *display, button_callback_t changeMode);
  void draw(DisplayT *display, const Coords& coords);

  void onLongPress();
  void onShortPress();

  bool isFinished() { return m_finished; }

  void end();
private:
  void saveParameters();

  ParameterStore *m_parameters;

  menu_items_t m_items;
  menu_items_t::iterator m_current;
  button_callback_t m_end_of_menu_callback;
  bool m_finished;
};
