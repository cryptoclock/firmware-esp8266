#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
  namespace Action {
    class Clock : public ActionT
    {
    public:
      Clock(double duration, const Coords& coords=Coords{0,0}, font_t font = nullptr)
        : ActionT(duration, coords, font), m_time("")
        {}
      void tick(DisplayT *display, double elapsed_time);
      void draw(DisplayT *display, Coords coords);
      void updateTime(const String& time);
    private:
      String m_time;
    };
  }
}
