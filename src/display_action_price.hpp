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

/*
  Action for animated display of price/ticker data
*/

#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"
#include "price.hpp"
#include <limits>

namespace Display {
class PriceAction : public ActionT
{
public:
  PriceAction(const double animation_speed, const Coords& coords=Coords{0,0})
    : ActionT(-1, coords), m_animation_speed(animation_speed), m_price(""), m_last_price(""),
      m_displayed_price(""), m_ath_price(""), m_price_timeout(300.0), m_display_float_part(false),
      m_price_timeout_reported(false)
    {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
  void updatePrice(const String &price);
  void setATHPrice(const String &ath_price);
  void reset();

  void setPriceTimeout(double timeout);
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
  double m_price_timeout; // after X seconds without receiving price updates, stop displaying it
  static constexpr double c_ath_animation_length = 4.0;
  bool m_display_float_part;
  bool m_price_timeout_reported;
};
}
