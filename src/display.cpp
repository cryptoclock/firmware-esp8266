#include <Arduino.h>
#include "display.hpp"

Display::~Display() {}

void Display::displayText(const String& value, Coords coords, bool immediate)
{
  displayText(value, coords.x, coords.y, immediate);
}

Coords Display::centerTextOffset(const String& text)
{
  return Coords {
    (int) std::max(ceil((getDisplayWidth() - getTextWidth(text)) / 2.0), 0.0),
    - (int) std::max(ceil((getDisplayHeight() - getCurrentFontHeight()) / 2.0), 0.0)
  };
}

void Display::setupTickCallback(Ticker::callback_t callback)
{
    m_ticker.attach(MILIS_PER_TICK / 1000.0,callback);
}

void Display::tick(void)
{
  unsigned long current_time = micros();
  double elapsed_time = (current_time - m_last_tick_at) / 1000000.0;
  m_last_tick_at = micros();

  while(true) {
    if (m_actions.size()<1)
      return;

    auto action = m_actions.at(0);
    action->tick(this, elapsed_time);

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
//  action->tick(this);
//  action->draw(this, Coords{0,0});
  m_actions.push_back(action);
}

void Display::prependAction(shared_ptr<Action> action)
{
  action->setFinished(false);
  action->tick(this, 0.0);
  action->draw(this, Coords{0,0});
  m_actions.insert(m_actions.begin(), action);
}

void Display::replaceAction(shared_ptr<Action> action)
{
  action->setFinished(false);
  action->tick(this, 0.0);
  action->draw(this, Coords{0,0});
  m_actions[0] = action;
}
