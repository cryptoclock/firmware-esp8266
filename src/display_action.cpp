#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"
#include <cmath>

double Action::elapsedTime(void)
{
  return m_elapsed_time;
}

bool Action::isFinished()
{
  return m_finished;
}

void Action::setFinished(bool status)
{
  m_finished = status;
}

/* === StaticTextAction === */
void StaticTextAction::tick(Display *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

  if (m_duration >= 0 && m_elapsed_time > m_duration)
    m_finished = true;
}

void StaticTextAction::draw(Display *display, Coords coords)
{
  display->setFont(m_font);
  coords += display->centerTextOffset(m_text);
  display->displayText(m_text, m_coords + coords);
}

/* === RotatingTextAction === */
void RotatingTextAction::draw(Display *display, Coords coords)
{
  display->setFont(m_font);
  int width = display->getTextWidth(m_text);
  int offset_x = (int)(m_elapsed_time * m_speed) % width;
  Coords offset_center = display->centerTextOffset(m_text);

  display->clearBuffer();
  display->displayText(m_text, m_coords + coords + Coords{-offset_x, offset_center.y}, false);
  display->displayText(m_text, m_coords + coords + Coords{-offset_x + width, offset_center.y}, false);
  display->sendBuffer();
}

/* === PriceAction === */
void PriceAction::tick(Display *display, double elapsed_time)
{
  if (m_price == m_last_price)
    return;

  m_elapsed_time += elapsed_time;

  // if the price changes mid-animation
  const bool outside_bounds =
    m_displayed_price < std::min(m_price, m_last_price) ||
    m_displayed_price > std::max(m_price, m_last_price);

  const double displayed_price_delta = fabs(m_displayed_price - m_price);
  const double price_delta = fabs(m_price - m_last_price);
  const double price_delta_from_start = outside_bounds ? 0 : fabs(m_displayed_price - m_last_price);
  const double price_delta_from_end = outside_bounds ? price_delta : fabs(m_displayed_price - m_price);

  double animation_multiplier = std::max(std::min(std::min(price_delta_from_start, price_delta_from_end), 3.0) / 3.0, 0.2);
  if (outside_bounds)
    animation_multiplier = 1.0;

  animation_multiplier *=  std::max(displayed_price_delta, 40.0) / 40.0; // boost the speed if the price difference is too large

  const double animation_speed = m_animation_speed * animation_multiplier;
  const double time_delta = elapsed_time * animation_speed;

  if (m_price >= m_displayed_price)
    m_displayed_price = std::min(m_displayed_price + time_delta, (double) m_price);
  else
    m_displayed_price = std::max(m_displayed_price - time_delta, (double) m_price);

  if (fabs(m_price - m_displayed_price) < 0.001) {
    m_displayed_price = m_price;
    m_last_price = m_price;
  }
}

void PriceAction::draw(Display *display, Coords coords)
{
  display->setFont(m_font);
  coords += m_coords;

  String price_top = String((int)m_displayed_price);
  String price_bottom = String((int)m_displayed_price + 1);
  if (m_price<0) {
    String text = "-----";
    display->displayText(text, coords + display->centerTextOffset(text));
    return;
  }

  // fractional part = vertical position of animated glyph(s)
  double tmp;
  double fract = (double) std::modf(m_displayed_price,&tmp);

  if (fract<0.01)
    price_bottom = price_top; // for horizontal shift during rollovers

  int offset_top = (int) (-fract * display->getDisplayHeight());
  int offset_bottom = offset_top + display->getDisplayHeight();
  coords += display->centerTextOffset(price_bottom); // higher price has more spaces

  if (price_bottom.length()>price_top.length())
    price_top = " " + price_top;

  display->clearBuffer();
  for (int i=0,offset_x=0;i<price_top.length();++i)
  {
    if (price_top[i]==price_bottom[i]) {
      display->drawGlyph(price_top.charAt(i), coords + Coords{offset_x, 0});
    } else {
      display->drawGlyph(price_top.charAt(i), coords + Coords{offset_x, offset_top});
      display->drawGlyph(price_bottom.charAt(i), coords + Coords{offset_x, offset_bottom});
    }
    offset_x += display->getTextWidth(String(price_bottom[i]))+1;
  }
  display->sendBuffer();
}

void PriceAction::updatePrice(const int new_price)
{
  m_last_price = m_price;
  if (m_price==-1) {
    m_last_price = new_price;
    m_displayed_price = new_price;
  }
  m_price = new_price;
}
//     //m_display->drawUTF8(0, 16, "B"); //""₿");

/* === ClockAction === */
void ClockAction::tick(Display *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

  if (m_duration <= 0.0) return;
  if (m_time=="" || m_elapsed_time > m_duration) {
    setFinished();
    m_elapsed_time = 0;
  }
}

void ClockAction::draw(Display *display, Coords coords)
{
  if (m_time=="") return;

  display->setFont(m_font);
  String text;
  if (int(m_elapsed_time) % 2 == 0)
    text = m_time;
  else
    text = m_time.substring(0,2) + " " + m_time.substring(3,5);

  display->displayText(text, m_coords + coords);
}

void ClockAction::updateTime(const String& time)
{
  if (time=="Time not set")
    return;
  m_time = time.substring(0,5);
}

/* === Transition === */

void SlideUpTransitionAction::tick(Display *display, double elapsed_time)
{
//  int offset_x = (int)(elapsedTimeSecs() * m_speed) % width;
}

void SlideUpTransitionAction::draw(Display *display, Coords coords)
{

}
