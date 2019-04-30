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
        
        //for range enumerations we want to enumerate all deriving types
        bool bDerivingPathElements = false;
        if( operationType == id_Range )
        {
            bDerivingPathElements = true;
        }
        
        std::vector< std::vector< const concrete::Element* > > concreteTypePath;
        {
            for( const std::vector< const abstract::Element* >& typePathElement : typePath )
            {
                std::vector< const concrete::Element* > instances;
                for( const abstract::Element* pElement : typePathElement )
                    m_pDerivationAnalysis->getInstances( pElement, instances, bDerivingPathElements );
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