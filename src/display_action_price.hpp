#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"
#include <limits>

class Price
{
public:
  Price(const String& price) :
    m_price(0.0), m_display_decimals(6), m_display_float_part(false)
  {
    fromString(price);
  }

  Price(const double price, const bool display_float_part) :
    m_price(price), m_display_decimals(6), m_display_float_part(display_float_part)
  {}

  void fromString(const String& price)
  {
    int pos=price.indexOf('.');
    if (pos!=-1)
      m_display_float_part = true;

    m_price = std::atof(price.c_str());
  }

  void debug_print()
  {
    DEBUG_SERIAL.printf("price: '%.6f', toS: '%s', incr: '%0.9f'\n", m_price, toString().c_str(), increment());
  }

  friend bool operator< (const Price& lhs, const Price& rhs)
  {
    return (lhs.m_price < rhs.m_price);
  }
  friend bool operator==(const Price& lhs, const Price& rhs)
  {
    return (std::fabs(lhs.m_price - rhs.m_price) < std::numeric_limits<double>::epsilon());
  }
  friend bool operator> (const Price& lhs, const Price& rhs){ return rhs < lhs; }
  friend bool operator<=(const Price& lhs, const Price& rhs){ return !(lhs > rhs); }
  friend bool operator>=(const Price& lhs, const Price& rhs){ return !(lhs < rhs); }
  friend bool operator!=(const Price& lhs, const Price& rhs){ return !(lhs == rhs); }

  String toString()
  {
    char buf[16] = {0};
    if (m_display_float_part)
      snprintf(buf,15,"%.15f", m_price);
    else
      snprintf(buf,15,"%li", static_cast<long>(m_price));

    String number = String(buf).substring(0,m_display_decimals);
    if (number.endsWith("."))
      number = String(buf).substring(0,m_display_decimals-1);

    return number;
  }

  int increment_exponent()
  {
    if (!m_display_float_part)
      return 1;

    const String& s= toString();
    int pos = s.indexOf('.');
    if (pos==-1)
      return 1;

    int exponent = s.length()-1 - pos;
    return std::pow(10, exponent);
  }

  double increment()
  {
    return (1.0 / increment_exponent());
  }

  Price nextPrice()
  {
    return Price(m_price + increment(), m_display_float_part);
  }

  long delta(const Price& other)
  {
    return (long) ((m_price - other.m_price) * increment_exponent());
  }

private:
  double m_price;

  unsigned int m_display_decimals;
  bool m_display_float_part;
};

namespace Display {
class PriceAction : public ActionT
{
public:
  PriceAction(const double animation_speed, const Coords& coords=Coords{0,0})
    : ActionT(-1, coords), m_animation_speed(animation_speed), m_price(-1), m_last_price(-1),
      m_displayed_price(-1.0), m_ath_price(std::numeric_limits<int>::max())
    {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
  void updatePrice(const int new_price);
  void setATHPrice(const int ath_price);
private:
  void blinkIfATH(DisplayT *display);
  void blinkPixelIfReceivedPriceUpdate(DisplayT *display);

  double m_animation_speed;
  int m_price;
  int m_last_price;
  double m_displayed_price;
  int m_ath_price;
  double m_price_last_changed_at;
  double m_price_last_updated_at;
  const double m_price_timeout = 300.0; // after X seconds without receiving price updates, stop displaying it
  static constexpr double c_ath_animation_length = 4.0;
};
}
