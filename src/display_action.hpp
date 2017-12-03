#pragma once
#include "config_common.hpp"
#include <memory>

using std::shared_ptr;


// TODO: namespace
// TODO: transtitions

struct Coords {
  int x,y;

  Coords& operator+=(const Coords& other)
  {
    this->x += other.x;
    this->y += other.y;
    return *this;
  }
  Coords& operator-=(const Coords& other)
  {
    this->x -= other.x;
    this->y -= other.y;
    return *this;
  }

  friend Coords operator+(Coords lhs, const Coords& rhs)
  {
    lhs += rhs;
    return lhs;
  }
  friend Coords operator-(Coords lhs, const Coords& rhs)
  {
    lhs -= rhs;
    return lhs;
  }
};

class Display;

typedef const uint8_t* font_t;

class Action
{
public:
  Action(double duration, Coords coords, font_t font = nullptr)
    : m_duration(duration), m_coords(coords), m_font(font), m_finished(false), m_elapsed_time(0.0)
  {}

  virtual void tick(Display *display, double elapsed_time) = 0;
  virtual void draw(Display *display, Coords coords) = 0;
  bool isFinished(void);
  void setFinished(bool status = true);

  double elapsedTime(void);
  // transition_from, transition_to
  void setCoords(Coords coords);
  virtual ~Action() = 0;
protected:
  double m_duration;
  Coords m_coords;
  font_t m_font;
  bool m_finished;
  double m_elapsed_time;
};

inline Action::~Action() {}

class StaticTextAction : public Action
{
public:
  StaticTextAction(const String& text, const double duration, const Coords& coords = Coords{0,0}, font_t font = nullptr)
    : Action(duration, coords, font), m_text(text)
  {}

  void tick(Display *display, double elapsed_time);
  void draw(Display *display, Coords coords);
protected:
  String m_text;
};

class RotatingTextAction : public StaticTextAction
{
public:
  RotatingTextAction(const String& text, const double duration, const int speed, const Coords& coords = Coords{0,0}, font_t font = nullptr)
    : StaticTextAction(text, duration, coords, font), m_speed(speed)
  {}

  void draw(Display *display, Coords coords) override;

private:
  int m_speed;
};

class PriceAction : public Action
{
public:
  PriceAction(const double animation_speed, const Coords& coords=Coords{0,0}, font_t font = nullptr)
    : Action(-1, coords, font), m_animation_speed(animation_speed), m_price(-1), m_last_price(-1), m_displayed_price(-1.0)
    {}

  void tick(Display *display, double elapsed_time);
  void draw(Display *display, Coords coords);
  void updatePrice(const int new_price);
private:
  double m_animation_speed;
  int m_price;
  int m_last_price;
  double m_displayed_price;
};

class ClockAction : public Action
{
public:
  ClockAction(double duration, const Coords& coords=Coords{0,0}, font_t font = nullptr)
    : Action(duration, coords, font), m_time("")
    {}
  void tick(Display *display, double elapsed_time);
  void draw(Display *display, Coords coords);
  void updateTime(const String& time);
private:
  String m_time;
};

class SlideUpTransitionAction : public Action
{
public:
  SlideUpTransitionAction(shared_ptr<Action> actionA, shared_ptr<Action> actionB, double duration, double speed, const Coords& coords=Coords{0,0})
    : Action(duration, coords), m_actionA(actionA), m_actionB(actionB), m_speed(speed)
    {}

  void tick(Display *display, double elapsed_time);
  void draw(Display *display, Coords coords);
private:

  shared_ptr<Action> m_actionA;
  shared_ptr<Action> m_actionB;
  int m_speed;
};
