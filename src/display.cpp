#include <Arduino.h>
#include "display.hpp"

namespace Display {
DisplayT::~DisplayT() {}

Coords DisplayT::centerTextOffset(const String& text)
{
  return Coords {
    (int) std::max(ceil((getDisplayWidth() - getTextWidth(text)) / 2.0), 0.0),
    - (int) std::max(ceil((getDisplayHeight() - getCurrentFontHeight()) / 2.0), 0.0)
  };
}

void DisplayT::setupTickCallback(Ticker::callback_t callback)
{
    m_ticker.attach(MILIS_PER_TICK / 1000.0,callback);
}

void DisplayT::tick(void)
{
  unsigned long current_time = micros();
  double elapsed_time = (current_time - m_last_tick_at) / 1000000.0;
  m_last_tick_at = micros();

  while(true) {
    if (m_actions.empty())
      return;

    auto action = m_actions.at(0);
    action->tick(this, elapsed_time);

    if (action->isFinished()) {
      m_actions.erase(m_actions.begin());
      continue;
    }
    clearBuffer();
    action->draw(this, Coords{0,0});
    sendBuffer();
    break;
  }
}

void DisplayT::queueAction(shared_ptr<ActionT> action)
{
  action->setFinished(false);
  m_actions.push_back(action);
}

void DisplayT::prependAction(shared_ptr<ActionT> action)
{
  action->setFinished(false);
  action->tick(this, 0.0);
  m_actions.insert(m_actions.begin(), action);
}

void DisplayT::replaceAction(shared_ptr<ActionT> action)
{
  action->setFinished(false);
  action->tick(this, 0.0);
  m_actions[0] = action;
}

void DisplayT::removeBottomAction(void)
{
  if (!m_actions.empty())
    m_actions.pop_back();
}

void DisplayT::removeTopAction(void)
{
  if (!m_actions.empty())
    m_actions.erase(m_actions.begin());
}

void DisplayT::cleanQueue(void)
{
  m_actions.clear();
}

void DisplayT::setFont(const uint8_t font)
{
  m_current_font = font;
};

}
