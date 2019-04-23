
#include "eg.hpp"

namespace eg
{

    static const OperationTypeStringArray g_pszOperationStrings = 
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
    
    OperationType getOperationName( const std::string& strName )
    {
        OperationTypeStringArray::const_iterator iFind = 
            std::find( g_pszOperationStrings.begin(), g_pszOperationStrings.end(), strName );
        if( iFind == g_pszOperationStrings.end() )
            return HIGHEST_OPERATION_TYPE;
        else
            return static_cast< OperationType >( 
                std::numeric_limits< EGTypeID >::min() + 
                std::distance( g_pszOperationStrings.begin(), iFind ) );
    }
    
    const OperationTypeStringArray& getOperationStrings()
    {
        return g_pszOperationStrings;
    }
}