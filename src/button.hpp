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
  Class for control buttons. By default, FLASH button is used as sole controller button for the whole device.
*/

#pragma once
#include "config_common.hpp"
#include <Ticker.h>
#include <Arduino.h>
#include <functional>

typedef std::function<void()> button_callback_t;

class Button {
public:
  Button(uint8_t pin) :
    m_short_press_cb(nullptr), m_long_press_cb(nullptr), m_super_long_press_cb(nullptr), m_pin(pin),
    m_currently_pressed(false), m_pressed_at(0), m_long_press_dispatched(false), m_super_long_press_dispatched(false)
  {
    pinMode(m_pin, INPUT);
  }

  void setupTickCallback(Ticker::callback_function_t callback);

  void onShortPress(button_callback_t func);
  void onLongPress(button_callback_t func);
  void onSuperLongPress(button_callback_t func);
  void tick();
  bool pressed() { return m_currently_pressed; }
private:
  button_callback_t m_short_press_cb;
  button_callback_t m_long_press_cb;
  button_callback_t m_super_long_press_cb;
  uint8_t m_pin;
  bool m_currently_pressed;
  unsigned long m_pressed_at;
  bool m_long_press_dispatched;
  bool m_super_long_press_dispatched;
  Ticker m_ticker;

  static const unsigned int m_long_press_delay = 1000;
  static const unsigned int m_super_long_press_delay = 10000;
};
