#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"
#include <limits>

namespace Display {
namespace Action {
class Price : public ActionT
{
public:
  Price(const double animation_speed, const Coords& coords=Coords{0,0}, font_t font = nullptr)
    : ActionT(-1, coords, font), m_animation_speed(animation_speed), m_price(-1), m_last_price(-1),
      m_displayed_price(-1.0), m_ath_price(std::numeric_limits<int>::max())
    {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
  void updatePrice(const int new_price);
  void setATHPrice(const int ath_price);
private:
  double m_animation_speed;
  int m_price;
  int m_last_price;
  double m_displayed_price;
  int m_ath_price;
  double m_price_last_updated_at;
  const double m_price_timeout = 30.0; // after X seconds without receiving price updates, stop displaying it
};
}
}
