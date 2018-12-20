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

#include "config_common.hpp"
#include "display_action_price.hpp"
#include "data_source.hpp"
#include <cmath>

extern DataSource *g_data_source;

namespace Display {
void PriceAction::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

  /* send warning about receiving no price updates at 90% of set timeout,
     so that the server can send us repeated price in case of very slowly
     updating data sources */
  const float timeout_pre_warning = 0.90f;

  if (m_price_timeout > 0 &&
    (m_elapsed_time - m_price_last_updated_at) > (m_price_timeout * timeout_pre_warning))
  {
    if (!m_price_timeout_reported) {
      g_data_source->queueText(";WARN Data timeout imminent");
      m_price_timeout_reported = true;
    }
  }

  if (m_price == m_last_price)
    return;

  // if the price changes mid-animation
  const bool outside_bounds =
    m_displayed_price < std::min(m_price, m_last_price) ||
    m_displayed_price > std::max(m_price, m_last_price);

  const double displayed_price_delta = fabs(m_displayed_price.delta(m_price));
  const double price_delta = fabs(m_price.delta(m_last_price));
  const double price_delta_from_start = outside_bounds ? 0 : fabs(m_displayed_price.delta(m_last_price));
  const double price_delta_from_end = outside_bounds ? price_delta : fabs(m_displayed_price.delta(m_price));

  double animation_multiplier = std::max(std::min(std::min(price_delta_from_start, price_delta_from_end), 3.0) / 3.0, 0.2);
  if (outside_bounds)
    animation_multiplier = 1.0;

  animation_multiplier *=  std::max(displayed_price_delta, 40.0) / 40.0; // boost the speed if the price difference is too large

  const double animation_speed = m_animation_speed * animation_multiplier;
  const double time_delta = elapsed_time * animation_speed;

  double p_anim_delta = time_delta*m_displayed_price.getIncrement();
  if (m_price >= m_displayed_price) {
    m_displayed_price = std::min(m_displayed_price + p_anim_delta, m_price);
  } else {
    m_displayed_price = std::max(m_displayed_price - p_anim_delta, m_price);
  }

  if (fabs(m_price.delta(m_displayed_price)) < 0.001) {
     m_displayed_price = m_price;
     m_last_price = m_price;
  }
}

void PriceAction::blinkIfATH(DisplayT *display)
{
  if (m_ath && 
     m_displayed_price >= m_ath_price &&
    (m_elapsed_time - m_price_last_changed_at < c_ath_animation_length)) {
    if ((int)((m_elapsed_time - m_price_last_changed_at) * 10) % 4 < 2 ) {
      display->useBrightness(0);
    } else {
      display->useBrightness(255);
    }
  } else {
    display->resetBrightness();
  }
}

void PriceAction::blinkPixelIfReceivedPriceUpdate(DisplayT *display)
{
  // blink pixel when we received price update
  if (m_elapsed_time - m_price_last_updated_at <= 0.05)
    display->drawPixel({display->getDisplayWidth()-1, display->getDisplayHeight()-1});
}

void PriceAction::draw(DisplayT *display, Coords orig_coords)
{
  if (!display->isGraphic()) {
    // FIXME: for numeric-only displays
    if (display->isNumeric())
      display->displayNumber((int)m_displayed_price.get());
    return;
  }

  String postfix = "";
  auto draw_price = m_displayed_price;
  // FIXME: adjust number of digits displayed by display width
  if (draw_price.get() >= pow(10,6)-1) {
    if (draw_price.get() >= pow(10,10)-1) {
      draw_price /= pow(10,9);
      postfix = 'G';
    } else {
      draw_price /= pow(10,6);
      postfix = 'M';
    }
    draw_price.setDisplayFloatPart(true);
    draw_price.fromString(draw_price.toString());
  }
  
  Coords coords = orig_coords + m_coords;

  String price_top = draw_price.toString() + postfix;
  String price_bottom = draw_price.nextPrice().toString() + postfix;
  if (!m_price.isInitialized() || (m_price_timeout > 0 && (m_elapsed_time - m_price_last_updated_at) > m_price_timeout))
  {
    String text = "-----";
    display->displayText(text, coords + display->centerTextOffset(text));
    return;
  }

  // fractional part = vertical position of animated glyph(s)
  double tmp;
  double fract = (double) std::modf(draw_price.get(),&tmp);
  fract = (double) std::modf(fract*draw_price.getIncrExponent(),&tmp);
  if (fract>0.999)
    fract = 0.0;
  if (fract<0.001)
    fract = 0.1;

  // FIXME:
  // if (fract<(0.01*draw_price.getIncrement()))
  //   price_bottom = price_top; // for horizontal shift during rollovers

  int offset_top = (int) (-fract * display->getDisplayHeight());
  int offset_bottom = offset_top + display->getDisplayHeight();

  if (price_bottom.length()>price_top.length())
    price_top = " " + price_top;

  blinkIfATH(display);

  coords += display->centerTextOffset(price_bottom); // higher price has more spaces

  int offset_x = 0;
  for (unsigned int i=0;i<price_top.length();++i)
  {
    const int offset_dot_top = (price_top.charAt(i)=='.') ? -1 : 0;
    const int offset_dot_bottom = (price_bottom.charAt(i)=='.') ? -1 : 0;
    if (price_top[i]==price_bottom[i]) {
      display->drawGlyph(price_top.charAt(i), coords + Coords{offset_x + offset_dot_top, 0});
    } else {
      display->drawGlyph(price_top.charAt(i), coords + Coords{offset_x + offset_dot_top, offset_top});
      display->drawGlyph(price_bottom.charAt(i), coords + Coords{offset_x + offset_dot_bottom, offset_bottom});
    }
    offset_x += display->getTextWidth(String(price_bottom[i]))+1;
  }

  if (display->getDisplayHeight() >= 64 ) {
    auto old_font = display->getFont();
    // TODO:
    display->setFont(2);
    display->displayTextHCentered("USD/BTC",orig_coords + Coords{0,0});
    display->setFont(old_font);
  }

  blinkPixelIfReceivedPriceUpdate(display);
  // blink pixel when we received price update
  if (m_elapsed_time - m_price_last_updated_at <= 0.05)
    display->drawPixel({display->getDisplayWidth()-1, display->getDisplayHeight()-1});
}

void PriceAction::updatePrice(const String& n_price)
{
  Price new_price(n_price);
  if (new_price.displayFloatPart())
    m_display_float_part = true;
  new_price.setDisplayFloatPart(m_display_float_part); // once we receive price with floating point part, it's always enabled

  if (m_price_timeout_reported) { // recovering from price timeout
    g_data_source->queueText(";DIAG data_timeout_recovered");
  }

  m_price_last_updated_at = m_elapsed_time;
  m_price_timeout_reported = false;
  if (m_price == new_price)
    return;

  if (new_price>m_ath_price)
    m_ath_price = new_price;

  m_last_price = m_price;
  if (!m_price.isInitialized()) {
    m_last_price = new_price;
    m_displayed_price = new_price;
  }
  m_price = new_price;

  m_price_last_changed_at = m_elapsed_time;
}

void PriceAction::setATHPrice(const String& ath_price)
{
  if (ath_price=="off")
    m_ath = false;
  else
    m_ath_price = Price(ath_price);
}

void PriceAction::reset()
{
  m_price = Price("");
  m_last_price = Price("");
  m_displayed_price = Price("");
  m_display_float_part = false;
}

void PriceAction::setPriceTimeout(double timeout)
{
  m_price_timeout = timeout;
}

}
