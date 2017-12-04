#pragma once
#include "config_common.hpp"
#include <memory>
#include "coords.hpp"

using std::shared_ptr;

// TODO: transtitions

namespace Display {
  class DisplayT;

  typedef const uint8_t* font_t;

  class ActionT
  {
  public:
    ActionT(double duration, Coords coords, font_t font = nullptr)
      : m_duration(duration), m_coords(coords), m_font(font), m_finished(false), m_elapsed_time(0.0)
    {}

    virtual void tick(DisplayT *display, double elapsed_time) = 0;
    virtual void draw(DisplayT *display, Coords coords) = 0;
    bool isFinished(void);
    void setFinished(bool status = true);

    double elapsedTime(void);
    // transition_from, transition_to
    void setCoords(Coords coords);
    virtual ~ActionT() = 0;
  protected:
    double m_duration;
    Coords m_coords;
    font_t m_font;
    bool m_finished;
    double m_elapsed_time;
  };

  inline ActionT::~ActionT() {}

  namespace Action {
    class SlideTransition : public ActionT
    {
    public:
      SlideTransition(shared_ptr<ActionT> actionA, shared_ptr<ActionT> actionB, double duration, const Coords& coords=Coords{0,0})
        : ActionT(duration, coords), m_actionA(actionA), m_actionB(actionB)
        {}

      void tick(DisplayT *display, double elapsed_time);
      void draw(DisplayT *display, Coords coords);
    private:

      shared_ptr<ActionT> m_actionA;
      shared_ptr<ActionT> m_actionB;
    };
  }
}
