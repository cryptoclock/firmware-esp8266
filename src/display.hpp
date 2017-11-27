#pragma once

#include "config_common.hpp"
#include <Ticker.h>
#include <vector>
#include <memory>

#include "display_action.hpp"

const int MILIS_PER_TICK = 20; // in ms

using std::vector;
using std::shared_ptr;

class Display
{
public:
  Display(bool enabled = true) :
    m_enabled(enabled), m_contrast(0)
  {}
  virtual ~Display() = 0;

  virtual void displayText(const String& value, int x = 0, int y = 16) = 0;
  virtual int getTextWidth(const String& text) = 0;
  virtual void setContrast(uint8_t contrast) = 0;
  virtual void setFont(const uint8_t* font) = 0;
  virtual int getDisplayWidth() = 0;
  virtual int getDisplayHeight() = 0;
  virtual int getCurrentFontHeight() = 0;

  void setupTickCallback(Ticker::callback_t callback);
  void tick(void);
  void queueAction(shared_ptr<Action> action);
  void prependAction(shared_ptr<Action> action);

protected:
  bool m_enabled;
  uint8_t m_contrast;
  vector<shared_ptr<Action>> m_actions;
  Ticker m_ticker;
};

// namespace Display
