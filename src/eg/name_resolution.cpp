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

    Name* NameResolution::add( std::size_t iParent, InvocationSolution::ElementPair element, bool bIsMember, bool bIsReference )
    {
        ASSERT( iParent < m_nodes.size() );
        m_nodes.emplace_back( Name( element, bIsMember, bIsReference ) );
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
                        dynamic_cast< const concrete::Dimension_User* >( node.getConcrete() ) )
                    {
                        if( pUserDim->isEGType() )
                        {
                            node.m_bIsReference = true;
                            bContinue = true;
                            
                            InvocationSolution::ElementPairVector elements =
                                InvocationSolution::getElementVector( m_analysis, pUserDim->getActionTypes(), true );
                            
                            for( const InvocationSolution::ElementPair& element : elements )
                                add( iNodeID, element, false, false );
                        }
                    }
                }
            }
        }
    }
    
    void NameResolution::addType( const InvocationSolution::ElementPairVector& pathElement )
    {
        const std::size_t szSize = m_nodes.size();
        for( std::size_t iNodeID = 0U; iNodeID < szSize; ++iNodeID )
        {
            Name& node = m_nodes[ iNodeID ];
        
            //is node a leaf node?
            if( node.m_children.empty() )
            {
                const concrete::Action* pAction = 
                    dynamic_cast< const concrete::Action* >( node.getConcrete() );
                if( !pAction )
                {
                    THROW_NAMERESOLUTION_EXCEPTION( 
                        "Cannot resolve futher element in type path after a dimension. " << m_invocationID );
                }
                
                std::set< InvocationSolution::ElementPair > results;
                for( const InvocationSolution::ElementPair& element : pathElement )
                {
                    if( pAction->isMember( element.second ) )
                    {
                        results.insert( element );
                    }
                }
                
                if( !results.empty() )
                {
                    for( const InvocationSolution::ElementPair& element : results )
                    {
                        add( iNodeID, element, true, false );
                    }
                }
                else
                {
                    for( const InvocationSolution::ElementPair& element : pathElement )
                    {
                        add( iNodeID, element, false, false );
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
                const InvocationSolution::ElementPairVector& context,
                const InvocationSolution::ElementPairVectorVector& typePath,
                bool bExpandFinalReferences )
    {
        m_nodes.emplace_back( Name( false, true ) );
        
        for( const InvocationSolution::ElementPair& element : context )
        {
            add( 0, element, false, false );
        }
        
        for( const InvocationSolution::ElementPairVector& pathElement : typePath )
        {
            expandReferences();
            
            addType( pathElement );
            
            pruneBranches( &m_nodes[ 0 ] );
        }
        
        if( bExpandFinalReferences )
        {
            expandReferences();
            pruneBranches( &m_nodes[ 0 ] );
        }
    }
    
    NameResolution::NameResolution( const DerivationAnalysis& analysis, 
                const InvocationSolution::InvocationID& invocationID,
                const InvocationSolution::ElementPairVector& context,
                const InvocationSolution::ElementPairVectorVector& typePath  ) 
        :   m_analysis( analysis ),
            m_invocationID( invocationID )
    {
        if( context.empty() )
        {
            THROW_NAMERESOLUTION_EXCEPTION( "no invocation context. " << m_invocationID );
        }
        
        bool bExpandFinalReferences = false;
        switch( std::get< OperationID >( invocationID ) )
        {
            case id_Imp_NoParams          :
            case id_Imp_Params            :
                break;
            case id_Get                   :
                break;
            case id_Stop                  :
            case id_Pause                 :
            case id_Resume                :
            case id_Done                  :
                bExpandFinalReferences = true;
                break;
            case id_Range                 :
                break;
            case HIGHEST_OPERATION_TYPE   :
            default                       :
                THROW_RTE( "Invalid operation type" );
        }
            
        
        resolve( context, typePath, bExpandFinalReferences );
    }

}
