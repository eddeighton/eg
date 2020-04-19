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


#ifndef EG_18_04_2019
#define EG_18_04_2019

#include "eg/common.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <array>

namespace eg
{
    static const char* FILE_EXTENSION = ".eg";
    
    using TypeIDVector        = std::vector< TypeID >;
    using TypeIDVectorVector  = std::vector< TypeIDVector >;
    
    using OperationIDStringArray = std::array< std::string, TOTAL_OPERATION_TYPES >;
    
    OperationID getOperationName( const std::string& strName );
    
    const std::string& getOperationString( OperationID op );
    
    const OperationIDStringArray& getOperationStrings();
}

#endif //EG_18_04_2019
