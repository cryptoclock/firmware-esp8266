#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
namespace Action {

class TestDisplay : public ActionT
{
public:
  TestDisplay()
    : ActionT(-1, {0,0}), m_current_mode(0)
  {
  }

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
  void nextMode();
protected:
  int m_current_mode;
};

}
}
