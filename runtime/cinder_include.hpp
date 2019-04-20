
#ifndef CINDER_INCLUDE_12_04_2019
#define CINDER_INCLUDE_12_04_2019

#include <optional>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//pull namespaces into global namespace
using namespace ci;
using namespace ci::app;


class Input
{
public:
    static inline std::optional< cinder::app::MouseEvent > getMouseEvent();
    static inline std::optional< cinder::app::KeyEvent > getKeyEvent();
};

struct __eg_input
{
    virtual std::optional< cinder::app::MouseEvent > getMouseEvent() = 0;
    virtual std::optional< cinder::app::KeyEvent > getKeyEvent() = 0;
};
static __eg_input* g_eg_input;

inline std::optional< cinder::app::MouseEvent > Input::getMouseEvent()
{
    return g_eg_input->getMouseEvent();
}
inline std::optional< cinder::app::KeyEvent > Input::getKeyEvent()
{
    return g_eg_input->getKeyEvent();
}

#endif //CINDER_INCLUDE_12_04_2019