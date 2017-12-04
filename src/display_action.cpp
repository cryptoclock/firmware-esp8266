#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
  double ActionT::elapsedTime(void)
  {
    return m_elapsed_time;
  }

  bool ActionT::isFinished()
  {
    return m_finished;
  }

  void ActionT::setFinished(bool status)
  {
    m_finished = status;
  }

  /* === Transition === */

  void Action::SlideUpTransition::tick(DisplayT *display, double elapsed_time)
  {
  //  int offset_x = (int)(elapsedTimeSecs() * m_speed) % width;
  }

  void Action::SlideUpTransition::draw(DisplayT *display, Coords coords)
  {

  }
}
