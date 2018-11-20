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
  alternate setup()/loop() for display tester mode
*/

shared_ptr<Display::Action::TestDisplay> g_test_display_action;

void setup()
{
  setupSerial();
  setupDisplay();
  g_test_display_action = make_shared<Display::Action::TestDisplay>();
  g_display->queueAction(g_test_display_action);

  g_flash_button = make_shared<Button>(PORTAL_TRIGGER_PIN);
  g_flash_button->onShortPress([&](){g_test_display_action->nextMode();});
//  g_flash_button->onLongPress([](){});
  g_flash_button->setupTickCallback([&]() { g_flash_button->tick(); });
}

void loop()
{
  g_flash_button->tick();
  delay(5);
}
