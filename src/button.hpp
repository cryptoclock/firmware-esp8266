#pragma once
#include "config_common.hpp"
#include <Arduino.h>
#include <Ticker.h>
#include <functional>

typedef std::function<void()> button_callback_t;

class Button {
public:
  Button(uint8_t pin) :
    m_short_press_cb(nullptr), m_long_press_cb(nullptr), m_super_long_press_cb(nullptr), m_pin(pin),
    m_currently_pressed(false), m_pressed_at(0), m_long_press_dispatched(false), m_super_long_press_dispatched(false)
  {
    pinMode(m_pin, INPUT);
  }

  void setupTickCallback(Ticker::callback_t callback);

  void onShortPress(button_callback_t func);
  void onLongPress(button_callback_t func);
  void onSuperLongPress(button_callback_t func);
  void tick(void);
private:
  button_callback_t m_short_press_cb;
  button_callback_t m_long_press_cb;
  button_callback_t m_super_long_press_cb;
  uint8_t m_pin;
  bool m_currently_pressed;
  unsigned long m_pressed_at;
  bool m_long_press_dispatched;
  bool m_super_long_press_dispatched;
  Ticker m_ticker;

  static const unsigned int m_long_press_delay = 1000;
  static const unsigned int m_super_long_press_delay = 5000;
};
