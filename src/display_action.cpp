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
  if (m_finished && m_onfinished_cb)
    m_onfinished_cb();
}

/* === Transition === */

void Action::SlideTransition::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;
  if (m_actionA) m_actionA->tick(display, elapsed_time);
  if (m_actionB) m_actionB->tick(display, elapsed_time);
  if (elapsedTime() > m_duration)
    setFinished();
}

double slide_function(double position)
{
  double s = 0.5 + sin((position-0.5)*M_PI)*0.5;
  return std::max(std::min(s,1.0),0.0);
}

void Action::SlideTransition::draw(DisplayT *display, Coords coords)
{
  const int width = display->getDisplayWidth() * m_direction.x;
  const int height = display->getDisplayHeight() * m_direction.y;

  const int offset_x_a = round(slide_function(elapsedTime() / m_duration) * width);
  const int offset_x_b = offset_x_a - width;

  const int offset_y_a = round(slide_function(elapsedTime() / m_duration) * height);
  const int offset_y_b = offset_y_a - height;

  if (m_actionA) m_actionA->draw(display, coords+Coords{offset_x_a, offset_y_a});
  if (m_actionB) m_actionB->draw(display, coords+Coords{offset_x_b, offset_y_b});
}
}
