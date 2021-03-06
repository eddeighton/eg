//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.


#ifndef CINDER_INCLUDE_12_04_2019
#define CINDER_INCLUDE_12_04_2019

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

#include <deque>

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
            
            inline InputEvent(){}
            inline InputEvent( Type type, const cinder::app::MouseEvent& mouseEvent ) : type( type ), mouseEvent( mouseEvent ) {}
            inline InputEvent( Type type, const cinder::app::KeyEvent& keyEvent ) : type( type ), keyEvent( keyEvent ) {}
        };
    }
}

class Input
{
public:
    using Iter = std::deque< cinder::app::InputEvent >::const_iterator;
    static std::pair< Iter, Iter > getEvents();
};

#endif //CINDER_INCLUDE_12_04_2019