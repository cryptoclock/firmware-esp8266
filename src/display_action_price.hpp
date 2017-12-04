#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
  namespace Action {
    class Price : public ActionT
    {
    public:
      Price(const double animation_speed, const Coords& coords=Coords{0,0}, font_t font = nullptr)
        : ActionT(-1, coords, font), m_animation_speed(animation_speed), m_price(-1), m_last_price(-1), m_displayed_price(-1.0)
        {}

      void tick(DisplayT *display, double elapsed_time);
      void draw(DisplayT *display, Coords coords);
      void updatePrice(const int new_price);
    private:
      double m_animation_speed;
      int m_price;
      int m_last_price;
      double m_displayed_price;
    };
  }
}
