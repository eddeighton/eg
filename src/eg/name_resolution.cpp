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


#include "name_resolution.hpp"

namespace eg
{

    Name* NameResolution::add( std::size_t iParent, const concrete::Element* pElement, bool bIsMember, bool bIsReference )
    {
        ASSERT( iParent < m_nodes.size() );
        m_nodes.emplace_back( Name( pElement, bIsMember, bIsReference ) );
        Name* pNewNode = &m_nodes.back();
        m_nodes[ iParent ].m_children.push_back( m_nodes.size() - 1U );
        return pNewNode;
    }
    
    void NameResolution::expandReferences()
    {
        bool bContinue = true;
        while( bContinue )
        {
            bContinue = false;

            const std::size_t szSize = m_nodes.size();
            for( std::size_t iNodeID = 0U; iNodeID < szSize; ++iNodeID )
            {
                //is node a leaf node?
                Name& node = m_nodes[ iNodeID ];
                if( node.getChildren().empty() )
                {
                    if( const concrete::Dimension_User* pUserDim = 
                        dynamic_cast< const concrete::Dimension_User* >( node.getElement() ) )
                    {
                        if( pUserDim->isEGType() )
                        {
                            node.m_bIsReference = true;
                            bContinue = true;
                            
                            for( const interface::Action* pAction : pUserDim->getActionTypes() )
                            {
                                std::vector< const concrete::Element* > instances;
                                m_analysis.getInstances( pAction, instances, true );
                                
                                for( const concrete::Element* pElement : instances )
                                {
                                    add( iNodeID, pElement, false, false );
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    void NameResolution::addType( const std::vector< const concrete::Element* >& instances )
    {
        const std::size_t szSize = m_nodes.size();
        for( std::size_t iNodeID = 0U; iNodeID < szSize; ++iNodeID )
        {
            Name& node = m_nodes[ iNodeID ];
        
            //is node a leaf node?
            if( node.m_children.empty() )
            {
                const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( node.getElement() );
                if( !pAction )
                {
                    THROW_NAMERESOLUTION_EXCEPTION( 
                        "Cannot resolve futher element in type path after a dimension. " << m_invocationID );
                }
                
                std::set< const concrete::Element* > results;
                for( const concrete::Element* pInstance : instances )
                {
                    if( pAction->isMember( pInstance ) )
                    {
                        results.insert( pInstance );
                    }
                }
                
                if( !results.empty() )
                {
                    for( const concrete::Element* pResult : results )
                    {
                        add( iNodeID, pResult, true, false );
                    }
                }
                else
                {
                    for( const concrete::Element* pInstance : instances )
                    {
                        add( iNodeID, pInstance, false, false );
                    }
                }
            }
        }
    }
    
    void NameResolution::pruneBranches( Name* pNode )
    {
        if( !pNode->m_children.empty() )
        {
            if( pNode->m_bIsReference )
            {
                if( !pNode->m_bIsMember )
                {
                    for( int index : pNode->getChildren() )
                    {
                        Name* pChild = &( m_nodes[ index ] );
                        pruneBranches( pChild );
                        if( pChild->m_bIsMember )
                        {
                            pNode->m_bIsMember = true;
                        }
                    }
                }
            }
            else
            {
                std::vector< std::size_t > best;
                bool bRemovedChild = false;
                for( std::size_t index : pNode->getChildren() )
                {
                    Name* pChild = &( m_nodes[ index ] );
                    pruneBranches( pChild );
                    if( pChild->m_bIsMember )
                        best.push_back( index );
                    else
                        bRemovedChild = true;
                }
                if( !best.empty() )
                {
                    pNode->m_bIsMember = true;
                    if( bRemovedChild )
                        pNode->m_children = best;
                }
            }
        }
    }
    
    void NameResolution::resolve( 
        const std::vector< const concrete::Element* >& contextInstances,
        const std::vector< std::vector< const concrete::Element* > >& typePathInstances )
    {
        m_nodes.emplace_back( Name( (const concrete::Element*)nullptr, false, true ) );
        
        for( const concrete::Element* pRoot : contextInstances )
        {
            add( 0, pRoot, false, false );
        }
        
        for( const std::vector< const concrete::Element* >& instances : typePathInstances )
        {
            expandReferences();
            
            addType( instances );
            
            pruneBranches( &m_nodes[ 0 ] );
        }
    }
    
    NameResolution::NameResolution( const DerivationAnalysis& analysis, 
                const InvocationSolution::InvocationID& invocationID,
                std::vector< const concrete::Element* >& contextElements,
                std::vector< std::vector< const concrete::Element* > >& concreteTypePath  ) 
        :   m_analysis( analysis ),
            m_invocationID( invocationID )
    {
        if( contextElements.empty() )
        {
            THROW_NAMERESOLUTION_EXCEPTION( "no invocation context. " << m_invocationID );
        }
        
        resolve( contextElements, concreteTypePath );
    }

}
