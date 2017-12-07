#include "config_common.hpp"
#include <Arduino.h>
#include "display_action.hpp"
#include "display.hpp"

namespace Display {
namespace Action {
class StaticBitmap : public ActionT
{
public:
  StaticBitmap(const unsigned char *data, const int width, const int height,
    const double duration, const Coords& coords = Coords{0,0}, font_t font = nullptr)
    : ActionT(duration, coords, font), m_data(data), m_width(width), m_height(height)
  {}

  void tick(DisplayT *display, double elapsed_time);
  void draw(DisplayT *display, Coords coords);
protected:
  const unsigned char *m_data;
  const int m_width;
  const int m_height;
};

}
}
