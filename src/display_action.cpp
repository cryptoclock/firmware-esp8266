#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

StaticTextAction::StaticTextAction(const String& text, const Coords& coords, const double duration)
  : m_text(text), m_coords(coords), m_duration(duration), m_ticks(0)
{}

void StaticTextAction::tick(Display *display)
{
  ++m_ticks;
}

void StaticTextAction::draw(Display *display)
{
  display->displayText(m_text, m_coords.x, m_coords.y);
}

bool StaticTextAction::isFinished(void)
{
  return (m_ticks * MILIS_PER_TICK > m_duration * 1000.0);
}
