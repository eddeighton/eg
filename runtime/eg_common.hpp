
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
        id_Imp_NoParams             = std::numeric_limits< TypeID >::min(),//id_Imp_NoParams (-2147483648)
        id_Imp_Params               , //id_Imp_Params   (-2147483647)
        id_Get                      , //id_Get          (-2147483646)
        id_Update                   , //id_Update       (-2147483645)
        id_Old                      , //id_Old          (-2147483644)
        id_Stop                     , //id_Stop         (-2147483643)
        id_Pause                    , //id_Pause        (-2147483642)
        id_Resume                   , //id_Resume       (-2147483641)
        id_Defer                    , //id_Defer        (-2147483640)
        id_Size                     , //id_Size         (-2147483639)
        id_Range                    , //id_Range        (-2147483638)
        HIGHEST_OPERATION_TYPE //HIGHEST_OPERATION_TYPE (-2147483637)
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