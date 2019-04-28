
#ifndef EG_COMMON_22_04_2019
#define EG_COMMON_22_04_2019

#include <cstdint>
#include <limits>

#define DO_STUFF_AND_REQUIRE_SEMI_COLON( stuff ) do{ stuff } while( (void)0,0 )
    
namespace eg
{
    using Instance    = std::uint32_t; //32bit only for now
    using TypeID      = std::int32_t;
    using TimeStamp   = std::uint32_t;
    
    static const TimeStamp INVALID_TIMESTAMP = std::numeric_limits< TimeStamp >::max();

    using event_iterator = std::uint64_t;

    enum OperationID : TypeID
    {
        id_Imp_NoParams             = std::numeric_limits< TypeID >::min(),
        id_Imp_Params               ,
        id_Get                      ,
        id_Update                   ,
        id_Old                      ,
        id_Stop                     ,
        id_Pause                    ,
        id_Resume                   ,
        id_Defer                    ,
        id_Size                     ,
        id_Range                    ,
        HIGHEST_OPERATION_TYPE
    };

    enum InvocableID : TypeID
    {
        id_Variant = HIGHEST_OPERATION_TYPE,
        id_TypePath
    };

    struct reference
    {
        Instance  instance;
        TypeID    type;
        TimeStamp timestamp;
        
        inline bool operator==( const reference& cmp ) const
        {
            return  ( ( timestamp == INVALID_TIMESTAMP ) && ( cmp.timestamp == INVALID_TIMESTAMP ) ) ||
                    ( ( instance == cmp.instance ) &&
                        ( type == cmp.type ) &&
                        ( timestamp == cmp.timestamp ) );
        }
        
        inline bool operator!=( const reference& cmp ) const
        {
            return !( *this == cmp );
        }
        
        inline bool operator<( const reference& cmp ) const
        {
            return  ( instance != cmp.instance ) ?      ( instance < cmp.instance ) : 
                    ( type != cmp.type ) ?              ( type < cmp.type ) : 
                    ( timestamp != cmp.timestamp ) ?    ( timestamp < cmp.timestamp ) : 
                    false;
        }
    };
}

#endif //EG_COMMON_22_04_2019