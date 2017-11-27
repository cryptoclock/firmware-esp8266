#include <Arduino.h>
#include "display.hpp"

Display::~Display() {}

// void Display::refreshPrice(int currentPrice)
// {
//   DEBUG_SERIAL.printf("[DISPLAY] REFRESHED\n");
//   if (Display::m_enabled) {
//     if (currentPrice == -1 || m_last_price == currentPrice) {
//       DEBUG_SERIAL.printf("[DISPLAY] REPRINTING PRICE\n");
//       displayPrice(String(m_last_price));
//     } else if (m_last_price == -1) {
//       DEBUG_SERIAL.printf("[DISPLAY] INIT PRICE\n");
//       displayPrice(String(currentPrice));
//     } else if (m_last_price < currentPrice) {
//       DEBUG_SERIAL.printf("[DISPLAY] PRICE UP\n");
//       for (int i=m_last_price; i <= currentPrice; i++) {
//         displayPrice(String(i));
//         delay(100);
//       }
//     } else {
//       DEBUG_SERIAL.printf("[DISPLAY] PRICE DOWN\n");
//       for (int i=m_last_price; i >= currentPrice; i--) {
//         displayPrice(String(i));
//         delay(100);
//       }
//     }
//     //m_display->drawUTF8(0, 16, "B"); //""₿");
//   }
//   if (currentPrice != -1)
//     m_last_price = currentPrice;
// }

void Display::setupTickCallback(Ticker::callback_t callback)
{
    m_ticker.attach(0.02f,callback); // 20ms per tick
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

void Display::queueAction(unique_ptr<Action> action)
{
  m_actions.push_back(std::move(action));
}
