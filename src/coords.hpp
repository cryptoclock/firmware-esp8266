#include "config_common.hpp"

namespace Display {
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
}
