
#ifndef RUNTIME_API_26_04_2019
#define RUNTIME_API_26_04_2019



#ifdef EG_RUNTIME_SHARED_MODULE
    #define EGRT_EXPORT __declspec( dllexport )
#else
    #define EGRT_EXPORT __declspec( dllimport )
#endif

#include "runtime/eg_common.hpp"

#include <vector>

//EG Runtime Interface
namespace eg
{
    
    struct EGRT_EXPORT HostFunctionAccessor
    {
        virtual ~HostFunctionAccessor();
        
        virtual void doRead(    const reference& reference ) = 0;
        virtual void doWrite(   const reference& reference ) = 0;
        virtual void doStart(   const reference& reference ) = 0;
        virtual void doStop(    const reference& reference ) = 0;
        virtual void doPause(   const reference& reference ) = 0;
        virtual void doResume(  const reference& reference ) = 0;
        virtual void doSize(    const reference& reference ) = 0;
        
        virtual reference getReference( const reference& dimension ) = 0;
        
    };

    class EGRT_EXPORT EGRuntime
    {
    public:
        virtual ~EGRuntime();
        virtual void getIdentities( std::vector< const char* >& identities ) = 0;
        virtual TypeID getTypeID( const char* pszIdentity ) = 0;
        virtual void invoke( const reference& context, const std::vector< TypeID >& typePath, bool bHasParameters ) = 0;
    };

    EGRT_EXPORT EGRuntime* constructRuntime( HostFunctionAccessor& hostAccessor, const char* pszDatabaseFilePath );

}


#endif //RUNTIME_API_26_04_2019