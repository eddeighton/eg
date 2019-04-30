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
        std::string( "__eg_ImpNoParams"),   //0    id_Imp_NoParams 
        std::string( "__eg_ImpParams"),     //1    id_Imp_Params   
        std::string( "Get"),                //2    id_Get          
        std::string( "Update"),             //3    id_Update       
        std::string( "Old"),                //4    id_Old          
        std::string( "Stop"),               //5    id_Stop         
        std::string( "Pause"),              //6    id_Pause        
        std::string( "Resume"),             //7    id_Resume       
        std::string( "Defer"),              //8    id_Defer        
        std::string( "Size"),               //9    id_Size       
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