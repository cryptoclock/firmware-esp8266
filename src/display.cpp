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
#include "display.hpp"

namespace Display {
DisplayT::~DisplayT() {}

Coords DisplayT::centerTextOffset(const String& text)
{
  return Coords {
    (int) std::max(ceil((getDisplayWidth() - getTextWidth(text)) / 2.0), 0.0),
    - (int) std::max(floor((getDisplayHeight() - getCurrentFontHeight()) / 2.0), 0.0)
  };
}

void DisplayT::setupTickCallback(Ticker::callback_t callback)
{
    m_ticker.attach(c_milis_per_tick / 1000.0,callback);
}

void DisplayT::tick(void)
{
  unsigned long current_time = micros();
  double elapsed_time = (current_time - m_last_tick_at) / 1000000.0;
  m_last_tick_at = micros();

  while(true) {
    if (m_actions.empty())
      return;

    auto action = m_actions.at(0);
    action->tick(this, elapsed_time);

    if (action->isFinished()) {
      m_actions.erase(m_actions.begin());
      continue;
    }
    clearBuffer();
    resetBrightness();
    action->draw(this, Coords{0,0});
    sendBuffer();
    break;
  }
}

void DisplayT::queueAction(shared_ptr<ActionT> action)
{
  action->setFinished(false);
  m_actions.push_back(action);
}

void DisplayT::prependAction(shared_ptr<ActionT> action)
{
  action->setFinished(false);
  action->tick(this, 0.0);
  m_actions.insert(m_actions.begin(), action);
}

void DisplayT::replaceAction(shared_ptr<ActionT> action)
{
  action->setFinished(false);
  action->tick(this, 0.0);
  m_actions[0] = action;
}

void DisplayT::removeBottomAction(void)
{
  if (!m_actions.empty())
    m_actions.pop_back();
}

void DisplayT::removeTopAction(void)
{
  if (!m_actions.empty())
    m_actions.erase(m_actions.begin());
}

shared_ptr<ActionT> DisplayT::getTopAction(void)
{
  if (m_actions.empty())
    return nullptr;
  return m_actions[0];
}


void DisplayT::cleanQueue(void)
{
  m_actions.clear();
}

void DisplayT::setFont(const uint8_t font)
{
  m_current_font = font;
  useFont();
};

const uint8_t DisplayT::getFont()
{
  return m_current_font;
}

void DisplayT::displayTextHCentered(const String& value, const Coords& coords)
{
  auto offset = DisplayT::centerTextOffset(value);
  displayText(value, coords + Coords{offset.x,0});
}

}
