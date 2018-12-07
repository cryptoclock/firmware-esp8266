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
  alternate setup()/loop() if the ticker is configured to act as internet clock only
*/

void setup()
{
  setupSerial();
  setupHW();
  setupDisplay();
  loadParameters();
  setupMenu();

  /* WiFi */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("--> WiFi ", -1, 20));
  connectToWiFi();
  g_display->removeBottomAction();
  g_display->queueAction(make_shared<Display::Action::StaticText>(WiFi.SSID(), 1.0));

  /* Update */
  g_display->queueAction(make_shared<Display::Action::RotatingText>("UPDATING... ", -1, 20));
  Firmware::update(g_parameters["update_url"]);

  setupButton();
  setupNTP();
  g_clock_action = make_shared<Display::Action::Clock>(-1, Coords{0,0}); // display clock
  g_display->prependAction(g_clock_action);
}

void loop()
{
  if (g_start_ondemand_ap && !g_flash_button->pressed())
    startOnDemandAP();

  if (g_force_wipe==true)
    factoryReset();

  delay(100);
}
