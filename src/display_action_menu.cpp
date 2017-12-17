#include "config_common.hpp"
#include "display_action_menu.hpp"

namespace Display {
namespace Action {
void MenuWrapper::tick(DisplayT *display, double elapsed_time)
{
  if (m_menu->isFinished())
    setFinished(true);
//  m_elapsed_time += elapsed_time;
}

void MenuWrapper::draw(DisplayT *display, Coords coords)
{
  if (m_font)
    display->setFont(m_font);

  m_menu->draw(display, coords);
}

}
}
