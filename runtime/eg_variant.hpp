
#ifndef EG_VARIANT
#define EG_VARIANT

#include "eg_common.hpp"
#include "eg_event.hpp"
#include "eg_result_type.hpp"

namespace eg
{
    template< typename from, typename to >
    struct is_convertible
    {
        static constexpr const bool value = false;
    };

    template< typename... sequence >
    struct boolean_or
    {
        static constexpr const bool value = ( sequence::value || ... );
    };

    template< typename from, typename... toRest >
    struct is_convertible_many
    {
        static constexpr const bool value = boolean_or< is_convertible< from, toRest >... >::value;
    };

    //everything is convertible to and from an Event
    template< typename to >
    struct is_convertible< Event, to >
    {
        static constexpr const bool value = true;
    };

    template< typename from >
    struct is_convertible< from, Event >
    {
        static constexpr const bool value = true;
    };

    template< typename T >
    struct variant_runtime_type_check_helper
    {
        static inline bool test( const Event& from )
        {
            T check( from );
            return check;
        }
    };

    template< typename... sequence >
    struct variant_runtime_type_check
    {
        static inline bool test( const Event& from )
        {
            return ( variant_runtime_type_check_helper< sequence >::test( from ) || ... );
        }
    };

}

template< typename... Ts >
struct [[clang::eg_type( eg::id_Variant )]] __eg_variant
{
    inline __eg_variant()
    {
        data.timestamp = INVALID_TIMESTAMP;
    }
    
    inline __eg_variant( const Event& from )
    {
        //when convert from event need to check runtime type against all __eg_variant types
        if( ( from.timestamp == INVALID_TIMESTAMP ) || !eg::variant_runtime_type_check< Ts... >::test( from ) )
        {
            data.timestamp = INVALID_TIMESTAMP;
        }
        else 
        {
            data = from.data;
        }
    }
    
    template< class T >
    inline __eg_variant( const T& from )
    {
        static_assert( is_convertible_many< T, Ts... >::value, "Incompatible eg type conversion" );
        data = from.data;
    }
    
    inline __eg_variant& operator=( const Event& from )
    {
        //when convert from event need to check runtime type against all __eg_variant types
        if( ( from.data.timestamp == INVALID_TIMESTAMP ) || !eg::variant_runtime_type_check< Ts... >::test( from ) )
        {
            data.timestamp = INVALID_TIMESTAMP;
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
        static_assert( eg::is_convertible_many< T, Ts... >::value, "Incompatible eg type conversion" );
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
        if( data.timestamp != INVALID_TIMESTAMP )
        {
            return reinterpret_cast< const void* >( &data );
        }
        else
        {
            return nullptr;
        }
    }
    
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_variant< Ts... >, TypePath, Operation >::Type invoke( Args... args );
    
    eg::reference data;
}; 

template< typename... Args >
using Var = __eg_variant< Args... >;

namespace eg
{
    //__eg_variant conversion is reflexive so just reuse is_convertible_many in reverse
    template< typename... varArgs, typename to >
    struct is_convertible< __eg_variant< varArgs... >, to >
    {
        static constexpr const bool value = is_convertible_many< to, varArgs... >::value;
    };
}

#endif //EG_VARIANT