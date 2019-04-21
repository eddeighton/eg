
#ifndef EG_STANDARD_STUFF
#define EG_STANDARD_STUFF

#include "frame.hpp"
//#include <experimental/coroutine>
//#include <coroutine>

struct EGCoroutine
{
    struct promise_type
    {
        EGCoroutine get_return_object()
        {
            return EGCoroutine(
                std::experimental::coroutine_handle< promise_type >::from_promise( *this ) );
        }

        auto initial_suspend()  { return std::experimental::suspend_always{}; }
        auto final_suspend()    { return std::experimental::suspend_always{}; }
        void unhandled_exception() {}
        void return_void() {}
    };

    std::experimental::coroutine_handle< promise_type > _coroutine;

    EGCoroutine() = default;
    EGCoroutine( EGCoroutine const& ) = delete;
    EGCoroutine& operator=( EGCoroutine const& ) = delete;

    explicit EGCoroutine( std::experimental::coroutine_handle< promise_type > coroutine )
        : _coroutine( coroutine )
    {
    }

    EGCoroutine( EGCoroutine&& other )
        : _coroutine(other._coroutine )
    {
        other._coroutine = nullptr;
    }

    EGCoroutine& operator=( EGCoroutine&& other )
    {
        if( &other != this )
        {
            _coroutine = other._coroutine;
            other._coroutine = nullptr;
        }
        return *this;
    }

    ~EGCoroutine()
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


#define DO_STUFF_AND_REQUIRE_SEMI_COLON( stuff ) do{ stuff } while( (void)0,0 )
    
#define SLEEP co_await std::experimental::suspend_always{}

#define SLEEP_S( seconds )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        const float _t = clock::ct();                      \
        while( seconds > ( clock::ct() - _t ) )            \
        {                                                  \
            co_await std::experimental::suspend_always{};  \
        }                                                  \
    )
    
    
#define SLEEP_UNTIL( variable )\
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        bool bFound = false;\
        while( !bFound )\
        {\
            while( EGEvent e = get_next_event() )\
            {\
                if( variable == e )\
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


union EGIterator
{
    std::uint64_t data;
    struct
    {
        std::uint32_t head : 31;
        std::uint32_t full : 1;
        std::uint32_t tail : 31;
        std::uint32_t protection : 1;
    };
 
    EGIterator()
        :   data( 0UL ) 
    {
    }
    EGIterator( std::uint64_t value ) 
        :   data( value ) 
    { 
    }
};

struct EGDependencyProvider
{
    virtual void* getBuffer( const char* pszName ) = 0;
    virtual void* getInterface( const char* pszName ) = 0;
};

using EGInstance    = std::uint32_t; //32bit only for now
using EGTypeID      = std::int32_t;
using EGTimeStamp   = std::uint32_t;
static const EGTimeStamp EG_INVALID_TIMESTAMP = std::numeric_limits< EGTimeStamp >::max();

template< class ReferenceType >
class EGReferenceIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
{
public:
    EGInstance instance;
    EGInstance sentinal;
    inline EGReferenceIterator( EGInstance instance, EGInstance sentinal ) : instance( instance ), sentinal( sentinal ) {}
    inline EGReferenceIterator( const EGReferenceIterator& from ) : instance( from.instance ), sentinal( from.sentinal ) {}
    EGReferenceIterator& operator++();
    inline EGReferenceIterator operator++(int) {EGReferenceIterator tmp(*this); operator++(); return tmp;}
    inline bool operator==(const EGReferenceIterator& rhs) const {return instance==rhs.instance;}
    inline bool operator!=(const EGReferenceIterator& rhs) const {return instance!=rhs.instance;}
    const ReferenceType& operator*();
};

template< typename Iterator >
struct EGRange
{
    Iterator _begin, _end;
    EGRange( Iterator _begin, Iterator _end ) : _begin( _begin ), _end( _end ) {}
    Iterator begin() const { return _begin; }
    Iterator end() const { return _end; }
}; 
            
struct clock
{
    static EGTimeStamp cycle();
    static EGTimeStamp subcycle();
    static float ct();
    static float dt();
};


using __eg_event_iterator = std::uint64_t;

struct events
{
    static void put( const char* type, EGTimeStamp timestamp, const void* value, std::size_t size );
};

//default dimension traits 
template< typename T >
struct DimensionTraits
{
    using Read  = const T&;
    using Write = T;
    static const EGInstance Size = sizeof( T );
};

//default object traits
template< typename T >
struct ObjectTraits
{
    using Dependency = void;
    
    using PtrType = T*;
    
    static const EGInstance Size = sizeof( PtrType );
    
    static PtrType Allocate()
    {
        return new T;
    }
    static void Deallocate( PtrType p )
    {
        delete p;
    }
    static void Start( PtrType p )
    {
        //do nothing
    }
    static void Stop( PtrType p )
    {
        //do nothing
    }
    static void Pause( PtrType p )
    {
        //do nothing
    }
    static void Unpause( PtrType p )
    {
        //do nothing
    }
    
    
    //copy, restore, serialise, python bindings....
};


enum EGOperationType : EGTypeID
{
    egGet                      = std::numeric_limits< EGTypeID >::min(),
    egUpdate                   ,
    egRead                     ,
    egOld                      ,
    egWrite                    ,
    egStart                    ,
    egStop                     ,
    egPause                    ,
    egResume                   ,
    egDefer                    ,
    egEmpty                    ,
    egRange                    ,
    egHIGHEST_OPERATION_TYPE
};

struct [[clang::eg_type( egGet    )]] Get;          //egGet    
struct [[clang::eg_type( egUpdate )]] Update;       //egUpdate 
struct [[clang::eg_type( egRead   )]] Read;         //egRead   
struct [[clang::eg_type( egOld    )]] Old;          //egOld    
struct [[clang::eg_type( egWrite  )]] Write;        //egWrite  
struct [[clang::eg_type( egStart  )]] Start;        //egStart  
struct [[clang::eg_type( egStop   )]] Stop;         //egStop   
struct [[clang::eg_type( egPause  )]] Pause;        //egPause  
struct [[clang::eg_type( egResume )]] Resume;       //egResume 
struct [[clang::eg_type( egDefer  )]] Defer;        //egDefer  
struct [[clang::eg_type( egEmpty  )]] Empty;        //egEmpty   
struct [[clang::eg_type( egRange  )]] Range;        //egRange   

template< typename Context, typename TypePath, typename Operation >
struct __eg_invocation{};

template< typename Context, typename TypePath, typename Operation >
struct result_type
{
    using Type = __eg_result_type( __eg_invocation< Context, TypePath, Operation > );
};

struct __eg_reference
{
    EGInstance  instance;
    EGTypeID    type;
    EGTimeStamp timestamp;
    
    inline bool operator==( const __eg_reference& cmp ) const
    {
        return  ( ( timestamp == EG_INVALID_TIMESTAMP ) && ( cmp.timestamp == EG_INVALID_TIMESTAMP ) ) ||
                ( ( instance == cmp.instance ) &&
                    ( type == cmp.type ) &&
                    ( timestamp == cmp.timestamp ) );
    }
    
    inline bool operator!=( const __eg_reference& cmp ) const
    {
        return !( *this == cmp );
    }
    
    inline bool operator<( const __eg_reference& cmp ) const
    {
        return  ( instance != cmp.instance ) ?      ( instance < cmp.instance ) : 
                ( type != cmp.type ) ?              ( type < cmp.type ) : 
                ( timestamp != cmp.timestamp ) ?    ( timestamp < cmp.timestamp ) : 
                false;
    }
};

struct EGEvent
{
    inline EGEvent()
    {
        data.timestamp = EG_INVALID_TIMESTAMP;
    }
    
    template< class T >
    inline EGEvent( const T& from )
    {
        data = from.data;
    }
    
    template< class T >
    inline EGEvent& operator=( const T& from )
    {
        data = from.data;
        return *this;
    }
    
    template< typename TComp >
    inline bool operator==( const TComp& cmp ) const
    {
        return data == cmp.data;
    }
    
    template< typename TComp >
    inline bool operator!=( const TComp& cmp ) const
    {
        return !(data == cmp.data);
    }
    
    template< typename TComp >
    inline bool operator<( const TComp& cmp ) const
    {
        return data < cmp.data;
    }
    
    inline operator const void*() const
    {
        if( data.timestamp != EG_INVALID_TIMESTAMP )
        {
            return reinterpret_cast< const void* >( &data );
        }
        else
        {
            return nullptr;
        }
    }
    
    __eg_reference data;
};

static const EGEvent null;

template< typename from, typename to >
struct eg_is_convertible
{
    static constexpr const bool value = false;
};

//everything is convertible to and from an EGEvent
template< typename to >
struct eg_is_convertible< EGEvent, to >
{
    static constexpr const bool value = true;
};

template< typename from >
struct eg_is_convertible< from, EGEvent >
{
    static constexpr const bool value = true;
};


template< typename... sequence >
struct __boolean_or
{
    static constexpr const bool value = ( sequence::value || ... );
};

template< typename from, typename... toRest >
struct eg_is_convertible_many
{
    static constexpr const bool value = __boolean_or< eg_is_convertible< from, toRest >... >::value;
};

template< typename T >
struct variant_runtime_type_check_helper
{
    static inline bool test( const EGEvent& from )
    {
        T check( from );
        return check;
    }
};

template< typename... sequence >
struct variant_runtime_type_check
{
    static inline bool test( const EGEvent& from )
    {
        return ( variant_runtime_type_check_helper< sequence >::test( from ) || ... );
    }
};

enum EGInvocableTypes
{
    egVariant = egHIGHEST_OPERATION_TYPE,
    egTypePath
};

template< typename... Ts >
struct [[clang::eg_type( egVariant )]] __eg_variant
{
    inline __eg_variant()
    {
        data.timestamp = EG_INVALID_TIMESTAMP;
    }
    
    inline __eg_variant( const EGEvent& from )
    {
        //when convert from event need to check runtime type against all variant types
        if( ( from.timestamp == EG_INVALID_TIMESTAMP ) || !variant_runtime_type_check< Ts... >::test( from ) )
        {
            data.timestamp = EG_INVALID_TIMESTAMP;
        }
        else 
        {
            data = from.data;
        }
    }
    
    template< class T >
    inline __eg_variant( const T& from )
    {
        static_assert( eg_is_convertible_many< T, Ts... >::value, "Incompatible eg type conversion" );
        data = from.data;
    }
    
    inline __eg_variant& operator=( const EGEvent& from )
    {
        //when convert from event need to check runtime type against all variant types
        if( ( from.data.timestamp == EG_INVALID_TIMESTAMP ) || !variant_runtime_type_check< Ts... >::test( from ) )
        {
            data.timestamp = EG_INVALID_TIMESTAMP;
        }
        else 
        {
            data = from.data;
        }
        return *this;
    }
    
    template< class T >
    inline __eg_variant& operator=( const T& from )
    {
        static_assert( eg_is_convertible_many< T, Ts... >::value, "Incompatible eg type conversion" );
        data = from.data;
        return *this;
    }
    
    template< typename TComp >
    inline bool operator==( const TComp& cmp ) const
    {
        return data == cmp.data;
    }
    
    template< typename TComp >
    inline bool operator!=( const TComp& cmp ) const
    {
        return !(data == cmp.data);
    }
    
    template< typename TComp >
    inline bool operator<( const TComp& cmp ) const
    {
        return data < cmp.data;
    }
    
    inline operator const void*() const
    {
        if( data.timestamp != EG_INVALID_TIMESTAMP )
        {
            return reinterpret_cast< const void* >( &data );
        }
        else
        {
            return nullptr;
        }
    }
    
    template< typename TypePath, typename Operation, typename... Args >
    typename result_type< __eg_variant< Ts... >, TypePath, Operation >::Type invoke( Args... args );
    
    __eg_reference data;
}; 
 
 
template< typename... Args >
using var = __eg_variant< Args... >;

//variant conversion is reflexive so just reuse eg_is_convertible_many in reverse
template< typename... varArgs, typename to >
struct eg_is_convertible< var< varArgs... >, to >
{
    static constexpr const bool value = eg_is_convertible_many< to, varArgs... >::value;
};



template< typename... Ts >
struct [[clang::eg_type( egTypePath )]] __eg_type_path{};

//https://stackoverflow.com/questions/19838965/how-can-i-use-variadic-templates-to-flatten-a-tree-of-types

// first parameter - accumulator
// second parameter - input __eg_type_path
template <class T, class U>
struct __eg_CanonicaliseTypePath_impl;

// first case - the head of the __eg_type_path is __eg_type_path too
// expand this __eg_type_path and continue
template <class... Ts, class... Heads, class... Tail>
struct __eg_CanonicaliseTypePath_impl<__eg_type_path<Ts...>, __eg_type_path<__eg_type_path<Heads...>, Tail...>> 
{
    using Type = typename __eg_CanonicaliseTypePath_impl<__eg_type_path<Ts...>, __eg_type_path<Heads..., Tail...>>::Type;
};

// second case - the head of the __eg_type_path is not a __eg_type_path
// append it to our new, flattened __eg_type_path
template <class... Ts, class Head, class... Tail>
struct __eg_CanonicaliseTypePath_impl<__eg_type_path<Ts...>, __eg_type_path<Head, Tail...>> 
{
    using Type = typename __eg_CanonicaliseTypePath_impl<__eg_type_path<Ts..., Head>, __eg_type_path<Tail...>>::Type;
};

// base case - input __eg_type_path is empty
// return our flattened __eg_type_path
template <class... Ts>
struct __eg_CanonicaliseTypePath_impl<__eg_type_path<Ts...>, __eg_type_path<>> 
{
    using Type = __eg_type_path<Ts...>;
};

template< typename T >
struct __eg_CanonicaliseTypePath
{
    using Type = __eg_type_path< T >;
};

template< typename... Ts >
struct __eg_CanonicaliseTypePath< __eg_type_path< Ts... > >
{
    using Type = typename __eg_CanonicaliseTypePath_impl< __eg_type_path<>, __eg_type_path< Ts... > >::Type;
};





#endif //EG_STANDARD_STUFF