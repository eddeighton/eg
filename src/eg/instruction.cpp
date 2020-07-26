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

#include "eg_compiler/instruction.hpp"

namespace eg
{

    void ASTSerialiser::load( Loader& loader, const ASTElement*& pElement )
    {
        bool bNull = false;
        loader.load( bNull );
        if( bNull )
        {
            pElement = nullptr;
        }
        else
        {
            std::size_t szIndex = 0U;
            loader.load( szIndex );
            
            ASTElementMapBack::iterator iFind = m_addressMapBack.find( szIndex );
            if( iFind != m_addressMapBack.end() )
            {
                pElement = iFind->second;
                VERIFY_RTE( pElement );
            }
            else
            {
                ASTElementType type = TOTAL_AST_TYPES;
                loader.load( type );
                
                ASTElement* pNewElement = nullptr;
                switch( type )
                {
                    case eInstanceVariable:                     pNewElement = new InstanceVariable; break;
                    case eReferenceVariable:                    pNewElement = new ReferenceVariable; break;
                    case eDimensionVariable:                    pNewElement = new DimensionReferenceVariable; break;
                    case eContextVariable:                      pNewElement = new ContextVariable; break;
                            
                    case eRootInstruction:                      THROW_RTE( "Unreachable" );
                    case eParentDerivationInstruction:          pNewElement = new ParentDerivationInstruction; break;
                    case eChildDerivationInstruction:           pNewElement = new ChildDerivationInstruction; break;
                    case eEnumDerivationInstruction:            pNewElement = new EnumDerivationInstruction; break;
                    case eEnumerationInstruction:               pNewElement = new EnumerationInstruction; break;
                    case eFailureInstruction:                   THROW_RTE( "Unreachable" );
                    case eEliminationInstruction:               THROW_RTE( "Unreachable" );
                    case ePruneInstruction:                     THROW_RTE( "Unreachable" );
                    case eDimensionReferenceReadInstruction:    pNewElement = new DimensionReferenceReadInstruction; break;
                    case eMonoReferenceInstruction:             pNewElement = new MonomorphicReferenceInstruction; break;
                    case ePolyReferenceInstruction:             pNewElement = new PolymorphicReferenceBranchInstruction; break;
                    case ePolyCaseInstruction:                  pNewElement = new PolymorphicCaseInstruction; break;
                            
                    case eCallOperation:                        pNewElement = new CallOperation; break;
                    case eStartOperation:                       pNewElement = new StartOperation; break;
                    case eStopOperation:                        pNewElement = new StopOperation; break;
                    case ePauseOperation:                       pNewElement = new PauseOperation; break;
                    case eResumeOperation:                      pNewElement = new ResumeOperation; break;
                    case eDoneOperation:                        pNewElement = new DoneOperation; break;
                    case eWaitActionOperation:                  pNewElement = new WaitActionOperation; break;
                    case eWaitDimensionOperation:               pNewElement = new WaitDimensionOperation; break;
                    case eGetActionOperation:                   pNewElement = new GetActionOperation; break;
                    case eGetDimensionOperation:                pNewElement = new GetDimensionOperation; break;
                    case eReadOperation:                        pNewElement = new ReadOperation; break;
                    case eWriteOperation:                       pNewElement = new WriteOperation; break;
                    case eWriteLinkOperation:                   pNewElement = new WriteLinkOperation; break;
                    case eRangeOperation:                       pNewElement = new RangeOperation; break;
                    default:
                        break;
                }
                VERIFY_RTE( pNewElement );
                
                m_addressMap.insert( std::make_pair( pNewElement, szIndex ) );
                m_addressMapBack.insert( std::make_pair( szIndex, pNewElement ) );
                
                pNewElement->load( *this, loader );
                
                pElement = pNewElement;
            }
        }
    }
    
    void ASTSerialiser::store( Storer& storer, const ASTElement* pElement )
    {
        if( nullptr == pElement )
        {
            storer.store( true );
        }
        else
        {
            storer.store( false );
            ASTElementMap::iterator iFind = m_addressMap.find( pElement );
            if( iFind != m_addressMap.end() )
            {
                storer.store( iFind->second );
            }
            else
            {
                const std::size_t szIndex = m_addressMap.size();
                storer.store( szIndex );
                m_addressMap.insert( std::make_pair( pElement, szIndex ) );
                m_addressMapBack.insert( std::make_pair( szIndex, pElement ) );
                
                storer.store( pElement->getType() );
                pElement->store( *this, storer );
            }
        }
    }

    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    void Variable::load( ASTSerialiser& serialiser, Loader& loader )
    {
        serialiser.load( loader, m_pParent );
    }
    void Variable::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        serialiser.store( storer, m_pParent );
    }
    
    void InstanceVariable::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Variable::load( serialiser, loader );
        m_pType = loader.loadObjectRef< concrete::Action >();
    }
    void InstanceVariable::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Variable::store( serialiser, storer );
        storer.storeObjectRef( m_pType );
    }
    
    void ReferenceVariable::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Variable::load( serialiser, loader );
        loader.loadObjectVector( m_types );
    }
    void ReferenceVariable::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Variable::store( serialiser, storer );
        storer.storeObjectVector( m_types );
    }
    
    void DimensionReferenceVariable::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Variable::load( serialiser, loader );
    }
    void DimensionReferenceVariable::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Variable::store( serialiser, storer );
    }
    
    void ContextVariable::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Variable::load( serialiser, loader );
    }
    void ContextVariable::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Variable::store( serialiser, storer );
    }
    
    
    
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    void Instruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        std::size_t szSize = 0U;
        loader.load( szSize );
        for( std::size_t sz = 0U; sz != szSize; ++sz )
        {
            Instruction* pInstruction = nullptr;
            serialiser.load( loader, pInstruction );
            VERIFY_RTE( pInstruction );
            m_children.push_back( pInstruction );
        }
    }
    
    void Instruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        storer.store( m_children.size() );
        for( Vector::const_iterator i = m_children.begin(),
            iEnd = m_children.end(); i!=iEnd; ++i )
        {
            serialiser.store( storer, *i );
        }
    }
    
    Instruction::Elimination Instruction::eliminate()
    {
        Elimination elimination = eSuccess;
        
        for( std::size_t sz = 0U; sz != m_children.size(); ++sz )
        {
            Instruction* pChild = m_children[ sz ];
            if( EliminationInstruction* pElimination = 
                dynamic_cast< EliminationInstruction* >( pChild ) )
            {
                Vector success, failure, ambiguous;
                
                Vector& nested = pElimination->m_children;
                for( Instruction* pNested : nested )
                {
                    VERIFY_RTE( !dynamic_cast< EliminationInstruction* >( pNested ) );
                    const Elimination nestedResult = pNested->eliminate();
                    switch( nestedResult )
                    {
                        case eSuccess:   success.push_back( pNested );    break;
                        case eFailure:   failure.push_back( pNested );    break;
                        case eAmbiguous: ambiguous.push_back( pNested );  break;
                    }
                }
                
                nested.clear();
                
                for( Instruction* pFailure : failure )
                    delete pFailure;
                
                if( !ambiguous.empty() || ( success.size() > 1U ) )
                {
                    nested = std::move( ambiguous );
                    std::copy( success.begin(), success.end(), 
                        std::back_inserter( nested ) );
                    elimination = eAmbiguous;
                }
                else if( success.size() == 1U )
                {
                    //successful
                    m_children[ sz ] = success.front();
                    delete pElimination;
                }
                else if( success.empty() )
                {
                    if( elimination != eAmbiguous )
                        elimination = eFailure;
                }
            }
            else if( FailureInstruction* pFailure = 
                dynamic_cast< FailureInstruction* >( pChild ) )
            {
                if( elimination != eAmbiguous )
                    elimination = eFailure;
            }
            else if( PruneInstruction* pPrune = 
                dynamic_cast< PruneInstruction* >( pChild ) )
            {
                Vector success, failure;
                
                Vector& nested = pPrune->m_children;
                for( Instruction* pNested : nested )
                {
                    VERIFY_RTE( !dynamic_cast< PruneInstruction* >( pNested ) );
                    const Elimination nestedResult = pNested->eliminate();
                    switch( nestedResult )
                    {
                        case eSuccess:   success.push_back( pNested );    break;
                        case eFailure:   failure.push_back( pNested );    break;
                        case eAmbiguous: THROW_RTE( "Unreachable" );
                    }
                }
                
                nested.clear();
                
                for( Instruction* pFailure : failure )
                    delete pFailure;
                
                if( !success.empty() )
                {
                    //successful
                    VERIFY_RTE( m_children.size() == 1 );
                    m_children = std::move( success );
                    delete pPrune;
                    break;
                }
                else if( success.empty() )
                {
                    elimination = eFailure;
                }
            }
            else
            {
                const Elimination nestedResult = pChild->eliminate();
                switch( nestedResult )
                {
                    case eSuccess:
                        break;
                    case eFailure:  
                        if( elimination != eAmbiguous )
                            elimination = eFailure;
                        break;
                    case eAmbiguous: 
                        elimination = eAmbiguous;
                        break;
                }
            }
        }
        
        return elimination;
    }
    
    Instruction::Elimination Instruction::secondStageElimination( const std::vector< const Operation* >& candidateOperations )
    {
        Elimination elimination = eSuccess;
        
        if( Operation* pOperation = dynamic_cast< Operation* >( this ) )
        {
            std::vector< const Operation* >::const_iterator iFind = 
                std::find( candidateOperations.begin(), candidateOperations.end(), pOperation );
            if( iFind == candidateOperations.end() )
            {
                return eFailure;
            }
        }
        
        for( std::size_t sz = 0U; sz != m_children.size(); ++sz )
        {
            Instruction* pChild = m_children[ sz ];
            
            if( EliminationInstruction* pElimination = 
                dynamic_cast< EliminationInstruction* >( pChild ) )
            {
                Vector success, failure, ambiguous;
                
                Vector& nested = pElimination->m_children;
                for( Instruction* pNested : nested )
                {
                    VERIFY_RTE( !dynamic_cast< EliminationInstruction* >( pNested ) );
                    const Elimination nestedResult = pNested->secondStageElimination( candidateOperations );
                    switch( nestedResult )
                    {
                        case eSuccess:   success.push_back( pNested );    break;
                        case eFailure:   failure.push_back( pNested );    break;
                        case eAmbiguous: ambiguous.push_back( pNested );  break;
                    }
                }
                
                nested.clear();
                
                for( Instruction* pFailure : failure )
                    delete pFailure;
                
                if( !ambiguous.empty() || ( success.size() > 1U ) )
                {
                    nested = std::move( ambiguous );
                    std::copy( success.begin(), success.end(), 
                        std::back_inserter( nested ) );
                    elimination = eAmbiguous;
                }
                else if( success.size() == 1U )
                {
                    //successful
                    m_children[ sz ] = success.front();
                    delete pElimination;
                }
                else if( success.empty() )
                {
                    if( elimination != eAmbiguous )
                        elimination = eFailure;
                }
            }
            else if( FailureInstruction* pFailure = 
                dynamic_cast< FailureInstruction* >( pChild ) )
            {
                if( elimination != eAmbiguous )
                    elimination = eFailure;
            }
            else if( PruneInstruction* pPrune = 
                dynamic_cast< PruneInstruction* >( pChild ) )
            {
                Vector success, failure;
                
                Vector& nested = pPrune->m_children;
                for( Instruction* pNested : nested )
                {
                    VERIFY_RTE( !dynamic_cast< PruneInstruction* >( pNested ) );
                    const Elimination nestedResult = pNested->secondStageElimination( candidateOperations );
                    switch( nestedResult )
                    {
                        case eSuccess:   success.push_back( pNested );    break;
                        case eFailure:   failure.push_back( pNested );    break;
                        case eAmbiguous: THROW_RTE( "Unreachable" );
                    }
                }
                
                nested.clear();
                
                for( Instruction* pFailure : failure )
                    delete pFailure;
                
                if( !success.empty() )
                {
                    //successful
                    VERIFY_RTE( m_children.size() == 1 );
                    m_children = std::move( success );
                    delete pPrune;
                    break;
                }
                else if( success.empty() )
                {
                    elimination = eFailure;
                }
            }
            else
            {
                const Elimination nestedResult = pChild->secondStageElimination( candidateOperations );
                switch( nestedResult )
                {
                    case eSuccess:
                        break;
                    case eFailure:  
                        if( elimination != eAmbiguous )
                            elimination = eFailure;
                        break;
                    case eAmbiguous: 
                        elimination = eAmbiguous;
                        break;
                }
            }
        }
        
        return elimination;
    }
    
    void Instruction::getOperations( std::vector< const Operation* >& operations ) const
    {
        for( const Instruction* pChild : m_children )
        {
            switch( pChild->getType() )
            {
                case eInstanceVariable                   :
                case eReferenceVariable                  :
                case eDimensionVariable                  :
                case eContextVariable                    :
                
                case eRootInstruction                    :
                case eParentDerivationInstruction        :
                case eChildDerivationInstruction         :
                case eEnumDerivationInstruction          :
                case eEnumerationInstruction             :
                case eFailureInstruction                 :
                case eEliminationInstruction             :
                case ePruneInstruction                   :
                case eDimensionReferenceReadInstruction  :
                case eMonoReferenceInstruction           :
                case ePolyReferenceInstruction           :
                case ePolyCaseInstruction                :
                    pChild->getOperations( operations );
                    break;
                
                case eCallOperation                      :
                case eStartOperation                     :
                case eStopOperation                      :
                case ePauseOperation                     :
                case eResumeOperation                    :
                case eDoneOperation                      :
                case eWaitActionOperation                :
                case eWaitDimensionOperation             :
                case eGetActionOperation                 :
                case eGetDimensionOperation              :
                case eReadOperation                      :
                case eWriteOperation                     :
                case eWriteLinkOperation                 :
                case eRangeOperation                     :
                    operations.push_back( dynamic_cast< const Operation* >( pChild ) );
                    break;
                                
                case TOTAL_AST_TYPES:
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
    }
    
    int Instruction::setReturnTypes( const std::vector< const interface::Element* >& targets )
    {
        int maxReturnTypes = 1;
        for( Instruction* pChild : m_children )
        {
            maxReturnTypes = std::max( maxReturnTypes, pChild->setReturnTypes( targets ) );
        }
        return maxReturnTypes;
    }
    void Instruction::setMaxRanges( int iMaxRanges )
    {
        for( Instruction* pChild : m_children )
        {
            pChild->setMaxRanges( iMaxRanges );
        }
    }
    
    void RootInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pContext );
        loader.load( m_iMaxRanges );
    }
    void RootInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pContext );
        storer.store( m_iMaxRanges );
    }
    void RootInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        ASSERT( m_children.size() == 1U );
        m_children.front()->evaluate( evaluator );
    }
    
    void RootInstruction::setMaxRanges( int iMaxRanges )
    {
        Instruction::setMaxRanges( iMaxRanges );
        m_iMaxRanges = iMaxRanges;
    }
    
    void ParentDerivationInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pFrom );
        serialiser.load( loader, m_pTo );
    }
    void ParentDerivationInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pFrom );
        serialiser.store( storer, m_pTo );
    }
    void ParentDerivationInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        ASSERT( m_children.size() == 1U );
        const reference& from = evaluator.getVarValue( m_pFrom );
        
        const reference to{ from.instance / m_pFrom->getConcreteType()->getLocalDomainSize(), 
            static_cast< eg::TypeID >( m_pTo->getConcreteType()->getIndex() ), from.timestamp };
        
        evaluator.setVarValue( m_pTo, to );
        
        m_children.front()->evaluate( evaluator );
    }
    
    void ChildDerivationInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pFrom );
        serialiser.load( loader, m_pTo );
    }
    void ChildDerivationInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pFrom );
        serialiser.store( storer, m_pTo );
    }
    void ChildDerivationInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        ASSERT( m_children.size() == 1U );
        const reference& from = evaluator.getVarValue( m_pFrom );
        
        const reference to{ from.instance, static_cast< eg::TypeID >( m_pTo->getConcreteType()->getIndex() ), from.timestamp };
        
        evaluator.setVarValue( m_pTo, to );
        
        m_children.front()->evaluate( evaluator );
    }
    
    void EnumDerivationInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pFrom );
        serialiser.load( loader, m_pTo );
    }
    void EnumDerivationInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pFrom );
        serialiser.store( storer, m_pTo );
    }
    void EnumDerivationInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        THROW_RTE( "Unreachable" );
    }
    
    void EnumerationInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pContext );
        loader.loadObjectVector( m_returnTypes );
        loader.load( m_iMaxRanges );
    }
    void EnumerationInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pContext );
        storer.storeObjectVector( m_returnTypes );
        storer.store( m_iMaxRanges );
    }
    int EnumerationInstruction::setReturnTypes( const std::vector< const interface::Element* >& targets )
    {
        m_returnTypes = targets;
        std::vector< const Operation* > operations;
        getOperations( operations );
        return operations.size();
    }
    void EnumerationInstruction::setMaxRanges( int iMaxRanges )
    {
        m_iMaxRanges = iMaxRanges;
    }
    
    
    void EnumerationInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        ASSERT( !m_returnTypes.empty() );
        
        std::vector< const Operation* > operations;
        getOperations( operations );
        
        const concrete::Action* pEnumerationAction = m_pContext->getConcreteType();
        
        RangeDescription rangeDescription;
        rangeDescription.raw = true;
        
        ASSERT( !operations.empty() );
        if( !operations.empty() )
        {
            const RangeOperation* pFirstRangeOp = dynamic_cast< const RangeOperation* >( 
                dynamic_cast< const RangeOperation* >( operations.front() ) );
            ASSERT( pFirstRangeOp );
            switch( pFirstRangeOp->getRangeType() )
            {
                case RangeOperation::eRaw:      rangeDescription.raw = true;  break;
                case RangeOperation::eRange:    rangeDescription.raw = false; break;
                default: THROW_RTE( "Unknown range type" );
            }
        }
        
        for( const Operation* pOperation : operations )
        {
            const RangeOperation* pRangeOp = dynamic_cast< const RangeOperation* >( pOperation );
            ASSERT( pRangeOp );
            const concrete::Action* pTarget = pRangeOp->getTarget();
            std::int32_t szDomainMultiplier = 1U;
            
            const concrete::Action* pIter = pTarget;
            for( ; pIter != pEnumerationAction; 
                pIter = dynamic_cast< const concrete::Action* >( pIter->getParent() ) )
            {
                ASSERT( pIter );
                szDomainMultiplier *= pIter->getLocalDomainSize();
            }
            ASSERT( pIter == pEnumerationAction );
            
            rangeDescription.ranges.push_back(
                RangeDescription::SubRange
                { 
                    static_cast< TypeID >( pTarget->getIndex() ), 
                    evaluator.getVarValue( m_pContext ).instance * szDomainMultiplier, 
                    ( evaluator.getVarValue( m_pContext ).instance + 1 ) * szDomainMultiplier 
                } );
        }
        
        evaluator.doRange( rangeDescription );
    }
    
    void FailureInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        THROW_RTE( "Unreachable" );
    }
    void FailureInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        THROW_RTE( "Unreachable" );
    }
    
    void EliminationInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        THROW_RTE( "Unreachable" );
    }
    void EliminationInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        THROW_RTE( "Unreachable" );
    }
    
    void PruneInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        THROW_RTE( "Unreachable" );
    }
    void PruneInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        THROW_RTE( "Unreachable" );
    }
    
    void DimensionReferenceReadInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        serialiser.load( loader, m_pReference );
        m_pDimension = loader.loadObjectRef< concrete::Dimension >();
    }
    void DimensionReferenceReadInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        serialiser.store( storer, m_pReference );
        storer.storeObjectRef( m_pDimension );
    }
    void DimensionReferenceReadInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        const reference& actionRef = evaluator.getVarValue( m_pInstance );
        
        const reference deref = evaluator.dereferenceDimension( actionRef, m_pDimension->getIndex() );
        
        evaluator.setVarValue( m_pReference, deref );
        
        ASSERT( m_children.size() == 1U );
        m_children.front()->evaluate( evaluator );
    }
    
    void MonomorphicReferenceInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pFrom );
        serialiser.load( loader, m_pInstance );
    }
    void MonomorphicReferenceInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pFrom );
        serialiser.store( storer, m_pInstance );
    }
    void MonomorphicReferenceInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        const reference& ref = evaluator.getVarValue( m_pFrom );
        evaluator.setVarValue( m_pInstance, ref );
        ASSERT( m_children.size() == 1U );
        m_children.front()->evaluate( evaluator );
    }
    
    void PolymorphicReferenceBranchInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pFrom );
    }
    void PolymorphicReferenceBranchInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pFrom );
    }
    void PolymorphicReferenceBranchInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        const reference& ref = evaluator.getVarValue( m_pFrom );
        
        bool bFound = false;
        for( const Instruction* pChild : m_children )
        {
            const PolymorphicCaseInstruction* pCase = 
                dynamic_cast< const PolymorphicCaseInstruction* >( pChild );
            ASSERT( pCase );
            
            if( pCase->getTarget()->getConcreteType()->getIndex() == ref.type )
            {
                pChild->evaluate( evaluator );
                bFound = true;
                break;
            }
        }
        if( !bFound )
        {
            THROW_RTE( "Error handling needed here..." );
        }
    }
    
    void PolymorphicCaseInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pReference );
        serialiser.load( loader, m_pTo );
    }
    void PolymorphicCaseInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pReference );
        serialiser.store( storer, m_pTo );
    }
    void PolymorphicCaseInstruction::evaluate( RuntimeEvaluator& evaluator ) const
    {
        const reference& ref = evaluator.getVarValue( m_pReference );
        evaluator.setVarValue( m_pTo, ref );
        ASSERT( m_children.size() == 1U );
        m_children.front()->evaluate( evaluator );
    }
    
    ///////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////
    void Operation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
    }
    void Operation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
    }
    
    void CallOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void CallOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void CallOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void CallOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID CallOperation::getExplicitOperationType() const
    {
        return id_exp_Call;
    }
    void CallOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doCall( evaluator.getVarValue( m_pInstance ), m_pTarget->getIndex() );
    }
    
    void StartOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void StartOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void StartOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void StartOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID StartOperation::getExplicitOperationType() const
    {
        return id_exp_Start;
    }
    void StartOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doStart( evaluator.getVarValue( m_pInstance ), m_pTarget->getIndex() );
    }
    
    void StopOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void StopOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void StopOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void StopOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID StopOperation::getExplicitOperationType() const
    {
        return id_exp_Stop;
    }
    void StopOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doStop( evaluator.getVarValue( m_pInstance ) );
    }
    
    void PauseOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void PauseOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void PauseOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void PauseOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID PauseOperation::getExplicitOperationType() const
    {
        return id_exp_Pause;
    }
    void PauseOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doPause( evaluator.getVarValue( m_pInstance ) );
    }
    
    void ResumeOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void ResumeOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void ResumeOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void ResumeOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID ResumeOperation::getExplicitOperationType() const
    {
        return id_exp_Resume;
    }
    void ResumeOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doResume( evaluator.getVarValue( m_pInstance ) );
    }
    
    
    void DoneOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void DoneOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void DoneOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void DoneOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID DoneOperation::getExplicitOperationType() const
    {
        return id_exp_Done;
    }
    void DoneOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doDone( evaluator.getVarValue( m_pInstance ) );
    }
    
    void WaitActionOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void WaitActionOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void WaitActionOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void WaitActionOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID WaitActionOperation::getExplicitOperationType() const
    {
        return id_exp_WaitAction;
    }
    void WaitActionOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doWaitAction( evaluator.getVarValue( m_pInstance ) );
    }
    
    void WaitDimensionOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Dimension >();
        m_pTarget = loader.loadObjectRef< concrete::Dimension_User >();
    }
    void WaitDimensionOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void WaitDimensionOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void WaitDimensionOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID WaitDimensionOperation::getExplicitOperationType() const
    {
        return id_exp_WaitDimension;
    }
    void WaitDimensionOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doWaitDimension( evaluator.getVarValue( m_pInstance ), m_pTarget->getIndex() );
    }
    
    void GetActionOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void GetActionOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void GetActionOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void GetActionOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID GetActionOperation::getExplicitOperationType() const
    {
        return id_exp_GetAction;
    }
    void GetActionOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doGetAction( evaluator.getVarValue( m_pInstance ) );
    }
    
    void GetDimensionOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Dimension >();
        m_pTarget = loader.loadObjectRef< concrete::Dimension_User >();
    }
    void GetDimensionOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void GetDimensionOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void GetDimensionOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID GetDimensionOperation::getExplicitOperationType() const
    {
        return id_exp_GetDimension;
    }
    void GetDimensionOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doGetDimension( evaluator.getVarValue( m_pInstance ), m_pTarget->getIndex() );
    }
    
    void ReadOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Dimension >();
        m_pTarget = loader.loadObjectRef< concrete::Dimension_User >();
    }
    void ReadOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void ReadOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void ReadOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID ReadOperation::getExplicitOperationType() const
    {
        return id_exp_Read;
    }
    void ReadOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doRead( evaluator.getVarValue( m_pInstance ), m_pTarget->getIndex() );
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    WriteOperation::WriteOperation( InstanceVariable* pInstance, 
				const interface::Dimension* pInterface, const concrete::Dimension_User* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
		
    }
    void WriteOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Dimension >();
        m_pTarget = loader.loadObjectRef< concrete::Dimension_User >();
    }
    void WriteOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
    }
    void WriteOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInstance->getConcreteType()->getContext() );
    }
    void WriteOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    ExplicitOperationID WriteOperation::getExplicitOperationType() const
    {
        return id_exp_Write;
    }
    void WriteOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        evaluator.doWrite( evaluator.getVarValue( m_pInstance ), m_pTarget->getIndex() );
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    WriteLinkOperation::WriteLinkOperation( InstanceVariable* pInstance, 
				const interface::Dimension* pInterface, 
				const concrete::Dimension_User* pTarget,
                const LinkGroup* pLinkGroup )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget ),
            m_pLinkGroup( pLinkGroup )
    {
		
    }
    void WriteLinkOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface 		= loader.loadObjectRef< interface::Dimension >();
        m_pTarget 			= loader.loadObjectRef< concrete::Dimension_User >();
        m_pLinkGroup        = loader.loadObjectRef< LinkGroup >();
    }
    void WriteLinkOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
        storer.storeObjectRef( m_pLinkGroup );
    }
    void WriteLinkOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInstance->getConcreteType()->getContext() );
    }
    void WriteLinkOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    ExplicitOperationID WriteLinkOperation::getExplicitOperationType() const
    {
        return id_exp_Write;
    }
    void WriteLinkOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
		THROW_RTE( "TODO WriteLinkOperation emulation" );
        //evaluator.doWrite( evaluator.getVarValue( m_pInstance ), m_pTarget->getIndex() );
        //evaluator.doLink( evaluator.getVarValue( m_pLinkInstance ), 
		//	m_pLinkReference->getIndex(), evaluator.getVarValue( m_pInstance ) );
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////
    void RangeOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pInterface = loader.loadObjectRef< interface::Context >();
        m_pTarget = loader.loadObjectRef< concrete::Action >();
        loader.load( m_rangeType );
    }
    void RangeOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pInterface );
        storer.storeObjectRef( m_pTarget );
        storer.store( m_rangeType );
    }
    void RangeOperation::getReturnTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pInterface );
    }
    void RangeOperation::getParameterTypes( std::vector< const interface::Element* >& abstractTypes ) const
    {
    }
    ExplicitOperationID RangeOperation::getExplicitOperationType() const
    {
        switch( m_rangeType )
        {
            case eRaw:                return id_exp_Raw;
            case eRange:              return id_exp_Range;
            default:
            case TOTAL_RANGE_TYPES:   THROW_RTE( "Invalid range type" );
        }
        
    }
    void RangeOperation::evaluate( RuntimeEvaluator& evaluator ) const
    {
        THROW_RTE( "Unreachable" );
    }
}