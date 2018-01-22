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
    if (price=="") {
      m_initialized = false;
    } else {
      fromString(price);
      m_initialized = true;
    }
  }

  Price(const double price, const bool display_float_part) :
    m_price(price), m_display_decimals(6), m_display_float_part(display_float_part),
    m_initialized(true)
  {}

  void fromString(const String& price)
  {
    int pos=price.indexOf('.');
    if (pos!=-1)
      m_display_float_part = true;

    m_price = std::atof(price.c_str());
    if (m_display_float_part)
      m_price = std::atof(toString().c_str());
  }

  void debug_print()
  {
    DEBUG_SERIAL.printf("[Price] '%.6f', toS: '%s', incr: '%0.9f'\n", m_price, toString().c_str(), increment());
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

  Price& operator+=(const double rhs)
  {
    m_price += rhs;
    return *this;
  }
  Price& operator-=(const double rhs)
  {
    m_price -= rhs;
    return *this;
  }
  Price operator+(const double rhs)
  {
    Price res = *this;
    return (res += rhs);
  }
  Price operator-(const double rhs)
  {
    Price res = *this;
    return (res -= rhs);
  }

  String toString()
  {
    uint8_t l_digits = String((int) m_price).length();
    uint8_t r_digits = m_display_decimals - l_digits - 1;

    if (m_display_float_part && (l_digits+1<m_display_decimals)) {
      String str =  String(m_price, r_digits+5);
      return str.substring(0,l_digits+1+r_digits);
    } else {
      return String((int)m_price).substring(0,m_display_decimals);
    }
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

  double delta(const Price& other)
  {
    return ((m_price - other.m_price) * increment_exponent());
  }

  double get() {
    return m_price;
  }

  bool isInitialized()
  {
    return m_initialized;
  }

  void setDisplayFloatPart(bool display)
  {
    m_display_float_part = display;
  }
  bool displayFloatPart()
  {
    return m_display_float_part;
  }

private:
  double m_price;

  uint8_t m_display_decimals;
  bool m_display_float_part;
  bool m_initialized;
};

namespace Display {
class PriceAction : public ActionT
{
public:
  PriceAction(const double animation_speed, const Coords& coords=Coords{0,0})
    : ActionT(-1, coords), m_animation_speed(animation_speed), m_price(""), m_last_price(""),
      m_displayed_price(""), m_ath_price(""), m_display_float_part(false)
    {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
  void updatePrice(const String &price);
  void setATHPrice(const String &ath_price);
  void reset();
private:
  void blinkIfATH(DisplayT *display);
  void blinkPixelIfReceivedPriceUpdate(DisplayT *display);

  double m_animation_speed;
  Price m_price;
  Price m_last_price;
  Price m_displayed_price;
  Price m_ath_price;
  double m_price_last_changed_at;
  double m_price_last_updated_at;
  const double m_price_timeout = 300.0; // after X seconds without receiving price updates, stop displaying it
  static constexpr double c_ath_animation_length = 4.0;
  bool m_display_float_part;
};
}
