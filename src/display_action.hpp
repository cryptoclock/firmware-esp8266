#pragma once
#include "config_common.hpp"
#include <memory>
#include "coords.hpp"

using std::shared_ptr;

typedef std::function<void(void)> action_callback_t;

namespace Display {
class DisplayT;

class ActionT
{
public:
  ActionT(double duration, Coords coords, action_callback_t onfinished_cb = nullptr)
    : m_duration(duration), m_coords(coords), m_finished(false), m_elapsed_time(0.0), m_onfinished_cb(onfinished_cb)
  {}

  virtual void tick(DisplayT *display, double elapsed_time) = 0;
  virtual void draw(DisplayT *display, Coords coords) = 0;
  bool isFinished(void);
  void setFinished(bool status = true);

  double elapsedTime(void);
  // transition_from, transition_to
  void setCoords(Coords coords);
  virtual ~ActionT() = 0;
protected:
  double m_duration;
  Coords m_coords;
  bool m_finished;
  double m_elapsed_time;
  action_callback_t m_onfinished_cb;
};

inline ActionT::~ActionT() {}

namespace Action {
class SlideTransition : public ActionT
{
public:
  SlideTransition(shared_ptr<ActionT> actionA, shared_ptr<ActionT> actionB, double duration, const Coords& direction, const Coords& coords=Coords{0,0})
    : ActionT(duration, coords), m_actionA(actionA), m_actionB(actionB), m_direction(direction)
    {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
private:

  shared_ptr<ActionT> m_actionA;
  shared_ptr<ActionT> m_actionB;
  const Coords m_direction;
};
}
}
