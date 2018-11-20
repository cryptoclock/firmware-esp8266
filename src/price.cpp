/* 
  Cryptoclock ESP8266
  Copyright (C) 2018 www.cryptoclock.net <info@cryptoclock.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "price.hpp"

Price::Price(const String& price) :
  m_price(0.0), m_display_decimals(6), m_display_float_part(false)
{
  if (price=="") {
    m_initialized = false;
  } else {
    fromString(price);
    m_initialized = true;
  }
}

Price::Price(const double price, const bool display_float_part) :
  m_price(price), m_display_decimals(6), m_display_float_part(display_float_part), m_initialized(true)
{}

void Price::fromString(const String& price)
{
  int pos=price.indexOf('.');
  if (pos!=-1)
    m_display_float_part = true;

  m_price = std::atof(price.c_str());
  if (m_display_float_part)
    m_price = std::atof(toString().c_str());
}

void Price::debug_print()
{
  DEBUG_SERIAL.printf("[Price] '%.6f', toS: '%s', incr: '%0.9f'\n", m_price, toString().c_str(), getIncrement());
}

bool operator< (const Price& lhs, const Price& rhs)
{
  return (lhs.m_price < rhs.m_price);
}
bool operator==(const Price& lhs, const Price& rhs)
{
  return (std::fabs(lhs.m_price - rhs.m_price) < std::numeric_limits<double>::epsilon());
}
bool operator> (const Price& lhs, const Price& rhs){ return rhs < lhs; }
bool operator<=(const Price& lhs, const Price& rhs){ return !(lhs > rhs); }
bool operator>=(const Price& lhs, const Price& rhs){ return !(lhs < rhs); }
bool operator!=(const Price& lhs, const Price& rhs){ return !(lhs == rhs); }

Price& Price::operator+=(const double rhs)
{
  m_price += rhs;
  return *this;
}
Price& Price::operator-=(const double rhs)
{
  m_price -= rhs;
  return *this;
}
Price Price::operator+(const double rhs)
{
  Price res = *this;
  return (res += rhs);
}
Price Price::operator-(const double rhs)
{
  Price res = *this;
  return (res -= rhs);
}

String Price::toString()
{
  uint8_t l_digits = String((int) m_price).length();
  uint8_t r_digits = m_display_decimals - l_digits - 1;

  if (m_display_float_part && m_price >= 1.0)
    r_digits = 3;

  if (m_display_float_part && m_price >= 10.0)
    r_digits = 2;

  if (m_display_float_part && m_price >= 100.0)
    r_digits = 1;

  if (m_display_float_part && m_price >= 1000.0) {
    m_display_float_part = false;
    r_digits = 0;
  }

  if (m_display_float_part && (l_digits+1<m_display_decimals)) {
    String str =  String(m_price, r_digits+5);
    if (m_price<1.0)
      return str.substring(1,1 + 2 + r_digits);
    else
      return str.substring(0,l_digits+1+r_digits);
  } else {
    return String((int)m_price).substring(0,m_display_decimals);
  }
}

int Price::getIncrExponent()
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

double Price::getIncrement()
{
  return (1.0 / getIncrExponent());
}

Price Price::nextPrice()
{
  return Price(m_price + getIncrement(), m_display_float_part);
}

double Price::delta(const Price& other)
{
  return ((m_price - other.m_price) * getIncrExponent());
}

double Price::get() {
  return m_price;
}

bool Price::isInitialized()
{
  return m_initialized;
}

void Price::setDisplayFloatPart(bool display)
{
  m_display_float_part = display;
}

bool Price::displayFloatPart()
{
  return m_display_float_part;
}
