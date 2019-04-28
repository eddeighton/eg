
#ifndef EG_COROUTINE
#define EG_COROUTINE

#include "eg_common.hpp"

#include "frame.hpp"

//#include <experimental/coroutine>
//#include <coroutine>

namespace eg
{
    
struct Coroutine
{
    struct promise_type
    {
        Coroutine get_return_object()
        {
            return Coroutine(
                std::experimental::coroutine_handle< promise_type >::from_promise( *this ) );
        }

        auto initial_suspend()  { return std::experimental::suspend_always{}; }
        auto final_suspend()    { return std::experimental::suspend_always{}; }
        void unhandled_exception() {}
        void return_void() {}
    };

    std::experimental::coroutine_handle< promise_type > _coroutine;

    Coroutine() = default;
    Coroutine( Coroutine const& ) = delete;
    Coroutine& operator=( Coroutine const& ) = delete;

    explicit Coroutine( std::experimental::coroutine_handle< promise_type > coroutine )
        : _coroutine( coroutine )
    {
    }

    Coroutine( Coroutine&& other )
        : _coroutine(other._coroutine )
    {
        other._coroutine = nullptr;
    }

    Coroutine& operator=( Coroutine&& other )
    {
        if( &other != this )
        {
            _coroutine = other._coroutine;
            other._coroutine = nullptr;
        }
        return *this;
    }

    ~Coroutine()
    {
        if( _coroutine )
        {
            _coroutine.destroy();
            _coroutine = nullptr;
        }
    }

    void resume()
    {
        if( _coroutine )
            _coroutine.resume();
    }
    
    void destroy()
    {
        if( _coroutine )
        {
            _coroutine.destroy();
            _coroutine = nullptr;
        }
    }

    bool done()
    {
        if( _coroutine )
            return _coroutine.done();
        else
            return true;
    }
};

#define SLEEP co_await std::experimental::suspend_always{}

#define SLEEP_S( seconds )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        const float _t = clock::ct(); \
        auto _s = ( seconds );\
        while( _s > ( clock::ct() - _t ) ) \
        { \
            co_await std::experimental::suspend_always{};  \
        } \
    )
    
    
#define SLEEP_UNTIL( variable )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        bool bFound = false;\
        auto v = ( variable );\
        while( !bFound )\
        {\
            while( Event e = get_next_event() )\
            {\
                if( v == e )\
                {\
                    bFound = true;\
                    break;\
                }\
            }\
            if( !bFound )\
            {\
                SLEEP;\
            }\
        }\
    )
    
    
}

#endif //EG_COROUTINE