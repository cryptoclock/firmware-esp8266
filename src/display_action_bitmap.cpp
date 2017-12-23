#include "config_common.hpp"
#include "display_action_bitmap.hpp"

namespace Display {
namespace Action {
void StaticBitmap::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;

  if (m_duration >= 0 && m_elapsed_time > m_duration)
    m_finished = true;
}

void StaticBitmap::draw(DisplayT *display, Coords coords)
{
  display->displayBitmapP(m_data, coords, m_width, m_height);
}

}
}
