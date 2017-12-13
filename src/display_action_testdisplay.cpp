#include "config_common.hpp"
#include "display_action_testdisplay.hpp"

namespace Display {
namespace Action {
void TestDisplay::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;
}

void TestDisplay::draw(DisplayT *display, Coords coords)
{
  display->setBrightness(100);
  display->setDrawColor(1);

  int w = display->getDisplayWidth();
  int h = display->getDisplayHeight();

  int offset = (int) (m_elapsed_time*2) % 8;

  // modes
  switch(m_current_mode % 4) {
    case 0:
      display->fill({0,0});
      break;
    case 1: // walking columns
      for (int x=offset; x<w; x+=8)
        display->drawLine({x,0},{x,h});
      break;
    case 2: // walking rows
      for (int y=offset; y<h; y+=8)
        display->drawLine({0,y},{w,y});
      break;
    case 3: // walking diagonals
      for (int x=offset; x<w; x+=8)
        display->drawLine({x+8,0},{x,h});
      display->drawLine({offset,0},{0,offset});
      break;
    default:
      break;
  }
}

void TestDisplay::nextMode() { ++m_current_mode; }

}
}
