#include "config_common.hpp"
#include "display_action_multi.hpp"

using namespace std;

namespace Display { namespace Action {
void MultiRepeat::tick(DisplayT *display, double elapsed_time)
{
  m_elapsed_time += elapsed_time;
  if (m_duration >= 0 && m_elapsed_time > m_duration)
    setFinished(true);

  if (m_actions.empty())
    return;

  auto action = m_actions.front();
  if (action->isFinished()) {
    m_actions.pop();
    m_actions.push(action); // requeue
    action = m_actions.front();
    action->reset();
  }
  action->tick(display, elapsed_time);
}

void MultiRepeat::draw(DisplayT *display, Coords coords)
{
  m_actions.front()->draw(display,coords);
}

ActionPtr_t createRepeatedSlide(const Coords &direction, const double duration, const double slide_duration,
  ActionPtr_t action_a, ActionPtr_t action_b, action_callback_t onfinished_cb)
{
  queue<ActionPtr_t> q;
  q.push(action_a);
  q.push(make_shared<SlideTransition>(action_a, 1, action_b, 1, slide_duration, direction));
  q.push(action_b);
  q.push(make_shared<SlideTransition>(action_b, 1, action_a, 1, slide_duration, direction));

  return make_shared<MultiRepeat>(q, duration, onfinished_cb);
}

}}
