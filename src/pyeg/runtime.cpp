
#include "runtime.hpp"

#include "common/assert_verify.hpp"

namespace eg
{


    EGRuntime::EGRuntime( HostFunctionAccessor& hostFunctionAccessor, std::shared_ptr< eg::ReadSession > pDatabase )
        :   m_hostFunctionAccessor( hostFunctionAccessor ),
            m_pDatabase( pDatabase )
    {
    }
    EGRuntime::~EGRuntime()
    {
        for( InvocationMap::const_iterator i = m_invocations.begin(),
            iEnd = m_invocations.end(); i!=iEnd; ++i )
        {
            delete i->second;
        }
    }

    void EGRuntime::getIdentities( std::vector< std::string >& identities )
    {
        const Identifiers::IdentifierMap& identifiers = 
            m_pDatabase->getIdentifiers().getIdentifiersMap();
        for( Identifiers::IdentifierMap::const_iterator 
            i = identifiers.begin(),
            iEnd = identifiers.end(); i!=iEnd; ++i )
        {
            identities.push_back( i->first );
        }
        
        const OperationTypeStringArray& operations = getOperationStrings();
        std::copy( operations.begin(), operations.end(), std::back_inserter( identities ) );
    }
    
    EGTypeID EGRuntime::getTypeID( const char* pszIdentity )
    {
        const Identifiers& identifiers = m_pDatabase->getIdentifiers();
        
        const std::string strIdentity( pszIdentity );
        
        EGTypeID typeID = getOperationName( strIdentity );
        if( HIGHEST_OPERATION_TYPE == typeID )
        {
            if( const abstract::Element* pElement = identifiers.isElement( strIdentity )  )
            {
                typeID = - static_cast< EGTypeID >( pElement->getIndex() );
            }
            else
            {
                typeID = 0;
            }
        }
        
        return typeID;
    }

    __eg_reference EGRuntime::getReference( const __eg_reference& dimension )
    {
        return m_hostFunctionAccessor.getReference( dimension );
    }
    
    PyObject* EGRuntime::evaluate( const __eg_reference& reference, const InvocationSolution* pInvocation, PyObject* pArgs, PyObject* pKwArgs )
    {
        __eg_reference target = pInvocation->evaluate( *this, reference );
        
        pybind11::object result;
        
        switch( pInvocation->getOperation() )
        {
            case eGet    :
                {
                    result = m_hostFunctionAccessor.getRead( target.type )( target.instance );
                }
                break;
            case eUpdate :
                break;
            case eRead   :
            case eOld    :
                {
                    result = m_hostFunctionAccessor.getRead( target.type )( target.instance );
                }
                break;
            case eWrite  :
                {
                    pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pArgs );
                    result = m_hostFunctionAccessor.getWrite( target.type )( target.instance, args );
                }
                break;
            case eStart  :
                {
                    pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pArgs );
                    result = m_hostFunctionAccessor.getStart( target.type )( target.instance, args );
                }
                break;
            case eStop   :
                {
                    result = m_hostFunctionAccessor.getStop( target.type )( target.instance );
                }
                break;
            case ePause  :
                {
                    result = m_hostFunctionAccessor.getPause( target.type )( target.instance );
                }
                break;
            case eResume :
                {
                    result = m_hostFunctionAccessor.getResume( target.type )( target.instance );
                }
                break;
            case eDefer  :
            case eEmpty  :
                {
                    result = m_hostFunctionAccessor.getEmpty( target.type )( target.instance );
                }
                break;
            case eRange  :
                break;  
        }

        if( result )
        {
            pybind11::handle h = result;
            h.inc_ref();
            return h.ptr();
        }
        else
        {
            Py_INCREF( Py_None );
            return Py_None;
        }
    }
    
    PyObject* EGRuntime::invoke( const __eg_reference& reference, const std::vector< EGTypeID >& implicitTypePath, PyObject *args, PyObject *kwargs )
    {
        const IndexedObject::Array& objects = m_pDatabase->getObjects( IndexedObject::MASTER_FILE );
        const DerivationAnalysis& analysis = m_pDatabase->getDerivationAnalysis();
        const Identifiers& identifiers = m_pDatabase->getIdentifiers();
        
        InvocationSolution::Context context;
        {
            ASSERT( reference.type > 0 );
            const abstract::Action* pContextAction = 
                dynamic_cast< const abstract::Action* >( objects[ reference.type ] );
            ASSERT( pContextAction );
            context.push_back( pContextAction );
        }
    
        EGTypeID operationType;
        InvocationSolution::TypePath typePath;
        {
            for( EGTypeID typeID : implicitTypePath )
            {
                ASSERT( typeID < 0 );
                if( isOperationType( typeID ) )
                {
                    operationType = typeID;
                }
                else
                {
                    ASSERT( -typeID < static_cast< int >( objects.size() ) );
                        
                    const abstract::Element* pElement = 
                        dynamic_cast< const abstract::Element* >( objects[ -typeID ] );
                    ASSERT( pElement );
                    if( pElement )
                    {
                        std::vector< const abstract::Element* > elements =
                            identifiers.getGroup( pElement );
                        typePath.push_back( elements );
                    }
                }
            }
        }
        
        InvocationSolution* pInvocation = nullptr;
        {
            const InvocationID invocationID( context, typePath, operationType );
            InvocationMap::const_iterator iFind = m_invocations.find( invocationID );
            if( iFind != m_invocations.end() )
            {
                pInvocation = iFind->second;
            }
            else
            {
                std::vector< const concrete::Inheritance_Node* > contextNodes;
                {
                    for( const abstract::Element* pContext : context )
                    {
                        if( const abstract::Action* pContextAction = dynamic_cast< const abstract::Action* >( pContext ) )
                            analysis.getInheritanceNodes( pContextAction, contextNodes );
                    }
                }
                
                std::vector< std::vector< const concrete::Element* > > concreteTypePath;
                {
                    for( const std::vector< const abstract::Element* >& typePathElement : typePath )
                    {
                        std::vector< const concrete::Element* > instances;
                        for( const abstract::Element* pElement : typePathElement )
                            analysis.getInstances( pElement, instances );
                        concreteTypePath.emplace_back( std::move( instances ) );
                    }
                }
                
                pInvocation = new InvocationSolution( 
                    IndexedObject( InvocationSolution::Type, 
                        InvocationSolution::NO_FILE, 
                        InvocationSolution::NO_INDEX ) );
                        
                DerivationAnalysis::NameResolution nameResolution( analysis );
                nameResolution.resolve( contextNodes, concreteTypePath );
                
                pInvocation->m_operationType        = operationType;
                pInvocation->m_context              = context;
                pInvocation->m_implicitTypePath     = implicitTypePath;
                
                pInvocation->build( analysis, nameResolution );
                
                m_invocations.insert( std::make_pair( invocationID, pInvocation ) );
            }
        }
        ASSERT( pInvocation );
        
        return evaluate( reference, pInvocation, args, kwargs );
    }

}