#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
namespace Action {
class Clock : public ActionT
{
public:
  Clock(double duration, const Coords& coords=Coords{0,0})
    : ActionT(duration, coords), m_time(""), m_always_on(false)
    {}
  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
  void updateTime(const String& time);
  void setAlwaysOn(bool always_on);
  bool isAlwaysOn();
private:
  String m_time;
  bool m_always_on;
};
}
}
