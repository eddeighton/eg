
#include "operations_session.hpp"

namespace eg
{


    OperationsSession::OperationsSession( const boost::filesystem::path& treePath,
            const boost::filesystem::path& tuPath, 
            IndexedObject::FileID fileID )
        :   CreatingSession( IndexedFile::FileIDtoPathMap{{ IndexedObject::MASTER_FILE, treePath }}, fileID ),
            m_tuPath( tuPath )
    {
        m_pDerivationAnalysis = one_cst< DerivationAnalysis >( getMaster() );
        m_pIdentifiers = one_cst< Identifiers >( getMaster() );
    }

    void OperationsSession::store() const
    {
        CreatingSession::store( m_tuPath );
    }
    
    std::vector< const abstract::Element* > OperationsSession::fromEGTypeID( TypeID typeID, bool bForceDecl )
    {
        std::vector< const abstract::Element* > result;
        
        const IndexedObject::Array& objects = getObjects( IndexedObject::MASTER_FILE );
        if( !isOperationType( typeID ) )
        {
            if( typeID < 0 )
            {
                if( -typeID < static_cast< int >( objects.size() ) )
                {
                    if( const abstract::Element* pElement = 
                            dynamic_cast< const abstract::Element* >( objects[ -typeID ] ) )
                    {
                        return m_pIdentifiers->getGroup( pElement );
                    }
                }
            }
            else if( bForceDecl )
            {
                if( typeID < static_cast< int >( objects.size() ) )
                {
                    if( const abstract::Element* pElement = 
                        dynamic_cast< const abstract::Element* >( objects[ typeID ] ) )
                    {
                        return m_pIdentifiers->getGroupBack( pElement );
                    }   
                }   
            }
            else if( typeID < static_cast< int >( objects.size() ) )
            {
                if( const abstract::Element* pElement = 
                        dynamic_cast< const abstract::Element* >( objects[ typeID ] ) )
                {
                    result.push_back( pElement );
                }
            }
        }
        return result;
    }
    
    const InvocationSolution* OperationsSession::getInvocation( 
            const InvocationSolution::Context& context, 
            const InvocationSolution::TypePath& typePath, 
            TypeID operationType, const std::vector< eg::TypeID >& implicitTypePath )
    {
        const InvocationID invocationID( context, typePath, operationType );
        InvocationMap::const_iterator iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
            return iFind->second;
        
        //construct new invocation
        InvocationSolution* pInvocation = construct< InvocationSolution >();
        
        std::vector< const concrete::Inheritance_Node* > contextNodes;
        {
            for( const abstract::Element* pContext : context )
            {
                if( const abstract::Action* pContextAction = dynamic_cast< const abstract::Action* >( pContext ) )
                    m_pDerivationAnalysis->getInheritanceNodes( pContextAction, contextNodes );
            }
        }
        
        std::vector< std::vector< const concrete::Element* > > concreteTypePath;
        {
            for( const std::vector< const abstract::Element* >& typePathElement : typePath )
            {
                std::vector< const concrete::Element* > instances;
                for( const abstract::Element* pElement : typePathElement )
                    m_pDerivationAnalysis->getInstances( pElement, instances );
                concreteTypePath.emplace_back( std::move( instances ) );
            }
        }
        
        DerivationAnalysis::NameResolution nameResolution( *m_pDerivationAnalysis );
        nameResolution.resolve( contextNodes, concreteTypePath );
        
        pInvocation->m_operationType = operationType;
        pInvocation->m_context = context;
        pInvocation->m_implicitTypePath = implicitTypePath;
        
        pInvocation->build( *m_pDerivationAnalysis, nameResolution );
        
        m_invocations.insert( std::make_pair( invocationID, pInvocation ) );
                                    
        return pInvocation;
    }
}