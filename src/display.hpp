#pragma once

#include "config_common.hpp"
#include <Ticker.h>
#include <vector>
#include <memory>

#include "display_action.hpp"

using std::vector;
using std::shared_ptr;

namespace Display {
class DisplayT
{
public:
  DisplayT(const int milis_per_tick) :
    m_enabled(true), m_current_font(0), c_milis_per_tick(milis_per_tick), m_brightness(0)
  {
    m_last_tick_at = micros();
  }
  virtual ~DisplayT() = 0;

  virtual void displayNumber(const int number, const int length=0, const int position=0, const bool zero_fill=false) = 0;
  virtual void displayText(const String& value, const Coords& coords) = 0;
  virtual void displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h) =0;
  virtual void displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h) = 0;

  void displayTextHCentered(const String& value, const Coords& coords);

  virtual void fill(const Coords& coords, const int color=1) = 0;

  virtual void drawGlyph(const uint16_t glyph, const Coords& coords) = 0;
  virtual void drawLine(const Coords& start, const Coords& end) = 0;
  virtual void drawPixel(const Coords& coords) = 0;

  virtual void clearBuffer(void) = 0;
  virtual void sendBuffer(void) = 0;
  virtual int getTextWidth(const String& text) = 0;

  void setDisplayBrightness(const uint8_t brightness)
  {
    m_brightness = brightness;
    setBrightness(m_brightness);
  }
  void useBrightness(const uint8_t brightness)
  {
    setBrightness(brightness);
  }
  void resetBrightness() {
    setBrightness(m_brightness);
  }

  virtual void setDrawColor(const uint8_t color) = 0;
  virtual void setRotation(const bool rotation) = 0;
  virtual int getDisplayWidth() = 0;
  virtual int getDisplayHeight() = 0;
  virtual int getCurrentFontHeight() = 0;
  virtual bool isNumeric(void) = 0;
  virtual bool isGraphic(void) = 0;
  Coords centerTextOffset(const String& text);

  void setFont(const uint8_t font);
  virtual void useFont() {};
  const uint8_t getFont();

  void setupTickCallback(Ticker::callback_t callback);
  void tick(void);
  void queueAction(shared_ptr<ActionT> action);
  void prependAction(shared_ptr<ActionT> action);
  void replaceAction(shared_ptr<ActionT> action);
  void removeTopAction(void);
  void removeBottomAction(void);
  void cleanQueue(void);
  shared_ptr<ActionT> getTopAction(void);

protected:
  virtual void setBrightness(const uint8_t brightness) = 0; // 0..255

  unsigned long m_last_tick_at;
  bool m_enabled;
  vector<shared_ptr<ActionT>> m_actions;
  Ticker m_ticker;
  uint8_t m_current_font;
  const int c_milis_per_tick;
  uint8_t m_brightness;
//  uint8_t m_num_fonts;
};
}

using Display::DisplayT;
