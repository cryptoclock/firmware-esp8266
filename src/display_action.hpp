#pragma once
#include "config_common.hpp"

// TODO: namespace

const int MILIS_PER_TICK = 20; // FIXME: to Display

struct Coords {
  int x,y;
};

class Display;

class Action
{
public:
  virtual void tick(Display *display) = 0;
  virtual void draw(Display *display) = 0;
  virtual bool isFinished(void) = 0;
  // transition_from, transition_to
  virtual ~Action() = 0;
private:
};

inline Action::~Action() {}

class StaticTextAction : public Action
{
public:
  StaticTextAction(const String& text, const Coords& coords, const double duration);

  void tick(Display *display);
  void draw(Display *display);
  bool isFinished(void);

private:
  String m_text;
  Coords m_coords;
  double m_duration;
  unsigned int m_ticks;
};
