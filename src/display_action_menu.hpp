#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"
#include "menu.hpp"


namespace Display {
namespace Action {
class MenuWrapper : public ActionT
{
public:
  MenuWrapper(Menu* menu, const Coords& coords=Coords{0,0})
    : ActionT(-1, coords), m_menu(menu)
    {}
  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
private:
  Menu* m_menu;
};
}
}
