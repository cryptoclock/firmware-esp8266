#pragma once
#include "config_common.hpp"

// TODO: namespace
// TODO: transtitions

struct Coords {
  int x,y;
};

class Display;

typedef const uint8_t* font_t;

class Action
{
public:
  Action(double duration, Coords coords, font_t font = nullptr)
    : m_duration(duration), m_ticks(0), m_coords(coords), m_font(font)
  {}

  virtual void tick(Display *display) = 0;
  virtual void draw(Display *display) = 0;
  virtual bool isFinished(void) = 0;
  double elapsedTimeSecs(void);
  int elapsedTimeTicks(void);
  // transition_from, transition_to
  virtual ~Action() = 0;
protected:

  double m_duration;
  unsigned int m_ticks;
  Coords m_coords;
  font_t m_font;
};

inline Action::~Action() {}

class StaticTextAction : public Action
{
public:
  StaticTextAction(const String& text, const Coords& coords, const double duration, font_t font = nullptr)
    : Action(duration, coords, font), m_text(text)
  {}

  void tick(Display *display);
  void draw(Display *display);
  bool isFinished(void);

protected:
  String m_text;
};

class RotatingTextAction : public StaticTextAction
{
public:
  RotatingTextAction(const String& text, const Coords& coords, const int speed, const double duration, font_t font = nullptr)
    : StaticTextAction(text, coords, duration, font), m_speed(speed)
  {}

  void draw(Display *display) override;

private:
  int m_speed;
};

class PriceAction : public Action
{
public:
  PriceAction(Coords coords, font_t font = nullptr)
    : Action(-1, coords, font), m_price(-1), m_last_price(-1)
    {}

  void tick(Display *display);
  void draw(Display *display);
  bool isFinished(void);
  void updatePrice(const int new_price);
private:
  int m_price;
  int m_last_price;
};

class ClockAction : public Action
{
public:
  ClockAction(Coords coords, double duration, font_t font = nullptr)
    : Action(duration, coords, font), m_time("")
    {}
  void tick(Display *display);
  void draw(Display *display);
  bool isFinished(void);
  void updateTime(const String& time);
private:
  String m_time;
};
