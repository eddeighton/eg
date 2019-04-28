
#ifndef EG_EVENT
#define EG_EVENT

#include "eg_common.hpp"

namespace eg
{
    //an event is effectively a type erased reference
    //it is NOT possible to invoke on an event in c++
    struct Event
    {
        inline Event()
        {
            data.timestamp = INVALID_TIMESTAMP;
        }
        
        template< class T >
        inline Event( const T& from )
        {
            data = from.data;
        }
        
        template< class T >
        inline Event& operator=( const T& from )
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
            if( data.timestamp != INVALID_TIMESTAMP )
            {
                return reinterpret_cast< const void* >( &data );
            }
            else
            {
                return nullptr;
            }
        }
        
        reference data;
    };
}

using Event = eg::Event;

#endif //EG_EVENT