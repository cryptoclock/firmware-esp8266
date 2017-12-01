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
  m_elapsed_time += elapsed_time;
  double price_delta = elapsed_time * m_animation_speed;

//  DEBUG_SERIAL.printf("[PRICE] actual: %i, displayed %s\n", m_price, String(m_displayed_price).c_str());

  if (m_price >= m_displayed_price)
    m_displayed_price = std::min(m_displayed_price + price_delta, (double) m_price);
  else
    m_displayed_price = std::max(m_displayed_price - price_delta, (double) m_price);

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

  int offset_top = (int) (-fract * display->getDisplayHeight());
  int offset_bottom = offset_top + display->getDisplayHeight();
  coords += display->centerTextOffset(price_bottom); // higher price has more spaces

  if (price_bottom.length()>price_top.length())
    price_top = " " + price_top;

  display->clearBuffer();
//  DEBUG_SERIAL.printf("[PRICE] offset top: %i, bottom: %i\n", offset_top, offset_bottom);
  for (int i=0,offset_x=0;i<price_top.length();++i)
  {
    if (price_top[i]==price_bottom[i]) {
      display->drawGlyph(price_top.charAt(i), coords + Coords{offset_x, 0});
    } else {
      display->drawGlyph(price_top.charAt(i), coords + Coords{offset_x, offset_top});
      display->drawGlyph(price_bottom.charAt(i), coords + Coords{offset_x, offset_bottom});
    }
    offset_x += display->getTextWidth(String(price_top[i]))+1;
  }
  display->sendBuffer();
}

void PriceAction::updatePrice(const int new_price)
{
  if (m_price==-1)
    m_displayed_price = new_price;

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
