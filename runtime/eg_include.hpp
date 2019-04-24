#ifndef EG_DEPENDENCIES_INCLUDE_24_04_2019
#define EG_DEPENDENCIES_INCLUDE_24_04_2019

#include "eg_common.hpp"

// the eg clock
struct __eg_clock
{
    virtual EGTimeStamp cycle()     const = 0;
    virtual EGTimeStamp subcycle()  const = 0;
    virtual float ct()              const = 0;
    virtual float dt()              const = 0;
};

struct clock
{
    static EGTimeStamp cycle()    ;//  { return g_eg_clock->cycle(); }
    static EGTimeStamp subcycle() ;//  { return g_eg_clock->subcycle(); }
    static float ct()             ;//  { return g_eg_clock->ct(); }
    static float dt()             ;//  { return g_eg_clock->dt(); }
};

// the eg event log
struct __eg_event
{
    const char* type;
    EGTimeStamp timestamp;
    const void* value;
    std::size_t size;
};
struct __eg_event_log
{
    virtual __eg_event_iterator GetEventIterator() = 0;
    virtual bool GetEvent( __eg_event_iterator& iterator, __eg_event& event ) = 0;
    virtual void PutEvent( const __eg_event& event ) = 0;
};

struct events
{
    static void put( const char* type, EGTimeStamp timestamp, const void* value, std::size_t size );
};

#endif //EG_DEPENDENCIES_INCLUDE_24_04_2019