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

#ifndef EG_DEPENDENCIES_INCLUDE_24_04_2019
#define EG_DEPENDENCIES_INCLUDE_24_04_2019

#include "eg_common.hpp"

namespace eg
{

    struct DependencyProvider
    {
        virtual void* getBuffer( const char* pszName ) = 0;
        virtual void* getInterface( const char* pszName ) = 0;
    };

    // the eg clock
    struct _clock
    {
        virtual TimeStamp cycle()       const = 0;
        virtual TimeStamp subcycle()    const = 0;
        virtual float ct()              const = 0;
        virtual float dt()              const = 0;
    };

    // the eg event log
    struct _event
    {
        const char* type;
        TimeStamp timestamp;
        const void* value;
        std::size_t size;
    };
    struct _event_log
    {
        virtual event_iterator GetEventIterator() = 0;
        virtual bool GetEvent( event_iterator& iterator, _event& event ) = 0;
        virtual void PutEvent( const _event& event ) = 0;
    };
}

struct clock
{
    static eg::TimeStamp cycle();
    static eg::TimeStamp subcycle();
    static float ct();
    static float dt();
};

struct events
{
    static void put( const char* type, eg::TimeStamp timestamp, const void* value, std::size_t size );
};

#define LOG( __msg )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream os;\
        os << __msg;\
        std::string __str = os.str();\
        events::put( "log", clock::subcycle(), __str.data(), __str.size() + 1 );\
        )
        
#define ERROR( __msg )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        events::put( "error", clock::subcycle(), __msg, strlen( __msg ) + 1 );\
        )
        
#define TEST( __expr )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::string __str = #__expr;\
        if( !( __expr ) )\
        {\
            events::put( "fail", clock::subcycle(), __str.data(), __str.size() + 1 );\
        }\
        else\
        {\
            events::put( "pass", clock::subcycle(), __str.data(), __str.size() + 1 );\
        }\
    )
        
#define TEST_MSG( __expr, __msg )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream os;\
        os << #__expr << " " << __msg;\
        std::string __str = os.str();\
        if( !( __expr ) )\
        {\
            events::put( "fail", clock::subcycle(), __str.data(), __str.size() + 1 );\
        }\
        else\
        {\
            events::put( "pass", clock::subcycle(), __str.data(), __str.size() + 1 );\
        }\
    )
    
#endif //EG_DEPENDENCIES_INCLUDE_24_04_2019