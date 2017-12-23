void setup()
{
  setupSerial();
  setupTicker();
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
//  g_ticker_clock.attach(30.0, clock_callback);
  g_display->prependAction(g_clock_action);
}

void loop()
{
  if (g_start_ondemand_ap && !g_flash_button->pressed())
    startOnDemandAP();

  if (g_force_wipe==true)
    factoryReset();

  auto time = NTP.getTimeDateString();
  g_clock_action->updateTime(time);
  delay(100);
}
