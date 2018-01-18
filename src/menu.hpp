#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <memory>
#include "display.hpp"
#include "button.hpp"
#include "parameter_store.hpp"

using std::vector;
using Display::Coords;

typedef std::function<void(const String& value)> menuitem_onchange_callback_t;
typedef std::function<void(void)> menuitem_action_callback_t;

class MenuItem
{
public:
  MenuItem(const String& name, const String& display_name, const String &display_name_short, menuitem_onchange_callback_t onchange_cb)
    : m_name(name), m_display_name(display_name), m_display_name_short(display_name_short),
    m_onchange_cb(onchange_cb), m_active(false)
  {
  }
  virtual ~MenuItem();

  virtual void draw(DisplayT *display, const Coords& coords) = 0;
  virtual void onLongPress() = 0;
  virtual void onShortPress() = 0;
  virtual const String getValue() const = 0;
  virtual void setValue(const String& value) = 0;
  //void setOnChange(menuitem_onchange_callback_t cb) { m_onchange_cb = cb; }
  bool isActive() const { return m_active; }
  virtual void activate() { m_active = true; }
  virtual void deactivate() { m_active = false; }

  String getName() const { return m_name; }
protected:
  String m_name;
  String m_display_name;
  String m_display_name_short;
  menuitem_onchange_callback_t m_onchange_cb;
  bool m_active;
};

inline MenuItem::~MenuItem() {}

class MenuItemNumericRange : public MenuItem
{
public:
  MenuItemNumericRange(const String& name, const String& display_name, const String& display_name_short,
    const int start, const int end, const int current_value, menuitem_onchange_callback_t onchange_cb)
    : MenuItem(name, display_name, display_name_short, onchange_cb), m_start(start), m_end(end), m_current(current_value)
  {}

  void onLongPress();
  void onShortPress();
  const String getValue() const;
  void setValue(const String& value);

  void draw(DisplayT *display, const Coords& coords) override;
private:
  const int m_start;
  const int m_end;
  int m_current;
};

class MenuItemBoolean : public MenuItem
{
public:
  MenuItemBoolean(const String& name, const String& display_name, const String& display_name_short,
    const bool current_value, const String& on_text, const String& off_text, menuitem_onchange_callback_t onchange_cb)
    : MenuItem(name, display_name, display_name_short, onchange_cb), m_current(current_value), m_on_text(on_text), m_off_text(off_text)
  {}

  void onLongPress();
  void onShortPress();
  const String getValue() const;
  void setValue(const String& value);

  void draw(DisplayT *display, const Coords& coords) override;

private:
  bool m_current;
  String m_on_text, m_off_text;
};

class MenuItemAction : public MenuItem
{
public:
  MenuItemAction(const String& name, const String& display_name, const String& display_name_short,
    menuitem_action_callback_t action_cb)
    : MenuItem(name, display_name, display_name_short, nullptr), m_action(action_cb)
  {}

  void onLongPress();
  void onShortPress();
  const String getValue() const;
  void setValue(const String& value);
  void draw(DisplayT *display, const Coords& coords) override;
  void activate() override;
private:
  menuitem_action_callback_t m_action;
};

typedef vector<shared_ptr<MenuItem>> menu_items_t;

class Menu
{
public:
  Menu(ParameterStore* parameters, const menu_items_t& items) :
    m_parameters(parameters), m_items(items), m_current(m_items.begin()),
    m_end_of_menu_callback(nullptr), m_finished(false)
  {
  }

  void start(DisplayT *display, button_callback_t changeMode);
  void draw(DisplayT *display, const Coords& coords);

  void onLongPress();
  void onShortPress();

  bool isFinished() { return m_finished; }

  void end();
private:
  void saveParameters();

  ParameterStore *m_parameters;

  menu_items_t m_items;
  menu_items_t::iterator m_current;
  button_callback_t m_end_of_menu_callback;
  bool m_finished;
};
