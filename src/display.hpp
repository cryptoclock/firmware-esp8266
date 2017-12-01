#pragma once

#include "config_common.hpp"
#include <Ticker.h>
#include <vector>
#include <memory>

#include "display_action.hpp"

const int MILIS_PER_TICK = 15; // in ms

using std::vector;
using std::shared_ptr;

class Display
{
public:
  Display(bool enabled = true) :
    m_enabled(enabled), m_contrast(0)
  {
    m_last_tick_at = micros();
  }
  virtual ~Display() = 0;

  virtual void displayText(const String& value, int x = 0, int y = 16, bool immediate=true) = 0;
  void displayText(const String& value, Coords coords, bool immediate=true);
  virtual void drawGlyph(uint16_t glyph, Coords coords) = 0;
  virtual void clearBuffer(void) = 0;
  virtual void sendBuffer(void) = 0;
  virtual int getTextWidth(const String& text) = 0;
  virtual void setContrast(uint8_t contrast) = 0;
  virtual void setFont(const uint8_t* font) = 0;
  virtual int getDisplayWidth() = 0;
  virtual int getDisplayHeight() = 0;
  virtual int getCurrentFontHeight() = 0;
  virtual font_t getDefaultFont() = 0;
  Coords centerTextOffset(const String& text);

  void setupTickCallback(Ticker::callback_t callback);
  void tick(void);
  void queueAction(shared_ptr<Action> action);
  void prependAction(shared_ptr<Action> action);
  void replaceAction(shared_ptr<Action> action);
  void removeBottomAction(void);
  void cleanQueue(void);

protected:
  unsigned long m_last_tick_at;
  bool m_enabled;
  uint8_t m_contrast;
  vector<shared_ptr<Action>> m_actions;
  Ticker m_ticker;
};

// namespace Display
