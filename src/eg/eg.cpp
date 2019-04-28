
#include "eg.hpp"

namespace eg
{
    static const OperationIDStringArray g_pszOperationStrings = 
    {
        std::string( "__eg_ImpNoParams"),   //0    id_Imp_NoParams 
        std::string( "__eg_ImpParams"),     //1    id_Imp_Params   
        std::string( "Get"),                //2    id_Get          
        std::string( "Update"),             //3    id_Update       
        std::string( "Old"),                //4    id_Old          
        std::string( "Stop"),               //5    id_Stop         
        std::string( "Pause"),              //6    id_Pause        
        std::string( "Resume"),             //7    id_Resume       
        std::string( "Defer"),              //8    id_Defer        
        std::string( "Empty"),              //9    id_Empty        
        std::string( "Range"),              //10   id_Range        
    };
    const std::string& getOperationString( OperationID op )
    {
        return g_pszOperationStrings[ op - std::numeric_limits< TypeID >::min() ];
    }
    
    OperationID getOperationName( const std::string& strName )
    {
        OperationIDStringArray::const_iterator iFind = 
            std::find( g_pszOperationStrings.begin(), g_pszOperationStrings.end(), strName );
        if( iFind == g_pszOperationStrings.end() )
            return HIGHEST_OPERATION_TYPE;
        else
            return static_cast< OperationID >( 
                std::numeric_limits< TypeID >::min() + 
                std::distance( g_pszOperationStrings.begin(), iFind ) );
    }
    
    const OperationIDStringArray& getOperationStrings()
    {
        return g_pszOperationStrings;
    }
}