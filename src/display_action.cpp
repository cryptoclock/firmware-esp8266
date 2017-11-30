#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

double Action::elapsedTimeSecs(void)
{
  return (m_ticks * MILIS_PER_TICK) / 1000.0;
}

int Action::elapsedTimeTicks(void)
{
  return m_ticks;
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
void StaticTextAction::tick(Display *display)
{
  ++m_ticks;
  if (m_duration >= 0 && elapsedTimeSecs() > m_duration)
    m_finished = true;
}

void StaticTextAction::draw(Display *display, Coords coords)
{
  if (m_font) display->setFont(m_font);
  coords += display->centerTextOffset(m_text);
  display->displayText(m_text, m_coords + coords);
}

/* === RotatingTextAction === */
void RotatingTextAction::draw(Display *display, Coords coords)
{
  if (m_font) display->setFont(m_font);
  int width = display->getTextWidth(m_text);
  int offset_x = (int)(elapsedTimeSecs() * m_speed) % width;
  Coords offset_center = display->centerTextOffset(m_text);

  display->clearBuffer();
  display->displayText(m_text, m_coords + coords + Coords{-offset_x, offset_center.y}, false);
  display->displayText(m_text, m_coords + coords + Coords{-offset_x + width, offset_center.y}, false);
  display->sendBuffer();
}

/* === PriceAction === */
void PriceAction::tick(Display *display) { ++m_ticks; }

void PriceAction::draw(Display *display, Coords coords)
{
  if (m_font) display->setFont(m_font);

  String text = String(m_price);
  if (m_price<0)
    text = "-----";

  coords += display->centerTextOffset(text);

  display->displayText(text, m_coords + coords);
}

void PriceAction::updatePrice(const int new_price)
{
  m_last_price = m_last_price>=0 ? m_price : new_price;
  m_price = new_price;
}
//     //m_display->drawUTF8(0, 16, "B"); //""₿");

/* === ClockAction === */
void ClockAction::tick(Display *display)
{
  ++m_ticks;

  if (m_duration <= 0.0) return;
  if (m_time=="" || elapsedTimeSecs() > m_duration) {
    setFinished();
    m_ticks = 0;
  }
}

void ClockAction::draw(Display *display, Coords coords)
{
  if (m_time=="") return;

  String text;
  if (m_font) display->setFont(m_font);
  if (int(elapsedTimeSecs()) % 2 == 0)
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

void SlideUpTransitionAction::tick(Display *display)
{
//  int offset_x = (int)(elapsedTimeSecs() * m_speed) % width;
}

void SlideUpTransitionAction::draw(Display *display, Coords coords)
{

}
