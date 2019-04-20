
#ifndef CINDER_INCLUDE_12_04_2019
#define CINDER_INCLUDE_12_04_2019

#include <optional>

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

//pull namespaces into global namespace
using namespace ci;
using namespace ci::app;


namespace cinder
{
    namespace app
    {
        struct InputEvent
        {
            enum Type
            {
                eMouseDown,
                eMouseUp,
                eMouseMove,
                eMouseWheel,
                eMouseDrag,
                eKeyDown,
                eKeyUp
            };
            Type type;
            cinder::app::MouseEvent mouseEvent;
            cinder::app::KeyEvent keyEvent;
            
            InputEvent(){}
            InputEvent( Type type, const cinder::app::MouseEvent& mouseEvent ) : type( type ), mouseEvent( mouseEvent ) {}
            InputEvent( Type type, const cinder::app::KeyEvent& keyEvent ) : type( type ), keyEvent( keyEvent ) {}
        };
    }
}

class Input
{
public:
    static inline std::optional< cinder::app::InputEvent > getEvent();
};

struct __eg_input
{
    virtual std::optional< cinder::app::InputEvent > getEvent() = 0;
};
static __eg_input* g_eg_input;

inline std::optional< cinder::app::InputEvent > Input::getEvent()
{
    return g_eg_input->getEvent();
}

#endif //CINDER_INCLUDE_12_04_2019