#include "config_common.hpp"
#include "display_action_clock.hpp"

namespace Display {
namespace Action {
void Clock::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

  if (m_duration <= 0.0) return;
  if (m_time=="" || m_elapsed_time > m_duration) {
    setFinished();
    m_elapsed_time = 0;
  }
}

void Clock::draw(DisplayT *display, Coords coords)
{
  if (m_time=="") return;

  display->setFont(m_font);
  String text;
  if (int(m_elapsed_time) % 2 == 0)
    text = m_time;
  else
    text = m_time.substring(0,2) + " " + m_time.substring(3,5);

  display->displayText(text, m_coords + coords, false);
}

void Clock::updateTime(const String& time)
{
  if (time=="Time not set")
    return;
  m_time = time.substring(0,5);
}
}
}
