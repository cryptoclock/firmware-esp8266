/* 
  Cryptoclock ESP8266
  Copyright (C) 2018 www.cryptoclock.net <info@cryptoclock.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*
  Display base class, subclassed by actual display HW driver class (display_*).
  Contains stack of display Actions, of which top-most is displaying, until
  it finishes. (or until it is prepended if it has infinite duration)
*/

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

  virtual void displayNumber(const int number, const int length=0, const int position=0, const bool zero_fill=false) {}
  virtual void displayText(const String& value, const Coords& coords) {}
  virtual void displayBitmap(const unsigned char *bitmap, const Coords& coords, const int w, const int h) {}
  virtual void displayBitmapP(const unsigned char *bitmap, const Coords& coords, const int w, const int h) {}

  void displayTextHCentered(const String& value, const Coords& coords);

  virtual void fill(const Coords& coords, const int color=1) {}

  virtual void drawGlyph(const uint16_t glyph, const Coords& coords) {}
  virtual void drawLine(const Coords& start, const Coords& end) {}
  virtual void drawPixel(const Coords& coords) {}

  virtual void clearBuffer(void) {}
  virtual void sendBuffer(void) {}
  virtual int getTextWidth(const String& text) { return text.length(); }

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

  virtual void setDrawColor(const uint8_t color) {}
  virtual void setRotation(const bool rotation) {}
  virtual int getDisplayWidth() = 0;
  virtual int getDisplayHeight() = 0;
  virtual int getCurrentFontHeight() { return 1; }
  virtual bool isGraphic(void) = 0;
  bool isNumeric(void) { return !isGraphic(); }
  Coords centerTextOffset(const String& text);

  void setFont(const uint8_t font);
  virtual void useFont() {};
  const uint8_t getFont();

  void setupTickCallback(Ticker::callback_function_t callback);
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
