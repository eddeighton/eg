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



#include "invocation.hpp"

#include "identifiers.hpp"
#include "name_resolution.hpp"

namespace eg
{

    std::vector< const abstract::Element* > fromEGTypeID( const IndexedObject::Array& objects, 
        const Identifiers& identifiers, TypeID typeID, bool bForceDecl )
    {
        std::vector< const abstract::Element* > result;
        
        if( !isOperationType( typeID ) )
        {
            if( typeID < 0 )
            {
                if( -typeID < static_cast< int >( objects.size() ) )
                {
                    if( const abstract::Element* pElement = 
                            dynamic_cast< const abstract::Element* >( objects[ -typeID ] ) )
                    {
                        return identifiers.getGroup( pElement );
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
                        return identifiers.getGroupBack( pElement );
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
    
    InvocationSolution::InvocationID InvocationSolution::invocationIDFromTypeIDs( 
        const IndexedObject::Array& objects, const Identifiers& identifiers, 
        const std::vector< TypeID >& contextTypes, 
        const std::vector< TypeID >& implicitTypePath, OperationID operationType )
    {
        
        std::vector< const abstract::Element* > context;
        {
            for( TypeID typeID : contextTypes )
            {
                std::vector< const abstract::Element* > result =
                    fromEGTypeID( objects, identifiers, typeID, false );
                std::copy( result.begin(), result.end(), std::back_inserter( context ) );
            }
            context = uniquify_without_reorder( context );
        }
        
        
        std::vector< std::vector< const abstract::Element* > > typePath;
        {
            for( TypeID typeID : implicitTypePath )
            {
                std::vector< const abstract::Element* > result =
                    fromEGTypeID( objects, identifiers, typeID, true );
                if( !result.empty() )
                    typePath.push_back( result );
            }
        }
        
        return InvocationSolution::InvocationID( context, typePath, operationType );
    }
    
    InvocationSolution::InvocationID InvocationSolution::invocationIDFromTypeIDs( 
        const IndexedObject::Array& objects, const Identifiers& identifiers, 
        const TypeID runtimeContextType, 
        const std::vector< TypeID >& implicitTypePath, bool bHasParameters )
    {        
        InvocationSolution::Context context;
        {
            ASSERT( runtimeContextType > 0 );
            const concrete::Action* pContextAction = 
                dynamic_cast< const concrete::Action* >( objects[ runtimeContextType ] );
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
        
        return InvocationSolution::InvocationID( context, typePath, operationType );
    }
    
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
        const NameResolution& resolution,
        const Name* pLast,
        const Name& name,
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
                        const Name& ref = resolution.getNodes()[ i ];
                        ASSERT( ref.pInheritanceNode );

                        DerivationStep* pNextStep = 
                            addStep( ref.pInheritanceNode->getRootConcreteAction(), pStep, DerivationStep::eDeReference, 1 );

                        for( int j : ref.children )
                        {
                            buildSolutionGraph_recurse( analysis, resolution, &ref, resolution.getNodes()[ j ], pNextStep );
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
                    buildSolutionGraph_recurse( analysis, resolution, pLast, resolution.getNodes()[ i ], pNextStep );
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
                buildSolutionGraph_recurse( analysis, resolution, pLast, resolution.getNodes()[ i ], pStep );
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
    
    void InvocationSolution::build( const DerivationAnalysis& analysis, const NameResolution& resolution )
    {
        VERIFY_RTE( !resolution.getNodes().empty() );
        
        m_pRoot = addStep( nullptr, nullptr, DerivationStep::eRoot, 0 );
            
        const Name& rootName = resolution.getNodes()[ 0U ];
        const Name* pLast = nullptr;
        for( int i : rootName.children )
        {
            buildSolutionGraph_recurse( analysis, resolution, pLast, resolution.getNodes()[ i ], m_pRoot );
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
    
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    InvocationSolution* InvocationSolutionMap::constructInvocation( 
        const InvocationSolution::InvocationID& invocationID, const std::vector< TypeID >& implicitTypePath )
    {
        //construct new invocation
        InvocationSolution* pInvocation = m_session.construct< InvocationSolution >();
        pInvocation->m_operationType    = std::get< OperationID >( invocationID );
        pInvocation->m_context          = std::get< InvocationSolution::Context >( invocationID );
        pInvocation->m_implicitTypePath = implicitTypePath;
        if( !std::get< InvocationSolution::TypePath >( invocationID ).empty() )
            pInvocation->m_finalPathTypes   = std::get< InvocationSolution::TypePath >( invocationID ).back();
        
        return pInvocation;
    }
    
    const InvocationSolution* InvocationSolutionMap::getInvocation( 
        const InvocationSolution::InvocationID& invocationID, 
        const std::vector< TypeID >& implicitTypePath )
    {
        InvocationSolution::InvocationMap::const_iterator
            iFind = m_invocations.find( invocationID );
        if( iFind != m_invocations.end() )
        {
            return iFind->second;
        }
        
        NameResolution nameResolution( m_analysis, invocationID );
        
        InvocationSolution* pInvocation = constructInvocation( invocationID, implicitTypePath );
        
        pInvocation->build( m_analysis, nameResolution );
        
        m_invocations.insert( std::make_pair( invocationID, pInvocation ) );
                                    
        return pInvocation;
    }
}