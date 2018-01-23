#include "config_common.hpp"
#include "display_action_price.hpp"
#include <cmath>

namespace Display {
void PriceAction::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

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
  if (m_displayed_price >= m_ath_price &&
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

  Coords coords = orig_coords + m_coords;

  String price_top = m_displayed_price.toString();
  String price_bottom = m_displayed_price.nextPrice().toString();
  if (!m_price.isInitialized() || (m_elapsed_time - m_price_last_updated_at) > m_price_timeout)
  {
    String text = "-----";
    display->displayText(text, coords + display->centerTextOffset(text));
    return;
  }

  // fractional part = vertical position of animated glyph(s)
  double tmp;
  double fract = (double) std::modf(m_displayed_price.get(),&tmp);
  fract = (double) std::modf(fract*m_displayed_price.getIncrExponent(),&tmp);
  if (fract>0.999)
    fract = 0.0;
  if (fract<0.001)
    fract = 0.1;

  // FIXME:
  // if (fract<(0.01*m_displayed_price.getIncrement()))
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

  m_price_last_updated_at = m_elapsed_time;
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
  m_ath_price = Price(ath_price);
}

void PriceAction::reset()
{
  m_price = Price("");
  m_last_price = Price("");
  m_displayed_price = Price("");
  m_display_float_part = false;
}

}
