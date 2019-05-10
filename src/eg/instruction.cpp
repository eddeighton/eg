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

#include "instruction.hpp"
#include "codegen.hpp"

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
                    case eFailureInstruction:                   pNewElement = new FailureInstruction; break;
                    case eEliminationInstruction:               pNewElement = new EliminationInstruction; break;
                    case eDimensionReferenceReadInstruction:    pNewElement = new DimensionReferenceReadInstruction; break;
                    case eMonoReferenceInstruction:             pNewElement = new MonomorphicReferenceInstruction; break;
                    case ePolyReferenceInstruction:             pNewElement = new PolymorphicReferenceBranchInstruction; break;
                    case ePolyCaseInstruction:                  pNewElement = new PolymorphicCaseInstruction; break;
                            
                    case eStartOperation:                       pNewElement = new StartOperation; break;
                    case eStopOperation:                        pNewElement = new StopOperation; break;
                    case ePauseOperation:                       pNewElement = new PauseOperation; break;
                    case eResumeOperation:                      pNewElement = new ResumeOperation; break;
                    case eGetActionOperation:                   pNewElement = new GetActionOperation; break;
                    case eReadOperation:                        pNewElement = new ReadOperation; break;
                    case eWriteOperation:                       pNewElement = new WriteOperation; break;
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
                case eFailureInstruction                 :
                case eEliminationInstruction             :
                case eDimensionReferenceReadInstruction  :
                case eMonoReferenceInstruction           :
                case ePolyReferenceInstruction           :
                case ePolyCaseInstruction                :
                    pChild->getOperations( operations );
                    break;
                
                case eStartOperation                     :
                case eStopOperation                      :
                case ePauseOperation                     :
                case eResumeOperation                    :
                case eGetActionOperation                 :
                case eReadOperation                      :
                case eWriteOperation                     :
                    operations.push_back( dynamic_cast< const Operation* >( pChild ) );
                    break;
                                
                case TOTAL_AST_TYPES:
                default:
                    THROW_RTE( "Unreachable" );
                    break;
            }
        }
    }
    
    void RootInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
        serialiser.load( loader, m_pContext );
    }
    void RootInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
        serialiser.store( storer, m_pContext );
    }
    void RootInstruction::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        generator.setVarExpr( m_pContext, "context" );
        
        ASSERT( m_children.size() == 1U );
        for( const Instruction* pChild : m_children )
        {
            pChild->generate( generator, os );
        }
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
    void ParentDerivationInstruction::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        std::ostringstream osExpr;
        {
            const concrete::Action* pType = m_pFrom->getConcreteType();
            osExpr << "( " << generator.getVarExpr( m_pFrom ) << " )";
            osExpr << " / " << pType->getLocalDomainSize();
        }
        generator.setVarExpr( m_pTo, osExpr.str() );
        
        ASSERT( m_children.size() == 1U );
        for( const Instruction* pChild : m_children )
        {
            pChild->generate( generator, os );
        }
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
    void ChildDerivationInstruction::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        generator.setVarExpr( m_pTo, generator.getVarExpr( m_pFrom ) );
        ASSERT( m_children.size() == 1U );
        for( const Instruction* pChild : m_children )
        {
            pChild->generate( generator, os );
        }
    }
    
    void FailureInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
    }
    void FailureInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
    }
    
    void EliminationInstruction::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Instruction::load( serialiser, loader );
    }
    void EliminationInstruction::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Instruction::store( serialiser, storer );
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
    void DimensionReferenceReadInstruction::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        std::ostringstream osReadRef;
        osReadRef << generator.getDimension( m_pDimension, generator.getVarExpr( m_pInstance ) );
        generator.setVarExpr( m_pReference, osReadRef.str() );
        
        ASSERT( m_children.size() == 1U );
        for( const Instruction* pChild : m_children )
        {
            pChild->generate( generator, os );
        }
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
    void MonomorphicReferenceInstruction::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        std::ostringstream osExpr;
        {
            osExpr << generator.getVarExpr( m_pFrom ) << ".data.instance";
        }
        generator.setVarExpr( m_pInstance, osExpr.str() );
        
        ASSERT( m_children.size() == 1U );
        for( const Instruction* pChild : m_children )
        {
            pChild->generate( generator, os );
        }
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
    void PolymorphicReferenceBranchInstruction::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        os << generator.getIndent() << "switch( " << generator.getVarExpr( m_pFrom ) << ".data.type )\n";
        os << generator.getIndent() << "{\n";
        generator.pushIndent();
        for( const Instruction* pChild : m_children )
        {
            pChild->generate( generator, os );
        }
        generator.popIndent();
        os << generator.getIndent() << "}\n";
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
    void PolymorphicCaseInstruction::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        std::ostringstream osExpr;
        {
            osExpr << generator.getVarExpr( m_pReference ) << ".data.instance";
        }
        generator.setVarExpr( m_pTo, osExpr.str() );
        
        const concrete::Action* pType = m_pTo->getConcreteType();
        os << generator.getIndent() << "case " << pType->getIndex() << ":\n";
        os << generator.getIndent() << "{\n";
        generator.pushIndent();
        for( const Instruction* pChild : m_children )
        {
            pChild->generate( generator, os );
        }
        generator.popIndent();
        os << generator.getIndent() << "}\n";
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
    
    void StartOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void StartOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pTarget );
    }
    void StartOperation::getTargetAbstractTypes( std::vector< const abstract::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pTarget->getAction() );
    }
    void StartOperation::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        os << generator.getIndent() << "return " << m_pTarget->getName() << 
            "_starter( " << generator.getVarExpr( m_pInstance ) << " );\n";
    }
    
    void StopOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void StopOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pTarget );
    }
    void StopOperation::getTargetAbstractTypes( std::vector< const abstract::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pTarget->getAction() );
    }
    void StopOperation::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        os << generator.getIndent() << "return " << m_pTarget->getName() << 
            "_stopper( " << generator.getVarExpr( m_pInstance ) << " );\n";
    }
    
    void PauseOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void PauseOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pTarget );
    }
    void PauseOperation::getTargetAbstractTypes( std::vector< const abstract::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pTarget->getAction() );
    }
    void PauseOperation::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        os << generator.getIndent() << 
            generator.getDimension( m_pTarget->getPauseTimestamp(), generator.getVarExpr( m_pInstance ) ) << 
                " = " << EG_INVALID_TIMESTAMP << ";\n";
    }
    
    void ResumeOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void ResumeOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pTarget );
    }
    void ResumeOperation::getTargetAbstractTypes( std::vector< const abstract::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pTarget->getAction() );
    }
    void ResumeOperation::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        os << generator.getIndent() << 
            generator.getDimension( m_pTarget->getPauseTimestamp(), generator.getVarExpr( m_pInstance ) ) << 
                " = clock::subcycle() + 1;\n";
    }
    
    void GetActionOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pTarget = loader.loadObjectRef< concrete::Action >();
    }
    void GetActionOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pTarget );
    }
    void GetActionOperation::getTargetAbstractTypes( std::vector< const abstract::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pTarget->getAction() );
    }
    void GetActionOperation::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        //os << generator.getIndent() << "if( " <<
        //    generator.getDimension( m_pTarget->getRunningTimestamp(), generator.getVarExpr( m_pInstance ) ) <<
        //    " <= clock::subcycle() )\n";
        
        os << generator.getIndent() << "return " <<
            generator.getDimension( m_pTarget->getReference(), generator.getVarExpr( m_pInstance ) ) << ";\n";
            
        //os << generator.getIndent() << "else\n";
        
        //os << generator.getIndent() << "    return " <<
        
    }
    
    void ReadOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pTarget = loader.loadObjectRef< concrete::Dimension_User >();
    }
    void ReadOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pTarget );
    }
    void ReadOperation::getTargetAbstractTypes( std::vector< const abstract::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pTarget->getDimension() );
    }
    void ReadOperation::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        os << generator.getIndent() << "return " << 
            generator.getDimension( m_pTarget, generator.getVarExpr( m_pInstance ) ) << ";\n";
    }
    
    void WriteOperation::load( ASTSerialiser& serialiser, Loader& loader )
    {
        Operation::load( serialiser, loader );
        serialiser.load( loader, m_pInstance );
        m_pTarget = loader.loadObjectRef< concrete::Dimension_User >();
    }
    void WriteOperation::store( ASTSerialiser& serialiser, Storer& storer ) const
    {
        Operation::store( serialiser, storer );
        serialiser.store( storer, m_pInstance );
        storer.storeObjectRef( m_pTarget );
    }
    void WriteOperation::getTargetAbstractTypes( std::vector< const abstract::Element* >& abstractTypes ) const
    {
        abstractTypes.push_back( m_pTarget->getDimension() );
    }
    void WriteOperation::generate( CodeGenerator& generator, std::ostream& os ) const
    {
        os << generator.getIndent() << 
            generator.getDimension( m_pTarget, generator.getVarExpr( m_pInstance ) ) << " = value;\n";
    }
}