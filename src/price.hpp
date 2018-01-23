#pragma once
#include "config_common.hpp"
#include <Arduino.h>

class Price
{
public:
  Price(const String& price);
  Price(const double price, const bool display_float_part);
  void fromString(const String& price);
  void debug_print();
  friend bool operator< (const Price& lhs, const Price& rhs);
  friend bool operator==(const Price& lhs, const Price& rhs);
  friend bool operator> (const Price& lhs, const Price& rhs);
  friend bool operator<=(const Price& lhs, const Price& rhs);
  friend bool operator>=(const Price& lhs, const Price& rhs);
  friend bool operator!=(const Price& lhs, const Price& rhs);

  Price& operator+=(const double rhs);
  Price& operator-=(const double rhs);
  Price operator+(const double rhs);
  Price operator-(const double rhs);
  String toString();

  double getIncrement();
  int getIncrExponent();

  Price nextPrice();

  double delta(const Price& other);
  double get();

  bool isInitialized();
  void setDisplayFloatPart(bool display);
  bool displayFloatPart();
private:
  double m_price;

  uint8_t m_display_decimals;
  bool m_display_float_part;
  bool m_initialized;
};
