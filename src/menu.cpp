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
