#include <Arduino.h>
#include "display.hpp"

Display::~Display() {}

void Display::displayText(const String& value, Coords coords)
{
  displayText(value, coords.x, coords.y);
}


void Display::setupTickCallback(Ticker::callback_t callback)
{
    m_ticker.attach(MILIS_PER_TICK / 1000.0,callback);
}

void Display::tick(void)
{
  while(true) {
    if (m_actions.size()<1)
      return;

    auto action = m_actions.at(0);
    action->tick(this);

    if (action->isFinished()) {
      m_actions.erase(m_actions.begin());
      continue;
    }
    action->draw(this, Coords{0,0});
    break;
  }
}

void Display::queueAction(shared_ptr<Action> action)
{
  action->setFinished(false);
  m_actions.push_back(action);
}

void Display::prependAction(shared_ptr<Action> action)
{
  action->setFinished(false);
  m_actions.insert(m_actions.begin(), action);
}
