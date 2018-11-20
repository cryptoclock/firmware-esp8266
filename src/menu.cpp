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

#include "menu.hpp"
#include "utils.hpp"
#include "display_action_menu.hpp"

#include "display_u8g2.hpp"

using std::shared_ptr;

void Menu::start(DisplayT *display, button_callback_t changeMode)
{
  m_finished = false;
  display->prependAction(std::make_shared<Display::Action::MenuWrapper>(this, Coords{1,-1}));
  m_end_of_menu_callback = changeMode;

  // load item values from parameters
  for (auto item: m_items) {
    auto parameter = m_parameters->findByName(item->getName());
    if (parameter)
      item->setValue(parameter->value);
  }

  m_current = m_items.begin();
}

void Menu::onLongPress()
{
  auto item = (*m_current);
  if (item->isActive()) {
    item->onLongPress();
    m_parameters->setIfExistsAndTriggerCallback(item->getName(), item->getValue(), true);
    saveParameters();
  } else {
    item->activate();
  }
}

void Menu::end()
{
  m_finished = true;
  m_current = m_items.begin();
  if (m_end_of_menu_callback)
    m_end_of_menu_callback();
}

void Menu::onShortPress()
{
  auto item = *m_current;
  if (item->isActive()) {
    item->onShortPress();
    m_parameters->setIfExistsAndTriggerCallback(item->getName(), item->getValue(), false);
  } else {
    m_current++;
    if (m_current==m_items.end())
      end();
  }
}

void Menu::saveParameters()
{
  m_parameters->storeToEEPROM();
}

void Menu::draw(DisplayT *display, const Coords& coords)
{
  (*m_current)->draw(display, coords);
}
