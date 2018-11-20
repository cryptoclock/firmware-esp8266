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

#include "button.hpp"

void Button::setupTickCallback(Ticker::callback_t callback)
{
  m_ticker.attach(100.0 / 1000.0,callback);
}

void Button::onShortPress(button_callback_t func)
{
  m_short_press_cb = func;
}

void Button::onLongPress(button_callback_t func)
{
  m_long_press_cb = func;
}

void Button::onSuperLongPress(button_callback_t func)
{
  m_super_long_press_cb = func;
}

void Button::tick(void)
{
  unsigned long elapsed_time = millis() - m_pressed_at;
  bool state = (digitalRead(m_pin) == LOW);

  if (state && m_currently_pressed && elapsed_time > m_long_press_delay && !m_long_press_dispatched) {
    if (m_long_press_cb) m_long_press_cb();
    m_long_press_dispatched = true;
  }

  if (state && m_currently_pressed && elapsed_time > m_super_long_press_delay && !m_super_long_press_dispatched) {
    if (m_super_long_press_cb) m_super_long_press_cb();
    m_super_long_press_dispatched = true;
  }

  if (state != m_currently_pressed) { // state change
    if (m_currently_pressed==false) { // button down
      m_pressed_at = millis();
    } else { // button released
      if (elapsed_time <= m_long_press_delay) {
        if (m_short_press_cb) m_short_press_cb();
      }
    }
    m_currently_pressed = !m_currently_pressed;
    m_long_press_dispatched = false;
    m_super_long_press_dispatched = false;
  }
}
