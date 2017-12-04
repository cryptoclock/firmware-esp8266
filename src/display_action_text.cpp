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
      display->setFont(m_font);
      coords += display->centerTextOffset(m_text);
      display->displayText(m_text, m_coords + coords);
    }

    void RotatingText::draw(DisplayT *display, Coords coords)
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
  }
}
