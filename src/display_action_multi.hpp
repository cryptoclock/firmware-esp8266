#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

#include <queue>

namespace Display {
namespace Action {

typedef shared_ptr<ActionT> ActionPtr_t;

class MultiRepeat : public ActionT
{
public:
  MultiRepeat(const std::queue<ActionPtr_t> &actions, const double duration=-1, action_callback_t onfinished_cb = nullptr)
    : ActionT(duration, Coords{0,0}, onfinished_cb), m_actions(actions)
  {
  }

  // MultiRepeat(std::initializer_list<ActionPtr_t> args, const double duration=-1)
  //   : ActionT(duration, Coords{0,0})
  // {
  //   for (auto it = args.begin(); it!=args.end(); ++it)
  //     m_actions.push(*it);
  // }

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);

protected:
  std::queue<ActionPtr_t> m_actions;
};

ActionPtr_t createRepeatedSlide(const Coords &direction, const double duration, const double slide_duration,
  ActionPtr_t action_a, ActionPtr_t action_b, action_callback_t onfinished_cb = nullptr);

}}
