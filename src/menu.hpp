#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <memory>
#include "display.hpp"
#include "button.hpp"

using std::vector;
using Display::Coords;

class MenuItem
{
public:
  MenuItem(const String& name, const String& display_name)
    : m_name(name), m_display_name(display_name)
  {
  }
  virtual ~MenuItem();

  virtual void draw(DisplayT *display, const Coords& coords) = 0;
  // display (virtual)
  // next_item (virtual)
protected:
  String m_name;
  String m_display_name;
//  vector<MenuItemOption> m_options;
//  vector<MenuItemOption>::iterator m_current;
};

inline MenuItem::~MenuItem() {}

class MenuItemNumericRange : public MenuItem
{
public:
  MenuItemNumericRange(const String& name, const String& display_name, const int start, const int end, const int current_value)
    : MenuItem(name, display_name), m_start(start), m_end(end), m_current(current_value)
  {}

  void draw(DisplayT *display, const Coords& coords) override;
private:
  const int m_start;
  const int m_end;
  int m_current;
};

class MenuItemBoolean : public MenuItem
{
public:
  MenuItemBoolean(const String& name, const String& display_name, const bool current_value)
    : MenuItem(name, display_name), m_value(current_value)
  {}
  void draw(DisplayT *display, const Coords& coords) override;

private:
  bool m_value;
};

typedef vector<shared_ptr<MenuItem>> menu_items_t;

class Menu
{
public:
  Menu(const menu_items_t& items) :
    m_items(items), m_current(m_items.begin()), m_end_of_menu_callback(nullptr), m_finished(false)
  {
  }

  void start(DisplayT *display, button_callback_t changeMode);
  void draw(DisplayT *display, const Coords& coords);

  void onLongPress();
  void onShortPress();

  bool isFinished() { return m_finished; }
private:
  void end(void);

  menu_items_t m_items;
  menu_items_t::iterator m_current;
  button_callback_t m_end_of_menu_callback;
  bool m_finished;
};
