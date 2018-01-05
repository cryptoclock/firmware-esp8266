#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
namespace Action {
class StaticText : public ActionT
{
public:
  StaticText(const String& text, const double duration, const Coords& coords = Coords{0,0}, action_callback_t onfinished_cb = nullptr)
    : ActionT(duration, coords, onfinished_cb), m_text(text)
  {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
protected:
  String m_text;
};

class RotatingText : public StaticText
{
public:
  RotatingText(const String& text, const double duration, const int speed, const Coords& coords = Coords{0,0}, action_callback_t onfinished_cb = nullptr)
    : StaticText(text, duration, coords, onfinished_cb), m_speed(speed)
  {}

  void draw(DisplayT *display, Coords coords) override;

private:
  int m_speed;
};

class RotatingTextOnce : public StaticText
{
public:
  RotatingTextOnce(const String& text, const int speed, const Coords& coords = Coords{0,0}, action_callback_t onfinished_cb = nullptr)
    : StaticText(text, -1, coords, onfinished_cb), m_speed(speed)
  {
  }

  void draw(DisplayT *display, Coords coords) override;
  void tick(DisplayT *display, double elapsed_time) override;
private:
  int m_speed;
};

}
}
