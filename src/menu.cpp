#include "config_common.hpp"

#include "menu.hpp"
#include "display_action_menu.hpp"


using std::shared_ptr;

void Menu::start(DisplayT *display, button_callback_t changeMode)
{
  m_finished = false;
  display->prependAction(std::make_shared<Display::Action::MenuWrapper>(this));
  m_end_of_menu_callback = changeMode;

//    m_button->onLongPress ...
//  m_button->onShortPress(nullptr);
//  m_button->onLongPress(nullptr);
}

void Menu::onLongPress()
{
  // activate menu item
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
  m_current++;
  if (m_current==m_items.end())
    end();
}

void Menu::draw(DisplayT *display, const Coords& coords)
{
  (*m_current)->draw(display, coords);
}


void MenuItemNumericRange::draw(DisplayT *display, const Coords& coords)
{
  display->displayText(m_display_name, coords);
}

void MenuItemBoolean::draw(DisplayT *display, const Coords& coords)
{
  display->displayText(m_display_name, coords);
}
