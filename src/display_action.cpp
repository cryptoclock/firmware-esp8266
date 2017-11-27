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

/* === StaticTextAction === */
void StaticTextAction::tick(Display *display) { ++m_ticks; }

void StaticTextAction::draw(Display *display)
{
  if (m_font) display->setFont(m_font);
  display->displayText(m_text, m_coords.x, m_coords.y);
}

bool StaticTextAction::isFinished(void)
{
  if (m_duration <= 0.0) return false;
  return elapsedTimeSecs() > m_duration;
}

/* === RotatingTextAction === */
void RotatingTextAction::draw(Display *display)
{
  int width = display->getTextWidth(m_text) + m_coords.x;
  int offset_x = (int)(elapsedTimeSecs() * m_speed) % width;
  if (m_font) display->setFont(m_font);
  display->displayText(m_text, m_coords.x - offset_x, m_coords.y);
}

/* === PriceAction === */
void PriceAction::tick(Display *display) { ++m_ticks; }
bool PriceAction::isFinished(void) { return false; }

void PriceAction::draw(Display *display)
{
  if (m_font) display->setFont(m_font);

  String text = String(m_price);
  if (m_price<0)
    text = "-----";

  int width = display->getTextWidth(text);
  int offset_x = (display->getDisplayWidth() - width) / 2.0;

  display->displayText(text, m_coords.x + offset_x, m_coords.y);
}

void PriceAction::updatePrice(const int new_price)
{
  m_last_price = m_last_price>=0 ? m_price : new_price;
  m_price = new_price;
}
//     //m_display->drawUTF8(0, 16, "B"); //""₿");

/* === ClockAction === */
void ClockAction::tick(Display *display) { ++m_ticks; }
bool ClockAction::isFinished(void)
{
  if (m_time=="")
    return true;

  if (m_duration <= 0.0) return false;
  if (elapsedTimeSecs() > m_duration) {
    m_ticks = 0; // reset
    return true;
  }
  return false;
}

void ClockAction::draw(Display *display)
{
  if (m_time=="") return;

  String text;
  if (m_font) display->setFont(m_font);
  if (int(elapsedTimeSecs()) % 2 == 0)
    text = m_time;
  else
    text = m_time.substring(0,2) + " " + m_time.substring(3,5);

  display->displayText(text, m_coords.x, m_coords.y);
}

void ClockAction::updateTime(const String& time)
{
  if (time=="Time not set")
    return;
  m_time = time.substring(0,5);
}
