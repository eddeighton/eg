
#ifndef EG_COMMON_22_04_2019
#define EG_COMMON_22_04_2019

#include <cstdint>
#include <limits>

using EGInstance    = std::uint32_t; //32bit only for now
using EGTypeID      = std::int32_t;
using EGTimeStamp   = std::uint32_t;
static const EGTimeStamp EG_INVALID_TIMESTAMP = std::numeric_limits< EGTimeStamp >::max();

using __eg_event_iterator = std::uint64_t;

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

enum EGInvocableTypes
{
    egVariant = egHIGHEST_OPERATION_TYPE,
    egTypePath
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

#endif //EG_COMMON_22_04_2019