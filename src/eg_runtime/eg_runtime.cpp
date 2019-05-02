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


#include "eg_runtime/eg_runtime.hpp"

#include "eg/implementation_session.hpp"
#include "eg/derivation.hpp"

#include "common/file.hpp"

#include <tuple>
#include <map>
#include <memory>

namespace eg
{
 
    HostFunctionAccessor::~HostFunctionAccessor()
    {
        
    }
    
    EGRuntime::~EGRuntime()
    {
        
    }
    
    
    class EGRuntimeImpl : public EGRuntime, public RuntimeEvaluator
    {
    public:
        EGRuntimeImpl( HostFunctionAccessor& hostAccessor, const char* pszDatabaseFilePath )
            :   m_hostAccessor( hostAccessor )
        {
            {
                const boost::filesystem::path databaseFilePath = 
                    boost::filesystem::edsCannonicalise(
                        boost::filesystem::absolute( 
                            std::string( pszDatabaseFilePath ) ) );
                if( !boost::filesystem::exists( databaseFilePath ) )
                {
                    THROW_RTE( "Cannot locate file: " << databaseFilePath.string() );
                } 
                m_pDatabase = std::make_shared< eg::ReadSession >( databaseFilePath );
            }
            
        }
        ~EGRuntimeImpl()
        {
            for( InvocationMap::const_iterator i = m_invocations.begin(),
                iEnd = m_invocations.end(); i!=iEnd; ++i )
            {
                delete i->second;
            }
        }
        //RuntimeEvaluator
        virtual reference getReference( const reference& dimension )
        {
            return dimension;
        }
        
        //EGRuntime
        virtual void getIdentities( std::vector< const char* >& identities )
        {
            const Identifiers::IdentifierMap& identifiers = 
                m_pDatabase->getIdentifiers().getIdentifiersMap();
            for( Identifiers::IdentifierMap::const_iterator 
                i = identifiers.begin(),
                iEnd = identifiers.end(); i!=iEnd; ++i )
            {
                identities.push_back( i->first.c_str() );
            }
            
            const OperationIDStringArray& operations = getOperationStrings();
            for( const std::string& strOp : operations )
            {
                identities.push_back( strOp.c_str() );
            }
        }
        
        virtual TypeID getTypeID( const char* pszIdentity )
        {
            const Identifiers& identifiers = m_pDatabase->getIdentifiers();
            
            const std::string strIdentity( pszIdentity );
            
            TypeID typeID = getOperationName( strIdentity );
            if( HIGHEST_OPERATION_TYPE == typeID )
            {
                if( const abstract::Element* pElement = identifiers.isElement( strIdentity )  )
                {
                    typeID = - static_cast< TypeID >( pElement->getIndex() );
                }
                else
                {
                    typeID = 0;
                }
            }
            
            return typeID;
        }
        
        virtual void invoke( const reference& ref, const std::vector< TypeID >& implicitTypePath, bool bHasParameters )
        {
            const IndexedObject::Array& objects = m_pDatabase->getObjects( IndexedObject::MASTER_FILE );
            const DerivationAnalysis& analysis = m_pDatabase->getDerivationAnalysis();
            const Identifiers& identifiers = m_pDatabase->getIdentifiers();
            
            InvocationSolution::Context context;
            {
                ASSERT( ref.type > 0 );
                const concrete::Action* pContextAction = 
                    dynamic_cast< const concrete::Action* >( objects[ ref.type ] );
                ASSERT( pContextAction );
                context.push_back( pContextAction->getAction() );
            }
        
            OperationID operationType = HIGHEST_OPERATION_TYPE;
            InvocationSolution::TypePath typePath;
            {
                for( TypeID typeID : implicitTypePath )
                {
                    ASSERT( typeID < 0 );
                    if( isOperationType( typeID ) )
                    {
                        operationType = static_cast< OperationID >( typeID );
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
            if( operationType == HIGHEST_OPERATION_TYPE )
            {
                //infer the operation type
                if( bHasParameters )
                {
                    operationType = id_Imp_Params;
                }
                else
                {
                    operationType = id_Imp_NoParams;
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
                    
                    //for range enumerations we want to enumerate all deriving types
                    const bool bDerivingPathElements = isOperationEnumeration( operationType );
                    
                    std::vector< std::vector< const concrete::Element* > > concreteTypePath;
                    {
                        for( const std::vector< const abstract::Element* >& typePathElement : typePath )
                        {
                            std::vector< const concrete::Element* > instances;
                            for( const abstract::Element* pElement : typePathElement )
                                analysis.getInstances( pElement, instances, bDerivingPathElements );
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
                    if( !typePath.empty() )
                        pInvocation->m_finalPathTypes   = typePath.back();
                    
                    pInvocation->build( analysis, nameResolution );
                    
                    m_invocations.insert( std::make_pair( invocationID, pInvocation ) );
                }
            }
            ASSERT( pInvocation );
            
            const InvocationSolution::TargetTypes& targets = pInvocation->getTargetTypes();
            
            const reference target = pInvocation->evaluate( *this, ref );
            
            switch( pInvocation->getOperation() )
            {
                case id_Imp_NoParams   :
                case id_Imp_Params  :
                    {
                        if( pInvocation->isImplicitStarter() )
                        {
                            m_hostAccessor.doStart( target );
                        }
                        else if( pInvocation->getOperation() == id_Imp_NoParams )
                        {
                            m_hostAccessor.doRead( target );
                        }
                        else if( pInvocation->getOperation() == id_Imp_Params )
                        {
                            m_hostAccessor.doWrite( target );
                        }
                        else
                        {
                            //error
                        }
                    }
                    break;
                case id_Get    :
                    m_hostAccessor.doRead( target );
                    break;
                case id_Update :
                    break;
                case id_Old    :
                    m_hostAccessor.doRead( target );
                    break;
                case id_Stop   :
                    m_hostAccessor.doStop( target );
                    break;
                case id_Pause  :
                    m_hostAccessor.doPause(target );
                    break;
                case id_Resume :
                    m_hostAccessor.doResume( target );
                    break;
                case id_Defer  :
                    break;
                case id_Size  :
                    m_hostAccessor.doSize( target );
                    break;
                case id_Range  :
                    break;  
            }
        }
        
    private:
        HostFunctionAccessor& m_hostAccessor;
        std::shared_ptr< eg::ReadSession > m_pDatabase;
        
        using InvocationID = std::tuple< InvocationSolution::Context, InvocationSolution::TypePath, TypeID >;
        using InvocationMap = std::map< InvocationID, InvocationSolution* >;
        InvocationMap m_invocations;
    };
    
    
    EGRT_EXPORT EGRuntime* constructRuntime( HostFunctionAccessor& hostAccessor, const char* pszDatabaseFilePath )
    {
        EGRuntime* pRuntime = new EGRuntimeImpl( hostAccessor, pszDatabaseFilePath );
        
        return pRuntime;
    }
    
    
}