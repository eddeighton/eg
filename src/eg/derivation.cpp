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



#include "derivation.hpp"

namespace eg
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void DerivationAnalysis::load( Loader& loader )
    {
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const abstract::Element*     pElement   = loader.loadObjectRef< const abstract::Element >();
                const concrete::Element*     pInstance  = loader.loadObjectRef< concrete::Element >();
                m_instanceMap.insert( std::make_pair( pElement, pInstance ) );
            }
        }
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const abstract::Action*             pAction          = loader.loadObjectRef< const abstract::Action >();
                const concrete::Inheritance_Node*   pInheritanceNode = loader.loadObjectRef< const concrete::Inheritance_Node >();
                m_inheritanceMap.insert( std::make_pair( pAction, pInheritanceNode ) );
            }
        }
    }
    void DerivationAnalysis::store( Storer& storer ) const
    {
        {
            const std::size_t szSize = m_instanceMap.size();
            storer.store( szSize );
            for( InstanceMap::const_iterator 
                i = m_instanceMap.begin(),
                iEnd = m_instanceMap.end(); i!=iEnd; ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectRef( i->second );
            }
        }
        
        {
            const std::size_t szSize = m_inheritanceMap.size();
            storer.store( szSize );
            for( InheritanceNodeMap::const_iterator 
                i = m_inheritanceMap.begin(),
                iEnd = m_inheritanceMap.end(); i!=iEnd; ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectRef( i->second );
            }
        }
    }
    
    
            
    DerivationAnalysis::NameResolution::Name* DerivationAnalysis::NameResolution::add( 
        int iParent, const concrete::Inheritance_Node* pInheritanceNode, int score, bool bIsTarget /*= false*/ )
    {
        nodes.emplace_back( Name( pInheritanceNode, score, bIsTarget ) );
        Name* pNewNode = &nodes.back();
        nodes[ iParent ].children.push_back( nodes.size() - 1U );
        return pNewNode;
    }
    DerivationAnalysis::NameResolution::Name* DerivationAnalysis::NameResolution::add( 
        int iParent, const concrete::Dimension* pDimension, int score, bool bIsTarget /*= false*/ )
    {
        nodes.emplace_back( Name( pDimension, score, bIsTarget ) );
        Name* pNewNode = &nodes.back();
        nodes[ iParent ].children.push_back( nodes.size() - 1U );
        return pNewNode;
    }
    
    void DerivationAnalysis::NameResolution::expandReferences()
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
                                        analysis.m_inheritanceMap.lower_bound( pAction );
                                    DerivationAnalysis::InheritanceNodeMap::const_iterator iUpper = 
                                        analysis.m_inheritanceMap.upper_bound( pAction );
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
    
    void DerivationAnalysis::NameResolution::addType( const std::vector< const concrete::Element* >& instances )
    {
        const std::size_t szSize = nodes.size();
        for( std::size_t iNodeID = 0U; iNodeID < szSize; ++iNodeID )
        {
            Name& node = nodes[ iNodeID ];
        
            //is node a leaf node?
            if( node.children.empty() )
            {
                int bestScore = std::numeric_limits< int >::max();
                
                VERIFY_RTE( node.pInheritanceNode );
                
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
    
    void DerivationAnalysis::NameResolution::pruneBranches( Name* pNode )
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
    
    void DerivationAnalysis::NameResolution::resolve( 
        const std::vector< const concrete::Inheritance_Node* >& contextInstances,
        std::vector< std::vector< const concrete::Element* > > typePathInstances )
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
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void InvocationSolution::load( Loader& loader )
    {
        {
            m_pRoot = nullptr;
            std::vector< int > parentIndices;
            std::size_t szSize = 0;
            loader.load( szSize );
            //allocate first
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                DerivationStep* pStep = new DerivationStep;
                m_steps.push_back( pStep );
                if( !m_pRoot ) m_pRoot = pStep;
            }
            
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                DerivationStep* pStep = m_steps[ sz ];
                
                pStep->pInstance = loader.loadObjectRef< concrete::Element >();
                loader.load( pStep->id );
                loader.load( pStep->type );
                loader.load( pStep->domain );
                VERIFY_RTE( pStep->id == sz );
                
                int iIndex = 0;
                loader.load( iIndex );
                if( iIndex != -1 )
                    pStep->pParent = m_steps[ iIndex ];
                else
                    pStep->pParent = nullptr;
                std::size_t szNextSize = 0;
                loader.load( szNextSize );
                for( std::size_t sz2 = 0; sz2 != szNextSize; ++sz2 )
                {
                    int iIndex = 0;
                    loader.load( iIndex );
                    pStep->next.push_back( m_steps[ iIndex ] );
                } 
            }
        }
        
        loader.load( m_operationType );
        loader.loadObjectVector( m_context );
        loader.loadObjectVector( m_targetTypes );
        loader.loadObjectVector( m_finalPathTypes );
        loader.load( m_implicitTypePath );
        
    }
    void InvocationSolution::store( Storer& storer ) const
    {
        {
            const std::size_t szSize = m_steps.size();
            storer.store( szSize );
            for( DerivationStep* pStep : m_steps )
            {
                storer.storeObjectRef( pStep->pInstance );
                storer.store( pStep->id );
                storer.store( pStep->type );
                storer.store( pStep->domain );
                
                if( pStep->pParent )
                    storer.store( pStep->pParent->id );
                else
                    storer.store( -1 );
                storer.store( pStep->next.size() );
                for( DerivationStep* pNext : pStep->next )
                    storer.store( pNext->id );
            }
        }
        
        storer.store( m_operationType );
        storer.storeObjectVector( m_context );
        storer.storeObjectVector( m_targetTypes );
        storer.storeObjectVector( m_finalPathTypes );
        storer.store( m_implicitTypePath );
    }
    
    const concrete::Element* findCommonRoot( const concrete::Element* pLeft, const concrete::Element* pRight )
    {
        if( pLeft == pRight ) return pLeft;
        
        std::list< const concrete::Element* > left, right;
        while( pLeft )
        {
            left.push_front( pLeft );
            pLeft = pLeft->getParent();
        }
        while( pRight )
        {
            right.push_front( pRight );
            pRight = pRight->getParent();
        }
        const concrete::Element* pCommonRoot = nullptr;
        for( std::list< const concrete::Element* >::iterator 
            iLeft = left.begin(),
            iRight = right.begin();
            iLeft != left.end() && iRight != right.end() && *iLeft == *iRight; 
            ++iLeft, ++iRight )
            pCommonRoot = *iLeft;
        return pCommonRoot;
    }
    
    InvocationSolution::DerivationStep* InvocationSolution::buildDerivation( const DerivationAnalysis& analysis,
                    const concrete::Element* pFrom, 
                    const concrete::Element* pTo,
                    DerivationStep* pStep,
                    bool bFinal )
    {
        
        const concrete::Element* pCommon = findCommonRoot( pFrom, pTo );
        
        while( pFrom != pCommon )
        {
            //do not allow parent steps in enumerations
            if( isOperationEnumeration( m_operationType ) )
            {
                addStep( pFrom, pStep, DerivationStep::eFailed, pFrom->getLocalDomainSize() );
                return nullptr;
            }
            
            std::size_t szSize = pFrom->getLocalDomainSize();
            pFrom = pFrom->getParent();
            pStep = addStep( pFrom, pStep, DerivationStep::eParent, szSize );
        }
        
        std::vector< const concrete::Element* > path;
        while( pTo != pCommon )
        {
            path.push_back( pTo );
            pTo = pTo->getParent();
        }
        std::reverse( path.begin(), path.end() );
        for( const concrete::Element* pIter : path )
        {
            if( bFinal && isOperationEnumeration( m_operationType ) )
            {
                pStep = addStep( pIter, pStep, DerivationStep::eEnum, pIter->getLocalDomainSize() );
            }
            else if( pIter->getLocalDomainSize() != 1 )
            {
                addStep( pIter, pStep, DerivationStep::eFailed, pIter->getLocalDomainSize() );
                return nullptr;
            }
            else
            {
                pStep = addStep( pIter, pStep, DerivationStep::eChild, pIter->getLocalDomainSize() );
            }
        }
        
        return pStep;
    }
    
    void InvocationSolution::buildSolutionGraph_recurse( const DerivationAnalysis& analysis, 
        const DerivationAnalysis::NameResolution& resolution,
        const DerivationAnalysis::NameResolution::Name* pLast,
        const DerivationAnalysis::NameResolution::Name& name,
        DerivationStep* pStep )
    {
        if( name.isTarget )
        {
            ASSERT( pStep );
            if( pLast )
            {
                const concrete::Element* pFrom = pLast->getInstance();
                const concrete::Element* pTo   = name.getInstance();
                VERIFY_RTE( pTo && pFrom );
                
                if( pStep = buildDerivation( analysis, pFrom, pTo, pStep, false ) )
                {
                    //explicitly add the dereference steps
                    for( int i : name.children )
                    {
                        const DerivationAnalysis::NameResolution::Name& ref = resolution.nodes[ i ];
                        ASSERT( ref.pInheritanceNode );

                        DerivationStep* pNextStep = 
                            addStep( ref.pInheritanceNode->getRootConcreteAction(), pStep, DerivationStep::eDeReference, 1 );

                        for( int j : ref.children )
                        {
                            buildSolutionGraph_recurse( analysis, resolution, &ref, resolution.nodes[ j ], pNextStep );
                        }
                    }
                }
            }
            else if( pStep )
            {
                pLast = &name;

                DerivationStep* pNextStep = pStep;
                if( !isOperationEnumeration( m_operationType ) )
                {
                    pNextStep =
                        addStep( name.pInheritanceNode->getRootConcreteAction(), pStep, DerivationStep::eParent, 1 );
                }

                for( int i : name.children )
                {
                    buildSolutionGraph_recurse( analysis, resolution, pLast, resolution.nodes[ i ], pNextStep );
                }
            }
        }
        else if( name.children.empty() )
        {
            if( pLast )
            {
                const concrete::Element* pFrom = pLast->getInstance();
                const concrete::Element* pTo   = name.getInstance();
                VERIFY_RTE( pTo && pFrom );
                
                bool bIsStarter = false;
                {
                    if( ( m_operationType == id_Imp_NoParams ) || 
                        ( m_operationType == id_Imp_Params ) )
                    {
                        if( dynamic_cast< const concrete::Action* >( pTo ) )
                            bIsStarter = true;
                        
                    }
                }
                
                //if start operation then we actually want to derive the parent
                if( bIsStarter )
                {
                    const concrete::Element* pFrom = pLast->getInstance();
                    ASSERT( name.pInheritanceNode );
                    const concrete::Element* pTo = name.pInheritanceNode->getRootConcreteAction();
                    const concrete::Element* pParent = pTo->getParent();
                    VERIFY_RTE( pTo && pParent );
                    
                    if( pStep = buildDerivation( analysis, pFrom, pParent, pStep, true ) )
                    {
                        pStep = addStep( pTo, pStep, DerivationStep::eTarget, 1 );
                        m_targetTypes.push_back( pTo->getAbstractElement() );
                    }
                    else
                    {
                        //derivation failed...
                    }
                }
                else
                {
                    //solve derivation from pLast to name
                    if( pStep = buildDerivation( analysis, pFrom, pTo, pStep, true ) )
                    {
                        if( name.pDimension )
                        {
                            pStep = addStep( name.pDimension, pStep, DerivationStep::eTarget, 1 );
                        }
                        else 
                        {
                            ASSERT( name.pInheritanceNode );
                            pStep = addStep( name.pInheritanceNode->getRootConcreteAction(), pStep, DerivationStep::eTarget, 1 );
                        }
                        m_targetTypes.push_back( pStep->pInstance->getAbstractElement() );
                    }
                    else
                    {
                        //derivation failed...
                    }
                }
            }
            else
            {
                THROW_RTE( "This should be unreachable" );
            }
        }
        else
        {
            for( int i : name.children )
            {
                buildSolutionGraph_recurse( analysis, resolution, pLast, resolution.nodes[ i ], pStep );
            } 
        }
    }
    
    void InvocationSolution::propagate_failed( DerivationStep* pStep )
    {
        for( DerivationStep* pChildStep : pStep->next)
        {
            propagate_failed( pChildStep );
        }
        
        {
            std::vector< DerivationStep* > successful;
            for( DerivationStep* pChildStep : pStep->next)
            {
                if( pChildStep->type != DerivationStep::eFailed )
                {
                    successful.push_back( pChildStep );
                }
            }
            pStep->next = successful;
        }
        if( pStep->next.empty() && ( pStep->type != DerivationStep::eTarget ) )
        {
            pStep->type = DerivationStep::eFailed;
        }
    }
    
    void InvocationSolution::build( const DerivationAnalysis& analysis, const DerivationAnalysis::NameResolution& resolution )
    {
        VERIFY_RTE( !resolution.nodes.empty() );
        
        m_pRoot = addStep( nullptr, nullptr, DerivationStep::eRoot, 0 );
            
        const DerivationAnalysis::NameResolution::Name& rootName = resolution.nodes[ 0U ];
        const DerivationAnalysis::NameResolution::Name* pLast = nullptr;
        for( int i : rootName.children )
        {
            buildSolutionGraph_recurse( analysis, resolution, pLast, resolution.nodes[ i ], m_pRoot );
        }
        
        propagate_failed( m_pRoot );
        
        //uniquify the targets
        m_targetTypes = uniquify_without_reorder( m_targetTypes );
        
    }
    
    bool InvocationSolution::isImplicitStarter() const
    {
        for( const abstract::Element* pTarget : m_targetTypes )
        {
            if( !dynamic_cast< const abstract::Action* >( pTarget ) )
            {
                return false;
            }
        }
        return true;
    }
    
    reference InvocationSolution::evaluate( RuntimeEvaluator& evaluator, const reference& context, const DerivationStep* pStep, int& iPriority ) const
    {
        reference next = context;
        
        switch( pStep->type )
        {
            case DerivationStep::eParent      :
                {
                    const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pStep->pInstance );
                    ASSERT( pAction );
                    next = reference{  context.instance / pStep->domain, static_cast< eg::TypeID >( pAction->getIndex() ), 0 };
                    iPriority = 1;
                }
                break;
            case DerivationStep::eTarget      :
            case DerivationStep::eChild       :
                {
                    if( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pStep->pInstance ) )
                    {
                        next = reference{  context.instance, static_cast< eg::TypeID >( pAction->getIndex() ), 0 };
                        iPriority = 1;
                    }
                    else if( const concrete::Dimension_User* pDimension =
                                dynamic_cast< const concrete::Dimension_User* >( pStep->pInstance ) )
                    {
                        next = reference{  context.instance, static_cast< eg::TypeID >( pDimension->getIndex() ), 0 };
                        iPriority = 1;
                    }
                    else
                    {
                        ASSERT( false );
                    }
                }
                break;
            case DerivationStep::eLink        :
            case DerivationStep::eDeReference :
                break;
            case DerivationStep::eRoot        :
                break;
            case DerivationStep::eEnum        :
            case DerivationStep::eFailed      :
                break;
        }
        
        for( const DerivationStep* pChildStep : pStep->next )
        {
            int priority = 0, best = -1;
            reference candidate = evaluate( evaluator, next, pChildStep, priority );
            if( priority == best )
            {
                //ambiguity
            }
            else if( priority > best )
            {
                next = candidate;
                best = priority;
            }
        }
        
        return next;
    }
    
    reference InvocationSolution::evaluate( RuntimeEvaluator& evaluator, const reference& context ) const
    {
        int priority = 0;
        return evaluate( evaluator, context, m_pRoot, priority );
        
    }
    
}