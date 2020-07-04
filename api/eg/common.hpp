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
    
    enum ActionState
    {
        action_stopped = 0,
        action_running,
        action_paused,
        TOTAL_ACTION_STATES
    };
    
    inline const char* getActionState( ActionState state )
    {
        switch( state )
        {
            case action_running  : return "::eg::action_running"; 
            case action_paused   : return "::eg::action_paused";  
            case action_stopped  : return "::eg::action_stopped";
            case TOTAL_ACTION_STATES :
            default:               return "";
        }
    }
    
    static const TimeStamp INVALID_TIMESTAMP = 0U;

    using event_iterator = std::uint64_t;

    enum OperationID : TypeID
    {
        id_Imp_NoParams             = std::numeric_limits< TypeID >::min(), //id_Imp_NoParams (-2147483648)
        id_Imp_Params               ,                                       //id_Imp_Params   (-2147483647)
        id_Start                    ,                                       //id_Start        (-2147483646)
        id_Stop                     ,                                       //id_Stop         (-2147483645)
        id_Pause                    ,                                       //id_Pause        (-2147483644)
        id_Resume                   ,                                       //id_Resume       (-2147483643)
        id_Wait                     ,                                       //id_Wait         (-2147483642)
        id_Get                      ,                                       //id_Get          (-2147483641)
        id_Done                     ,                                       //id_Done         (-2147483640)
        id_Range                    ,                                       //id_Range        (-2147483639)
        id_Raw                      ,                                       //id_Raw          (-2147483638)
        HIGHEST_OPERATION_TYPE //HIGHEST_OPERATION_TYPE (-2147483637)
    };
    
    enum ExplicitOperationID : TypeID
    {
        id_exp_Read                     ,
        id_exp_Write                    ,
        id_exp_Call                     ,
        id_exp_Start                    ,
        id_exp_Stop                     ,
        id_exp_Pause                    ,
        id_exp_Resume                   ,
        id_exp_WaitAction               ,
        id_exp_WaitDimension            ,
        id_exp_GetAction                ,
        id_exp_GetDimension             ,
        id_exp_Done                     ,
        id_exp_Range                    ,
        id_exp_Raw                      ,
        HIGHEST_EXPLICIT_OPERATION_TYPE 
    };

    static const TypeID TOTAL_OPERATION_TYPES = HIGHEST_OPERATION_TYPE - std::numeric_limits< TypeID >::min();
    
    inline bool isOperationImplicit( OperationID operationType )
    {
        return ( operationType == id_Imp_NoParams ) || ( operationType == id_Imp_Params );
    }
    
    inline bool isOperationType( TypeID id )
    {
        return id < HIGHEST_OPERATION_TYPE;
    }
    
    inline bool isOperationEnumeration( OperationID id )
    {
        switch( id )
        {
            case id_Range:
            case id_Raw:
                return true;
            default:
                return false;
        }
    }

    enum InvocableID : TypeID
    {
        id_Variant = HIGHEST_OPERATION_TYPE,
        id_TypePath
    };
    
    enum ReferenceState
    {
        Null,
        Invalid,
        Stopped,
        Paused,
        Running
    };
    
    struct TypeInstance
    {
        Instance  instance  = 0U;
        TypeID    type      = 0;
        
        inline bool operator==( const TypeInstance& cmp ) const
        {
            return  ( instance == cmp.instance ) && ( type == cmp.type );
        }
        
        inline bool operator!=( const TypeInstance& cmp ) const
        {
            return !( *this == cmp );
        }
        
        inline bool operator<( const TypeInstance& cmp ) const
        {
            return  ( instance != cmp.instance ) ?      ( instance < cmp.instance ) : 
                    ( type != cmp.type ) ?              ( type < cmp.type ) : 
                    false;
        }
    };

    struct reference : TypeInstance
    {
        TimeStamp timestamp = INVALID_TIMESTAMP;
        
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