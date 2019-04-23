

#ifndef EG_RUNTIME_23_04_2019
#define EG_RUNTIME_23_04_2019

#include "eg_common.hpp"

#include "eg/implementation_session.hpp"
#include "eg/derivation.hpp"

#include <pybind11/embed.h> // everything needed for embedding

#include <tuple>
#include <map>
#include <memory>

namespace eg
{
    struct HostFunctionAccessor
    {
    public:
        virtual ~HostFunctionAccessor(){}
        virtual pybind11::function getFunction( EGTypeID type ) = 0;
    };

    class EGRuntime : public RuntimeEvaluator
    {
    public:
        EGRuntime( HostFunctionAccessor& hostFunctionAccessor, std::shared_ptr< eg::ReadSession > pDatabase );
        ~EGRuntime();
        
        void getIdentities( std::vector< std::string >& identities );
        EGTypeID getTypeID( const char* pszIdentity );
        PyObject* invoke( const __eg_reference& context, const std::vector< EGTypeID >& typePath, PyObject *args, PyObject *kwargs );
        
        //RuntimeEvaluator
        virtual __eg_reference eval( const __eg_reference& dimension );
    private:
        PyObject* evaluate( const __eg_reference& reference, const InvocationSolution* pInvocation, PyObject *args, PyObject *kwargs );
    
        HostFunctionAccessor& m_hostFunctionAccessor;
        std::shared_ptr< eg::ReadSession > m_pDatabase;
        
        using InvocationID = std::tuple< InvocationSolution::Context, InvocationSolution::TypePath, EGTypeID >;
        using InvocationMap = std::map< InvocationID, InvocationSolution* >;
        InvocationMap m_invocations;
    };


}


#endif //EG_RUNTIME_23_04_2019