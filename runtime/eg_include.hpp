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

#define LOG( msg )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream os;\
        os << msg;\
        std::string str = os.str();\
        events::put( "log", clock::subcycle(), str.data(), str.size() + 1 );\
        )
        
#define ERROR( msg )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        events::put( "error", clock::subcycle(), msg, strlen( msg ) + 1 );\
        )
        
#define TEST( expr )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::string str = #expr;\
        if( !( expr ) )\
        {\
            events::put( "fail", clock::subcycle(), str.data(), str.size() + 1 );\
        }\
        else\
        {\
            events::put( "pass", clock::subcycle(), str.data(), str.size() + 1 );\
        }\
    )
        
#define TEST_MSG( expr, msg )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream os;\
        os << #expr << " " << msg;\
        std::string str = os.str();\
        if( !( expr ) )\
        {\
            events::put( "fail", clock::subcycle(), str.data(), str.size() + 1 );\
        }\
        else\
        {\
            events::put( "pass", clock::subcycle(), str.data(), str.size() + 1 );\
        }\
    )
    
#endif //EG_DEPENDENCIES_INCLUDE_24_04_2019