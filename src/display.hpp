#pragma once

#include "config_common.hpp"
#include <Ticker.h>
#include <vector>
#include <memory>

#include "display_action.hpp"

const int MILIS_PER_TICK = 33; // in ms

using std::vector;
using std::shared_ptr;

namespace Display {
class DisplayT
{
public:
  DisplayT(bool enabled = true) :
    m_enabled(enabled), m_contrast(0)
  {
    m_last_tick_at = micros();
  }
  virtual ~DisplayT() = 0;

  virtual void displayNumber(const int number, const int length=0, const int position=0, const bool zero_fill=false) = 0;
  virtual void displayText(const String& value, const Coords& coords) = 0;
  virtual void displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h) =0;
  virtual void displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h) = 0;

  virtual void fill(const Coords& coords) = 0;

  virtual void drawGlyph(const uint16_t glyph, const Coords& coords) = 0;
  virtual void drawLine(const Coords& start, const Coords& end) = 0;
  virtual void drawPixel(const Coords& coords) = 0;

  virtual void clearBuffer(void) = 0;
  virtual void sendBuffer(void) = 0;
  virtual int getTextWidth(const String& text) = 0;
  virtual void setBrightness(const uint8_t brightness) = 0; // 0..255
  virtual void setFont(const uint8_t* font) = 0;
  virtual void setDrawColor(const uint8_t color) = 0;
  virtual int getDisplayWidth() = 0;
  virtual int getDisplayHeight() = 0;
  virtual int getCurrentFontHeight() = 0;
  virtual font_t getDefaultFont() = 0;
  virtual bool isNumeric(void) = 0;
  virtual bool isGraphic(void) = 0;
  Coords centerTextOffset(const String& text);

  void setupTickCallback(Ticker::callback_t callback);
  void tick(void);
  void queueAction(shared_ptr<ActionT> action);
  void prependAction(shared_ptr<ActionT> action);
  void replaceAction(shared_ptr<ActionT> action);
  void removeBottomAction(void);
  void cleanQueue(void);

protected:
  unsigned long m_last_tick_at;
  bool m_enabled;
  uint8_t m_contrast;
  vector<shared_ptr<ActionT>> m_actions;
  Ticker m_ticker;
};
}

using Display::DisplayT;
