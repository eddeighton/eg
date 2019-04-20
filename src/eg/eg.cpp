
#include "eg.hpp"

namespace eg
{

    static std::string g_pszOperationStrings[] =
    {
        std::string( "Get"),        //0     eGet    
        std::string( "Update"),     //1     eUpdate 
        std::string( "Read"),       //2     eRead   
        std::string( "Old"),        //3     eOld    
        std::string( "Write"),      //4     eWrite  
        std::string( "Start"),      //5     eStart  
        std::string( "Stop"),       //6     eStop   
        std::string( "Pause"),      //7     ePause  
        std::string( "Resume"),     //8     eResume 
        std::string( "Defer"),      //9     eDefer  
        std::string( "Empty"),      //10    eEmpty   
        std::string( "Range"),      //11    egRange   
    };
    const std::string& getOperationString( OperationType op )
    {
        return g_pszOperationStrings[ op - std::numeric_limits< EGTypeID >::min() ];
    }
    
    bool isOperationName( const std::string& strName )
    {
        return std::find( g_pszOperationStrings, g_pszOperationStrings + TOTAL_OPERATION_TYPES, strName ) 
            != g_pszOperationStrings + TOTAL_OPERATION_TYPES;
    }
}