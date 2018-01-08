#include "config_common.hpp"
#include "display_action_text.hpp"

namespace Display {
namespace Action {
void StaticText::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

  if (m_duration >= 0 && m_elapsed_time > m_duration)
    m_finished = true;
}

void StaticText::draw(DisplayT *display, Coords coords)
{
  coords += display->centerTextOffset(m_text);
  display->displayText(m_text, m_coords + coords);
}

void RotatingText::draw(DisplayT *display, Coords coords)
{
  int width = display->getTextWidth(m_text);
  int offset_x = (int)(m_elapsed_time * m_speed) % width;
  Coords offset_center = display->centerTextOffset(m_text);

  display->displayText(m_text, m_coords + coords + Coords{-offset_x, offset_center.y});
  display->displayText(m_text, m_coords + coords + Coords{-offset_x + width, offset_center.y});
}

void RotatingTextOnce::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;
}

void RotatingTextOnce::draw(DisplayT *display, Coords coords)
{
  const int text_length = display->getTextWidth(m_text);
  Coords offset = display->centerTextOffset(m_text);;
  offset.x = display->getDisplayWidth() - (int)(m_elapsed_time * m_speed);

  if (offset.x < -text_length) {
    setFinished(true);
    return;
  }

  display->displayText(m_text, m_coords + coords + offset);
}
}
}
