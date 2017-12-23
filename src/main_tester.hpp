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
