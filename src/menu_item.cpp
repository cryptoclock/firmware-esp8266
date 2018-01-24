#include "menu_item.hpp"

void MenuItemNumericRange::draw(DisplayT *display, const Coords& coords)
{
  if (isActive()) {
    String text = m_display_name_short + " " + String(m_current);
    display->displayText(text, coords);
  } else {
    display->displayText(m_display_name, coords);
  }
}

const String MenuItemNumericRange::getValue() const
{
  String s = String(m_current);
  return s;
}

void MenuItemNumericRange::setValue(const String& value)
{
  m_current = value.toInt();
}

void MenuItemNumericRange::onShortPress()
{
  if (++m_current>m_end)
    m_current = m_start;

  if (m_onchange_cb)
    m_onchange_cb(getValue());
}

void MenuItemNumericRange::onLongPress()
{
  deactivate();
}

// ----

void MenuItemBoolean::draw(DisplayT *display, const Coords& coords)
{
  if (isActive()) {
    String text = m_display_name_short + (m_current ? m_on_text : m_off_text);
    display->displayText(text, coords);
  } else {
    display->displayText(m_display_name, coords);
  }
}

void MenuItemBoolean::onShortPress()
{
  m_current = !m_current;

  if (m_onchange_cb)
    m_onchange_cb(getValue());
}

void MenuItemBoolean::onLongPress()
{
  deactivate();
}

const String MenuItemBoolean::getValue() const
{
  String s = m_current ? "1" : "0";
  return s;
}

void MenuItemBoolean::setValue(const String& value)
{
  m_current = (value == "1");
}

// ----

void MenuItemEnum::draw(DisplayT *display, const Coords& coords)
{
  if (isActive()) {
    const String text = m_display_name_short + " " + String(m_values.at(m_current));
    display->displayText(text, coords);
  } else {
    display->displayText(m_display_name, coords);
  }
}

const String MenuItemEnum::getValue() const
{
  String s = String(m_current);
  return s;
}

void MenuItemEnum::setValue(const String& value)
{
  m_current = value.toInt();
}

void MenuItemEnum::onShortPress()
{
  if (++m_current>=m_values.size())
    m_current = 0;

  if (m_onchange_cb)
    m_onchange_cb(getValue());
}

void MenuItemEnum::onLongPress()
{
  deactivate();
}

// ----

void MenuItemAction::draw(DisplayT *display, const Coords& coords)
{
  display->displayText(m_display_name, coords);
}

void MenuItemAction::onShortPress() {}
void MenuItemAction::onLongPress()
{
  deactivate();
}

void MenuItemAction::activate()
{
  if (m_action)
    m_action();
}

const String MenuItemAction::getValue() const
{
  return "";
}

void MenuItemAction::setValue(const String& value) {}
