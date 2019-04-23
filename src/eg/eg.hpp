
#ifndef EG_18_04_2019
#define EG_18_04_2019

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>
#include <array>

namespace eg
{
    static const char* EG_FILE_EXTENSION = ".eg";
    
    using EGTypeID              = std::int32_t;
    using EGTypeIDVector        = std::vector< EGTypeID >;
    using EGTypeIDVectorVector  = std::vector< EGTypeIDVector >;

    enum OperationType : EGTypeID
    {
        eGet                      = std::numeric_limits< EGTypeID >::min(),
        eUpdate                   ,
        eRead                     ,
        eOld                      ,
        eWrite                    ,
        eStart                    ,
        eStop                     ,
        ePause                    ,
        eResume                   ,
        eDefer                    ,
        eEmpty                    ,
        eRange                    ,
        HIGHEST_OPERATION_TYPE
    };
    
    enum OtherType : EGTypeID
    {
        egVariant = HIGHEST_OPERATION_TYPE,
        egTypePath
    };
    
    static const EGTypeID TOTAL_OPERATION_TYPES = HIGHEST_OPERATION_TYPE - std::numeric_limits< EGTypeID >::min();
    
    using OperationTypeStringArray = std::array< std::string, TOTAL_OPERATION_TYPES >;
    
    inline bool isOperationType( EGTypeID id )
    {
        return id < HIGHEST_OPERATION_TYPE;
    }
    
    OperationType getOperationName( const std::string& strName );
    
    const std::string& getOperationString( OperationType op );
    
    const OperationTypeStringArray& getOperationStrings();
}

#endif //EG_18_04_2019
