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

    Name* NameResolution::add( 
        int iParent, const concrete::Inheritance_Node* pInheritanceNode, int score, bool bIsTarget /*= false*/ )
    {
        nodes.emplace_back( Name( pInheritanceNode, score, bIsTarget ) );
        Name* pNewNode = &nodes.back();
        nodes[ iParent ].children.push_back( nodes.size() - 1U );
        return pNewNode;
    }
    Name* NameResolution::add( 
        int iParent, const concrete::Dimension* pDimension, int score, bool bIsTarget /*= false*/ )
    {
        nodes.emplace_back( Name( pDimension, score, bIsTarget ) );
        Name* pNewNode = &nodes.back();
        nodes[ iParent ].children.push_back( nodes.size() - 1U );
        return pNewNode;
    }
    
    void NameResolution::expandReferences()
    {
        bool bContinue = true;
        while( bContinue )
        {
            bContinue = false;

            const std::size_t szSize = nodes.size();
            for( std::size_t iNodeID = 0U; iNodeID < szSize; ++iNodeID )
            {
                //is node a leaf node?
                Name& node = nodes[ iNodeID ];
                if( node.children.empty() )
                {
                    if( node.pInheritanceNode )
                    {
                        //not a reference
                    }
                    else if( node.pDimension )
                    {
                        if( const concrete::Dimension_User* pUserDim = 
                            dynamic_cast< const concrete::Dimension_User* >( node.pDimension ) )
                        {
                            if( pUserDim->isEGType() )
                            {
                                node.isTarget = true;
                                bContinue = true;
                                
                                for( const abstract::Action* pAction : pUserDim->getActionTypes() )
                                {
                                    DerivationAnalysis::InheritanceNodeMap::const_iterator iLower = 
                                        m_analysis.m_inheritanceMap.lower_bound( pAction );
                                    DerivationAnalysis::InheritanceNodeMap::const_iterator iUpper = 
                                        m_analysis.m_inheritanceMap.upper_bound( pAction );
                                    for( ; iLower != iUpper; ++iLower )
                                    {
                                        add( iNodeID, iLower->second, 0 );
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        THROW_RTE( "Invalid node" );
                    }
                }
            }
        }
    }
    
    void NameResolution::addType( const std::vector< const concrete::Element* >& instances )
    {
        const std::size_t szSize = nodes.size();
        for( std::size_t iNodeID = 0U; iNodeID < szSize; ++iNodeID )
        {
            Name& node = nodes[ iNodeID ];
        
            //is node a leaf node?
            if( node.children.empty() )
            {
                int bestScore = std::numeric_limits< int >::max();
                
                if( !node.pInheritanceNode )
                {
                    THROW_NAMERESOLUTION_EXCEPTION( "Cannot resolve futher element in type path after a dimension. " << m_invocationID );
                }
                
                using ResultPair = std::pair< const concrete::Inheritance_Node*, const concrete::Dimension* >;
                std::map< const concrete::Element*, ResultPair > results;
                for( const concrete::Element* pInstance : instances )
                {
                    //is pInstance within node.pInheritanceNode?
                    const concrete::Dimension* pDimensionResult = nullptr;
                    concrete::Inheritance_Node::Inheritance_Node_SetCst visited;
                    const concrete::Inheritance_Node* pResult = 
                        node.pInheritanceNode->findInstance( pInstance, pDimensionResult, visited );
                    
                    if( pResult || pDimensionResult )
                    {
                        //we have found one...
                        results.insert( std::make_pair( pInstance, std::make_pair( pResult, pDimensionResult ) ) );
                    }
                }
                
                if( !results.empty() )
                {
                    //record the child results
                    for( const std::pair< const concrete::Element*, ResultPair >& result : results )
                    {
                        if( result.second.first )
                            add( iNodeID, result.second.first, 0 );
                        else
                            add( iNodeID, result.second.second, 0 );
                    }
                }
                else
                {
                    //get the non-child results
                    for( const concrete::Element* pInstance : instances )
                    {
                        if( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pInstance ) )
                        {
                            add( iNodeID, pAction->getInheritance(), 1 );
                        }
                        else if( const concrete::Dimension* pDimension = 
                            dynamic_cast< const concrete::Dimension* >( pInstance ) )
                        {
                            add( iNodeID, pDimension, 1 );
                        }
                        else
                        {
                            THROW_RTE( "Unknown type" );
                        }
                    }
                }
            }
        }
    }
    
    void NameResolution::pruneBranches( Name* pNode )
    {
        if( !pNode->children.empty() )
        {
            if( pNode->isTarget )
            {
                pNode->score = std::numeric_limits< int >::max();
                for( int index : pNode->children )
                {
                    Name* pChild = &( nodes[ index ] );
                    pruneBranches( pChild );
                    if( pNode->score > pChild->score )
                        pNode->score = pChild->score;
                }
            }
            else
            {
                //only keep highest scoring subset
                std::vector< int > best;
                pNode->score = std::numeric_limits< int >::max();
                bool bRemovedChild = false;
                for( int index : pNode->children )
                {
                    Name* pChild = &( nodes[ index ] );
                    pruneBranches( pChild );
                    if( pNode->score > pChild->score )
                    {
                        if( !best.empty() )
                        {
                            bRemovedChild = true;
                            best.clear();
                        }
                        best.push_back( index );
                        pNode->score = pChild->score;
                    }
                    else if( pNode->score == pChild->score )
                    {
                        best.push_back( index );
                    }
                    else
                    {
                        bRemovedChild = true;
                    }
                } 
                if( bRemovedChild )
                {
                    pNode->children = best;
                }
            }
        }
    }
    
    void NameResolution::resolve( 
        const std::vector< const concrete::Inheritance_Node* >& contextInstances,
        const std::vector< std::vector< const concrete::Element* > >& typePathInstances )
    {
        nodes.emplace_back( Name( (const concrete::Inheritance_Node*)nullptr, 0, false ) );
        
        for( const concrete::Inheritance_Node* pRoot : contextInstances )
        {
            add( 0, pRoot, 0, true );
        }
        
        for( const std::vector< const concrete::Element* >& instances : typePathInstances )
        {
            expandReferences();
            
            addType( instances );
            
            pruneBranches( &nodes[ 0 ] );
        }
    }
    
    NameResolution::NameResolution( const DerivationAnalysis& analysis, const InvocationSolution::InvocationID& invocationID ) 
        :   m_analysis( analysis ),
            m_invocationID( invocationID )
    {
        std::vector< const concrete::Inheritance_Node* > contextNodes;
        {
            for( const abstract::Element* pContext : std::get< InvocationSolution::Context >( invocationID ) )
            {
                if( const abstract::Action* pContextAction = dynamic_cast< const abstract::Action* >( pContext ) )
                    m_analysis.getInheritanceNodes( pContextAction, contextNodes );
            }
        }
        
        if( contextNodes.empty() )
        {
            THROW_NAMERESOLUTION_EXCEPTION( "no invocation context. " << m_invocationID );
        }
        
        //for range enumerations we want to enumerate all deriving types
        const bool bDerivingPathElements = isOperationEnumeration( std::get< OperationID >( invocationID ) );
        
        std::vector< std::vector< const concrete::Element* > > concreteTypePath;
        {
            for( const std::vector< const abstract::Element* >& typePathElement : std::get< InvocationSolution::TypePath >( invocationID ) )
            {
                std::vector< const concrete::Element* > instances;
                for( const abstract::Element* pElement : typePathElement )
                    m_analysis.getInstances( pElement, instances, bDerivingPathElements );
                concreteTypePath.emplace_back( std::move( instances ) );
            }
        }
        
        resolve( contextNodes, concreteTypePath );
    
    }

}
