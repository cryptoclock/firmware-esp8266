#include <Arduino.h>
#include "display.hpp"

Display::~Display() {}

void Display::setupTickCallback(Ticker::callback_t callback)
{
    m_ticker.attach(MILIS_PER_TICK / 1000.0,callback);
}

void Display::tick(void)
{
  if (m_actions.size()<1)
    return;

  auto& action = m_actions.at(0);
  action->tick(this);
  action->draw(this);
  if (action->isFinished()) {
    m_actions.erase(m_actions.begin());
  }
}

void Display::queueAction(shared_ptr<Action> action)
{
  m_actions.push_back(action);
}

void Display::prependAction(shared_ptr<Action> action)
{
  m_actions.insert(m_actions.begin(), action);
}
