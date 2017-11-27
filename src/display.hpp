#pragma once

#include "config_common.hpp"
#include <Ticker.h>
#include <vector>
#include <memory>

#include "display_action.hpp"

using std::vector;
using std::unique_ptr;

class Display
{
public:
  Display(bool enabled = true) :
    m_enabled(enabled), m_contrast(0)
  {}
  virtual ~Display() = 0;

  virtual void displayText(const String& value, int x = 0, int y = 16) {}
  virtual void setContrast(uint8_t contrast) {}

  void setupTickCallback(Ticker::callback_t callback);
  void tick(void);
  void queueAction(unique_ptr<Action> action);

protected:
  bool m_enabled;
  uint8_t m_contrast;
  vector<unique_ptr<Action>> m_actions;
  Ticker m_ticker;
};

// namespace Display
