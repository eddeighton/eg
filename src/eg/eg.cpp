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


#include "eg.hpp"

namespace eg
{
    static const OperationIDStringArray g_pszOperationStrings = 
    {
        std::string( "__eg_ImpNoParams"),   //id_Imp_NoParams (-2147483648)
        std::string( "__eg_ImpParams"),     //id_Imp_Params   (-2147483647)
        std::string( "Start"),              //id_Start        (-2147483646)
        std::string( "Stop"),               //id_Stop         (-2147483645)
        std::string( "Pause"),              //id_Pause        (-2147483644)
        std::string( "Resume"),             //id_Resume       (-2147483643)
        std::string( "Wait"),               //id_Wait         (-2147483642)
        std::string( "Get"),                //id_Get          (-2147483641) 
        std::string( "Done"),               //id_Done         (-2147483640)
        std::string( "Range"),              //id_Range        (-2147483639)
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