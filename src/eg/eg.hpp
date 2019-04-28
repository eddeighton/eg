
#ifndef EG_18_04_2019
#define EG_18_04_2019

#include "runtime/eg_common.hpp"

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

    static const TypeID TOTAL_OPERATION_TYPES = HIGHEST_OPERATION_TYPE - std::numeric_limits< TypeID >::min();
    
    using OperationIDStringArray = std::array< std::string, TOTAL_OPERATION_TYPES >;
    
    inline bool isOperationType( TypeID id )
    {
        return id < HIGHEST_OPERATION_TYPE;
    }
    
    OperationID getOperationName( const std::string& strName );
    
    const std::string& getOperationString( OperationID op );
    
    const OperationIDStringArray& getOperationStrings();
}

#endif //EG_18_04_2019
