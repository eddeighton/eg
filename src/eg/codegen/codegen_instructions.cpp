
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


#include "eg_compiler/codegen/codegen.hpp"

#include "eg_compiler/eg.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/interface.hpp"
#include "eg_compiler/concrete.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/layout.hpp"
#include "eg_compiler/identifiers.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/translation_unit.hpp"
#include "eg_compiler/invocation.hpp"
#include "eg_compiler/allocator.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>

namespace eg
{
    class CodeGenerator
    {
        static const int INDENT = 4;
    public:
        CodeGenerator( const Layout& layout, int iIndent, const std::string& strFailureType, PrinterFactory& printerFactory ) 
            :   m_layout( layout ), 
                m_strFailureType( strFailureType ),
                m_printerFactory( printerFactory )
        {
            for( int i = 0; i != iIndent; ++i )
                pushIndent();
        }
        
        using VariableExprMap = std::map< const Variable*, std::string >;
        
        inline void setVarExpr( const Variable* pVariable, const std::string& str )
        {
            VariableExprMap::const_iterator iFind = m_variables.find( pVariable );
            VERIFY_RTE( iFind == m_variables.end() );
            m_variables.insert( std::make_pair( pVariable, str ) );
        }
        inline const std::string& getVarExpr( const Variable* pVariable ) const
        {
            VariableExprMap::const_iterator iFind = m_variables.find( pVariable );
            VERIFY_RTE( iFind != m_variables.end() );
            return iFind->second;
        }
        
        const std::string& getIndent() const { return m_strIndent; }
        void pushIndent() 
        { 
            for( int i = 0; i != INDENT; ++i )
                m_strIndent.push_back( ' ' );
        }
        void popIndent()
        {
            for( int i = 0; i != INDENT; ++i )
                m_strIndent.pop_back();
        }
        
        Printer::Ptr getDimension( const concrete::Dimension* pDimension, const std::string& strIndex )
        {
            return m_printerFactory.getPrinter( m_layout.getDataMember( pDimension ), strIndex.c_str() );
        }
        
        std::string getFailureReturnType() const { return m_strFailureType; }
        
        const Layout& getLayout() const { return m_layout; }
    private:
        VariableExprMap m_variables;
        std::string m_strIndent;
        const Layout& m_layout;
        std::string m_strFailureType;
        PrinterFactory& m_printerFactory;
    };

    
    //instruction code generators
    
    void generate( const Instruction& ins, CodeGenerator& generator, std::ostream& os );
    
    void generate( const RootInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        generator.setVarExpr( ins.getContextVariable(), "context" );
        
        ASSERT( ins.getChildren().size() == 1U );
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
    }
    void generate( const ParentDerivationInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        std::ostringstream osExpr;
        {
            const concrete::Action* pType = ins.getFrom()->getConcreteType();
            if( pType->getLocalDomainSize() > 1U )
            {
                osExpr << "( " << generator.getVarExpr( ins.getFrom() ) << " )";
                osExpr << " / " << pType->getLocalDomainSize();
            }
            else
            {
                osExpr << generator.getVarExpr( ins.getFrom() );
            }
        }
        generator.setVarExpr( ins.getTo(), osExpr.str() );
        
        ASSERT( ins.getChildren().size() == 1U );
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
    }
    void generate( const ChildDerivationInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        generator.setVarExpr( ins.getTo(), generator.getVarExpr( ins.getFrom() ) );
        ASSERT( ins.getChildren().size() == 1U );
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
    }
    void generate( const EnumDerivationInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        ASSERT( ins.getChildren().size() == 1U );
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
    }
    void generate( const EnumerationInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        ASSERT( !ins.getReturnTypes().empty() );
        
        std::vector< const Operation* > operations;
        ins.getOperations( operations );
        
        const concrete::Action* pEnumerationAction = ins.getContext()->getConcreteType();
        
        if( ins.getMaxRanges() == 1U )
        {
            ASSERT( operations.size() == 1U );
            const RangeOperation* pRangeOp = dynamic_cast< const RangeOperation* >( operations.front() );
            ASSERT( pRangeOp );
            const concrete::Action* pTarget = pRangeOp->getTarget();
            std::size_t szDomainMultiplier = 1U;
            
            const char* pszIteratorType = nullptr;
            {
                switch( pRangeOp->getRangeType() )
                {
                    case RangeOperation::eRaw:      pszIteratorType = EG_REFERENCE_RAW_ITERATOR_TYPE; break;
                    case RangeOperation::eRange:    pszIteratorType = EG_REFERENCE_ITERATOR_TYPE; break;
                    default: THROW_RTE( "Unknown range type" );
                }
                ASSERT( pszIteratorType );
            }
            
            const concrete::Action* pIter = pTarget;
            for( ; pIter != pEnumerationAction; 
                pIter = dynamic_cast< const concrete::Action* >( pIter->getParent() ) )
            {
                ASSERT( pIter );
                szDomainMultiplier *= pIter->getLocalDomainSize();
            }
            ASSERT( pIter == pEnumerationAction );
            
            os << generator.getIndent() << "const " << EG_INSTANCE << " iBegin = " <<
                generator.getVarExpr( ins.getContext() ) << " * " << szDomainMultiplier << ";\n";
            os << generator.getIndent() << "const " << EG_INSTANCE << " iEnd = ( " <<
                generator.getVarExpr( ins.getContext() ) << " + 1 ) * " << szDomainMultiplier << ";\n";
                
            if( ins.getReturnTypes().size() == 1U )
            {
                const interface::Context* pReturnType = dynamic_cast< const interface::Context* >( ins.getReturnTypes().front() );
                ASSERT( pReturnType );
                
                std::ostringstream osIterType;
                {
                    osIterType << pszIteratorType << "< " << getStaticType( pReturnType ) << " >";
                }
                os << generator.getIndent() << osIterType.str() << " begin( iBegin - 1, iEnd, " << pTarget->getIndex() << " ); //" << pTarget->getFriendlyName() << "\n";
                os << generator.getIndent() << "++begin;\n";
                os << generator.getIndent() << osIterType.str() << " end( iEnd, iEnd, " << pTarget->getIndex() << " );\n";
                os << generator.getIndent() << "return " << EG_RANGE_TYPE << "< " << osIterType.str() << " >( begin, end );\n";
            }
            else
            {
                std::ostringstream osType;
                {
                    osType << EG_VARIANT_TYPE << "< ";
                    for( const interface::Element* pElement : ins.getReturnTypes() )
                    {
                        const interface::Context* pReturnType = 
                            dynamic_cast< const interface::Context* >( pElement );
                        ASSERT( pReturnType );
                        if( pElement != *ins.getReturnTypes().begin())
                            osType << ", ";
                        osType << getStaticType( pReturnType );
                    }
                    osType << " >";
                }
                std::ostringstream osIterType;
                {
                    osIterType << pszIteratorType << "< " << osType.str() << " >";
                }
                os << generator.getIndent() << osIterType.str() << " begin( iBegin - 1, iEnd, " << pTarget->getIndex() << " ); //" << pTarget->getFriendlyName() << "\n";
                os << generator.getIndent() << "++begin;\n";
                os << generator.getIndent() << osIterType.str() << " end( iEnd, iEnd, " << pTarget->getIndex() << " );\n";
                os << generator.getIndent() << "return " << EG_RANGE_TYPE << "< " << osIterType.str() << " >( begin, end );\n";
            }
        }
        else
        {
            const char* pszIteratorType = nullptr;
            ASSERT( !operations.empty() );
            if( !operations.empty() )
            {
                const RangeOperation* pFirstRangeOp = dynamic_cast< const RangeOperation* >( 
                    dynamic_cast< const RangeOperation* >( operations.front() ) );
                ASSERT( pFirstRangeOp );
                switch( pFirstRangeOp->getRangeType() )
                {
                    case RangeOperation::eRaw:      pszIteratorType = EG_REFERENCE_RAW_ITERATOR_TYPE; break;
                    case RangeOperation::eRange:    pszIteratorType = EG_REFERENCE_ITERATOR_TYPE; break;
                    default: THROW_RTE( "Unknown range type" );
                }
                ASSERT( pszIteratorType );
            }
            
            std::ostringstream osReturnType;
            std::ostringstream osIterType;
            if( ins.getReturnTypes().size() == 1U )
            {
                const interface::Context* pReturnType = dynamic_cast< const interface::Context* >( ins.getReturnTypes().front() );
                ASSERT( pReturnType );
                osReturnType << getStaticType( pReturnType );
                osIterType << pszIteratorType << "< " << osReturnType.str() << " >";
            }
            else
            {
                osReturnType << EG_VARIANT_TYPE << "< ";
                for( const interface::Element* pElement : ins.getReturnTypes() )
                {
                    const interface::Context* pReturnType = 
                        dynamic_cast< const interface::Context* >( pElement );
                    ASSERT( pReturnType );
                    if( pElement != *ins.getReturnTypes().begin())
                        osReturnType << ", ";
                    osReturnType << getStaticType( pReturnType );
                }
                osReturnType << " >";
                osIterType << pszIteratorType << "< " << osReturnType.str() << " >";
            }
            
            os << generator.getIndent() << "using IterType = " << osIterType.str() << ";\n";
            os << generator.getIndent() << "using MultiIterType = " << EG_MULTI_ITERATOR_TYPE << "< IterType, " << ins.getMaxRanges() << " >;\n";
            
            //begin iterators
            os << generator.getIndent() << "MultiIterType::IteratorArray iterators_begin = \n";
            os << generator.getIndent() << "{\n";
            
            for( const Operation* pOperation : operations )
            {
                const RangeOperation* pRangeOp = dynamic_cast< const RangeOperation* >( pOperation );
                ASSERT( pRangeOp );
                const concrete::Action* pTarget = pRangeOp->getTarget();
                std::size_t szDomainMultiplier = 1U;
                
                const concrete::Action* pIter = pTarget;
                for( ; pIter != pEnumerationAction; 
                    pIter = dynamic_cast< const concrete::Action* >( pIter->getParent() ) )
                {
                    ASSERT( pIter );
                    szDomainMultiplier *= pIter->getLocalDomainSize();
                }
                ASSERT( pIter == pEnumerationAction );
                
                os << generator.getIndent() << "    IterType( " << 
                    "( " << generator.getVarExpr( ins.getContext() ) << " * " << szDomainMultiplier << " ) - 1U" <<
                    ", ( " << generator.getVarExpr( ins.getContext() ) << " + 1 ) * " << szDomainMultiplier <<
                    ", static_cast< eg::TypeID >( " << pTarget->getIndex() << " ) ), //" << pTarget->getFriendlyName() << "\n";
            }
            
            //add addition empty ranges up to m_iMaxRanges
            for( std::size_t sz = operations.size(); sz != ins.getMaxRanges(); ++sz )
            {
                os << generator.getIndent() << "    IterType( 0, 0, 0 ) ),\n";
            }
            
            os << generator.getIndent() << "};\n";
            
            std::size_t szIndex = 0U;
            for( const Operation* pOperation : operations )
            {
                os << generator.getIndent() << "++iterators_begin[ " << szIndex++ << " ];\n";
            }
            
            //end iterators
            os << generator.getIndent() << "MultiIterType::IteratorArray iterators_end = \n";
            os << generator.getIndent() << "{\n";
            
            for( const Operation* pOperation : operations )
            {
                const RangeOperation* pRangeOp = dynamic_cast< const RangeOperation* >( pOperation );
                ASSERT( pRangeOp );
                const concrete::Action* pTarget = pRangeOp->getTarget();
                std::size_t szDomainMultiplier = 1U;
                
                const concrete::Action* pIter = pTarget;
                for( ; pIter != pEnumerationAction; 
                    pIter = dynamic_cast< const concrete::Action* >( pIter->getParent() ) )
                {
                    ASSERT( pIter );
                    szDomainMultiplier *= pIter->getLocalDomainSize();
                }
                ASSERT( pIter == pEnumerationAction );
                
                os << generator.getIndent() << "    IterType( " << 
                    "( " << generator.getVarExpr( ins.getContext() ) << " + 1 ) * " << szDomainMultiplier <<
                    ", ( " << generator.getVarExpr( ins.getContext() ) << " + 1 ) * " << szDomainMultiplier <<
                    ", static_cast< eg::TypeID >( " << pTarget->getIndex() << " ) ), //" << pTarget->getFriendlyName() << "\n";
            }
            
            //add addition empty ranges up to m_iMaxRanges
            for( std::size_t sz = operations.size(); sz != ins.getMaxRanges(); ++sz )
            {
                os << generator.getIndent() << "    IterType( 0, 0, 0 ) ),\n";
            }
            
            os << generator.getIndent() << "};\n";
            
            os << generator.getIndent() << "return " << EG_RANGE_TYPE << 
                "< MultiIterType >( MultiIterType( iterators_begin ), MultiIterType( iterators_end ) );\n";
        }
    }
    void generate( const DimensionReferenceReadInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        std::ostringstream osReadRef;
        osReadRef << *generator.getDimension( ins.getDimension(), generator.getVarExpr( ins.getInstance() ) );
        generator.setVarExpr( ins.getReference(), osReadRef.str() );
        
        //ASSERT( ins.getChildren().size() == 1U );
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
    }
    void generate( const MonomorphicReferenceInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        std::ostringstream osExpr;
        {
            osExpr << generator.getVarExpr( ins.getFrom() ) << ".data.instance";
        }
        generator.setVarExpr( ins.getInstance(), osExpr.str() );
        
        //ASSERT( ins.getChildren().size() == 1U );
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
    }
    void generate( const PolymorphicReferenceBranchInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        os << generator.getIndent() << "switch( " << generator.getVarExpr( ins.getFrom() ) << ".data.type )\n";
        os << generator.getIndent() << "{\n";
        generator.pushIndent();
        
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
        
        os << generator.getIndent() << "default: throw std::runtime_error( \"runtime type error\" );\n";
        generator.popIndent();
        os << generator.getIndent() << "}\n";
    }
    void generate( const PolymorphicCaseInstruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        std::ostringstream osExpr;
        {
            osExpr << generator.getVarExpr( ins.getFrom() ) << ".data.instance";
        }
        generator.setVarExpr( ins.getTo(), osExpr.str() );
        
        const concrete::Action* pType = ins.getTo()->getConcreteType();
        os << generator.getIndent() << "case " << pType->getIndex() << ": //" << pType->getFriendlyName() << "\n";
        os << generator.getIndent() << "{\n";
        generator.pushIndent();
        for( const Instruction* pChild : ins.getChildren() )
        {
            generate( *pChild, generator, os );
        }
        generator.popIndent();
        os << generator.getIndent() << "}\n";
        os << generator.getIndent() << "break;\n";
    }
    void generate( const CallOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        const concrete::Action* pParent = dynamic_cast< const concrete::Action* >( ins.getConcreteType()->getParent() );
        VERIFY_RTE( pParent );
        const concrete::Allocator* pAllocator = pParent->getAllocator( ins.getConcreteType() );
        VERIFY_RTE( pAllocator );
        const interface::Context* pStaticType = ins.getConcreteType()->getContext();
        VERIFY_RTE( pStaticType );
        
        //acquire the object via the starter / allocator
        if( const interface::Abstract* pContext = dynamic_cast< const interface::Abstract* >( pStaticType ) )
        {
            THROW_RTE( "Invalid attempt to invoke abstract" );
        }
        else if( const interface::Event* pContext = dynamic_cast< const interface::Event* >( pStaticType ) )
        {
            const concrete::SingletonAllocator* pSingletonAllocator =
                dynamic_cast< const concrete::SingletonAllocator* >( pAllocator );
            const concrete::RangeAllocator* pRangeAllocator =
                dynamic_cast< const concrete::RangeAllocator* >( pAllocator );
                
            if( const concrete::NothingAllocator* pNothingAllocator =
                dynamic_cast< const concrete::NothingAllocator* >( pAllocator ) )
            {
                os << generator.getIndent() << getStaticType( pStaticType ) << " ref = eg::reference{ " << generator.getVarExpr( ins.getInstance() ) << 
                    ", " << ins.getConcreteType()->getIndex() << ", clock::cycle() };\n";
                os << generator.getIndent() << "::eg::Scheduler::signal_ref( ref );\n";
                os << generator.getIndent() << "return ref;\n";
            }
            else if( pSingletonAllocator || pRangeAllocator )
            {
                os << generator.getIndent() << getStaticType( pStaticType ) << " ref = " << ins.getConcreteType()->getName() << 
                    "_starter( " << generator.getVarExpr( ins.getInstance() ) << " );\n";
                os << generator.getIndent() << "if( ref )\n";
                os << generator.getIndent() << "{\n";
                os << generator.getIndent() << "    ::eg::Scheduler::signal_ref( ref );\n";
                os << generator.getIndent() << "    " << ins.getConcreteType()->getName() << "_stopper( ref.data.instance );\n";
                os << generator.getIndent() << "}\n";
                os << generator.getIndent() << "else\n";
                os << generator.getIndent() << "{\n";
                os << generator.getIndent() << "    ERR( \"Failed to allocate action: " << ins.getConcreteType()->getName() << "\" );\n";
                os << generator.getIndent() << "}\n";
                os << generator.getIndent() << "return ref;\n";
            }
            else
            {
                THROW_RTE( "Unknown allocator type" );
            }
        }
        else if( const interface::Function* pContext = dynamic_cast< const interface::Function* >( pStaticType ) )
        {
            VERIFY_RTE( dynamic_cast< const concrete::NothingAllocator* >( pAllocator ) );
            //directly invoke the function
            os << generator.getIndent() << getStaticType( pStaticType ) << " ref = eg::reference{ " << generator.getVarExpr( ins.getInstance() ) << 
                ", " << ins.getConcreteType()->getIndex() << ", clock::cycle() };\n";
            os << generator.getIndent() << "return ref( args... );\n";
        }
        else
        {
            const interface::Action*    pActionContext  = dynamic_cast< const interface::Action* >( pStaticType );
            const interface::Object*    pObjectContext  = dynamic_cast< const interface::Object* >( pStaticType );
            const interface::Link*      pLinkContext    = dynamic_cast< const interface::Link* >( pStaticType );
            
            if( pActionContext || pObjectContext || pLinkContext )
            {
                os << generator.getIndent() << getStaticType( pStaticType ) << " ref = " << ins.getConcreteType()->getName() << 
                    "_starter( " << generator.getVarExpr( ins.getInstance() ) << " );\n";
                    
                if( pStaticType->hasDefinition() )
                {
                    os << generator.getIndent() << "if( ref )\n";
                    os << generator.getIndent() << "{\n";
                    os << generator.getIndent() << "    ::eg::Scheduler::call( ref, &" << ins.getConcreteType()->getName() << "_stopper, args... );\n";
                    os << generator.getIndent() << "}\n";
                    os << generator.getIndent() << "else\n";
                    os << generator.getIndent() << "{\n";
                    os << generator.getIndent() << "    ERR( \"Failed to allocate action: " << ins.getConcreteType()->getName() << "\" );\n";
                    os << generator.getIndent() << "}\n";
                    os << generator.getIndent() << "return ref;\n";
                }
                else
                {
                    os << generator.getIndent() << "if( ref )\n";
                    os << generator.getIndent() << "{\n";
                    os << generator.getIndent() << "    ::eg::Scheduler::allocated_ref( ref.data, &" << ins.getConcreteType()->getName() << "_stopper );\n";
                    os << generator.getIndent() << "}\n";
                    os << generator.getIndent() << "else\n";
                    os << generator.getIndent() << "{\n";
                    os << generator.getIndent() << "    ERR( \"Failed to allocate action: " << ins.getConcreteType()->getName() << "\" );\n";
                    os << generator.getIndent() << "}\n";
                    os << generator.getIndent() << "return ref;\n";
                }
            }
            else
            {
                THROW_RTE( "Unknown abstract type" );
            }
        }
    }
    void generate( const StartOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        THROW_RTE( "TODO StartOperation" );
    }
    void generate( const StopOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        const concrete::Dimension_Generated* pReferenceDimension = ins.getConcreteType()->getReference();
        VERIFY_RTE( pReferenceDimension );
        os << generator.getIndent() << "::eg::Scheduler::stop_ref( " << 
            *generator.getDimension( pReferenceDimension, generator.getVarExpr( ins.getInstance() ) ) << ".data );\n";
    }
    void generate( const PauseOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        const concrete::Dimension_Generated* pReferenceDimension = ins.getConcreteType()->getReference();
        VERIFY_RTE( pReferenceDimension );
        os << generator.getIndent() << "if( " << 
            *generator.getDimension( ins.getConcreteType()->getState(), generator.getVarExpr( ins.getInstance() ) ) <<
                " == " << getActionState( action_running ) << " )\n";
        os << generator.getIndent() << "{\n";
        os << generator.getIndent() << "    ::eg::Scheduler::pause_ref( " << 
            *generator.getDimension( pReferenceDimension, generator.getVarExpr( ins.getInstance() ) ) << ".data );\n";
        os << generator.getIndent() << "    " <<
            *generator.getDimension( ins.getConcreteType()->getState(), generator.getVarExpr( ins.getInstance() ) ) << 
                " = " << getActionState( action_paused ) << ";\n";
        os << generator.getIndent() << "}\n";
    }
    void generate( const ResumeOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        const concrete::Dimension_Generated* pReferenceDimension = ins.getConcreteType()->getReference();
        VERIFY_RTE( pReferenceDimension );
        os << generator.getIndent() << "if( " << 
            *generator.getDimension( ins.getConcreteType()->getState(), generator.getVarExpr( ins.getInstance() ) ) <<
                " == " << getActionState( action_paused ) << " )\n";
        os << generator.getIndent() << "{\n";
        os << generator.getIndent() << "    ::eg::Scheduler::unpause_ref( " << 
            *generator.getDimension( pReferenceDimension, generator.getVarExpr( ins.getInstance() ) ) << ".data );\n";
        os << generator.getIndent() << "    " <<
            *generator.getDimension( ins.getConcreteType()->getState(), generator.getVarExpr( ins.getInstance() ) ) << 
                " = " << getActionState( action_running ) << ";\n";
        os << generator.getIndent() << "}\n";
    }
    void generate( const DoneOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        const concrete::Action::AllocatorMap& allocators = ins.getConcreteType()->getAllocators();
        
        for( concrete::Action::AllocatorMap::const_iterator 
            i = allocators.begin(),
            iEnd = allocators.end(); i!=iEnd; ++i )
        {
            if( const concrete::NothingAllocator* pNothingAllocator = dynamic_cast< const concrete::NothingAllocator* >( i->second ) )
            {
                //do nothing
            }
            else if( const concrete::SingletonAllocator* pSingletonAllocator = dynamic_cast< const concrete::SingletonAllocator* >( i->second ) )
            {
                //test the active state of the single instance
                const concrete::Action* pAllocated = pSingletonAllocator->getAllocated();
                os << generator.getIndent() << "if( " <<
                    *generator.getDimension( pAllocated->getState(), generator.getVarExpr( ins.getInstance() ) ) << 
                    " != " << getActionState( action_stopped ) << " ) return false;\n";
            }
            else if( const concrete::RangeAllocator* pRangeAllocator = dynamic_cast< const concrete::RangeAllocator* >( i->second ) )
            {
                const concrete::Dimension_Generated* pAllocatorData = pRangeAllocator->getAllocatorData();
                os << generator.getIndent() << "if( !" <<
                    *generator.getDimension( pAllocatorData, generator.getVarExpr( ins.getInstance() ) ) << ".empty() ) return false;\n";
            }
            else
            {
                THROW_RTE( "Unknown allocator type" );
            }
        }
        
        os << generator.getIndent() << "return true;\n";
    }
    void generate( const WaitActionOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        THROW_RTE( "TODO WaitActionOperation" );
        os << generator.getIndent() << "return " <<
            *generator.getDimension( ins.getConcreteType()->getReference(), generator.getVarExpr( ins.getInstance() ) ) << ";\n";
        
    }
    void generate( const WaitDimensionOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        THROW_RTE( "TODO WaitDimensionOperation" );
        os << generator.getIndent() << "return " << 
            *generator.getDimension( ins.getConcreteType(), generator.getVarExpr( ins.getInstance() ) ) << ";\n";
    }
    void generate( const GetActionOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        VERIFY_RTE( ins.getConcreteType()->getReference() );
        os << generator.getIndent() << "return " <<
            *generator.getDimension( ins.getConcreteType()->getReference(), generator.getVarExpr( ins.getInstance() ) ) << ";\n";
        
    }
    void generate( const GetDimensionOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        os << generator.getIndent() << "return " << 
            *generator.getDimension( ins.getConcreteType(), generator.getVarExpr( ins.getInstance() ) ) << ";\n";
    }
    void generate( const ReadOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        os << generator.getIndent() << "return " << 
            *generator.getDimension( ins.getConcreteType(), generator.getVarExpr( ins.getInstance() ) ) << ";\n";
    }
    void generate( const WriteOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        os << generator.getIndent() << 
            *generator.getDimension( ins.getConcreteType(), generator.getVarExpr( ins.getInstance() ) ) << " = value;\n";
        
        const concrete::Action* pReturnType = ins.getInstance()->getConcreteType();
        if( pReturnType->getReference() )
        {
            os << generator.getIndent() << "return " << 
                *generator.getDimension( pReturnType->getReference(), generator.getVarExpr( ins.getInstance() ) ) << ";\n";
        }
        else
        {
            os << generator.getIndent() << getStaticType( pReturnType->getContext() ) << " ref = eg::reference{ " << generator.getVarExpr( ins.getInstance() ) << 
                ", " << ins.getConcreteType()->getIndex() << ", clock::cycle() };\n";
            os << generator.getIndent() << "return ref;\n";
        }         
    }
	
    void generate( const WriteLinkOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        const concrete::Dimension_User* pLinkDimension  = ins.getConcreteType();
        const LinkGroup* pLinkGroup                     = ins.getLinkGroup();
		
        const concrete::Action* pLink =
            dynamic_cast< const concrete::Action* >( ins.getConcreteType()->getParent() );
        VERIFY_RTE( pLink );
                
        //is the value different to the current base
        os << generator.getIndent() << "if( " << 
            *generator.getDimension( ins.getConcreteType(), generator.getVarExpr( ins.getInstance() ) ) << " != value )\n";
        os << generator.getIndent() << "{\n";
        generator.pushIndent();
            
        //if current base is NOT null then invoke the breaker
        {
            os << generator.getIndent() << "if( " << 
                *generator.getDimension( ins.getConcreteType(), generator.getVarExpr( ins.getInstance() ) ) << ".data.timestamp != eg::INVALID_TIMESTAMP )\n";
            os << generator.getIndent() << "{\n";
            os << generator.getIndent() << "  " << pLink->getName() << "_breaker( " << generator.getVarExpr( ins.getInstance() ) << " );\n";
            os << generator.getIndent() << "}\n";
        }
            
        //assign the new base
        os << generator.getIndent() << 
            *generator.getDimension( ins.getConcreteType(), generator.getVarExpr( ins.getInstance() ) ) << " = value;\n";
            
        const concrete::Dimension_Generated* pLinkerReferenceDimension = pLink->getReference();
        VERIFY_RTE( pLinkerReferenceDimension );
            
        //set the new link reference for the new link
        {
            os << generator.getIndent() << "switch( value.data.type )\n";
            os << generator.getIndent() << "{\n";
            generator.pushIndent();
            
            for( concrete::Action* pTargetType : pLinkGroup->getTargets() )
            {
                if( pTargetType->getObject() )
                {
                    //work out the generated dimension for the link group within this target type
                    LinkGroup::LinkRefMap::const_iterator iFind =
                        pLinkGroup->getDimensionMap().find( pTargetType );
                    VERIFY_RTE( iFind != pLinkGroup->getDimensionMap().end() );
                    const concrete::Dimension_Generated* pLinkBackReference = iFind->second;
                    os << generator.getIndent() << "case " << pTargetType->getIndex() << ":\n";
                    {
                        generator.pushIndent();
                        //before assigning the link back reference - test it to see if existing link to it
                        {
                            os << generator.getIndent() << "if( " << 
                                *generator.getDimension( pLinkBackReference, "value.data.instance" ) << ".timestamp != eg::INVALID_TIMESTAMP )\n";
                            os << generator.getIndent() << "{\n";
                            
                            os << generator.getIndent() << "  switch( " << *generator.getDimension( pLinkBackReference, "value.data.instance" ) << ".type )\n";
                            os << generator.getIndent() << "  {\n";
                            
                            for( const concrete::Action* pLink : pLinkGroup->getConcreteLinks() )
                            {
                            os << generator.getIndent() << "    case " << pLink->getIndex() << ": ::eg::Scheduler::stop_ref( " << 
                                *generator.getDimension( pLinkBackReference, "value.data.instance" ) << " ); break;\n";
                            }
                            os << generator.getIndent() << "    default: ERR( \"Unknown link type\" ); break;\n";
                            os << generator.getIndent() << "  }\n";
                            os << generator.getIndent() << "}\n";
                            
                        }
                        
                        //finally assign the back reference to the link
                        {
                            os << generator.getIndent() << *generator.getDimension( pLinkBackReference, "value.data.instance" ) << " = " <<
                                *generator.getDimension( pLinkerReferenceDimension, generator.getVarExpr( ins.getInstance() ) ) << ".data;\n";
                                
                            //increment the reference count
                            {
                                const concrete::Action* pObject = pTargetType->getObject();
                                VERIFY_RTE( pObject );
                                if( !pObject->getContext()->isMainExecutable() )
                                {
                                    const concrete::Dimension_Generated* pLinkRefCount = pObject->getLinkRefCount();
                                    VERIFY_RTE( pLinkRefCount );
                                    
                                    std::ostringstream osDomain;
                                    {
                                        const int iDomainFactor = pObject->getObjectDomainFactor();
                                        if( iDomainFactor == 1 )
                                            osDomain << "value.data.instance";
                                        else
                                            osDomain << "value.data.instance / " << iDomainFactor;
                                    }
                                    os << generator.getIndent() << *generator.getDimension( pLinkRefCount, osDomain.str().c_str() ) << 
                                        " = " << *generator.getDimension( pLinkRefCount, osDomain.str().c_str() ) << " + 1;\n";
                                }
                            }
                                
                            os << generator.getIndent() << "break;\n";
                        }
                        generator.popIndent();
                    }
                }
            }
            
            os << generator.getIndent() << "case 0: break;\n";
            os << generator.getIndent() << "default: ERR( \"Invalid link\" );\n";
            generator.popIndent();
            os << generator.getIndent() << "}\n";
            
            
        }
            
        generator.popIndent();
        os << generator.getIndent() << "}\n";
        
        const concrete::Action* pReturnType = ins.getInstance()->getConcreteType();
        os << generator.getIndent() << "return " << 
            *generator.getDimension( pReturnType->getReference(), generator.getVarExpr( ins.getInstance() ) ) << ";\n";
    }
    
    void generate( const RangeOperation& ins, CodeGenerator& generator, std::ostream& os )
    {
        //nothing to do here - the operation is aggregated up by the enumeration instruction
    }
    
    void generate( const Instruction& ins, CodeGenerator& generator, std::ostream& os )
    {
        switch( ins.getType() )
        {
            case eParentDerivationInstruction:       generate( dynamic_cast<  const ParentDerivationInstruction           & >( ins ) , generator, os ); break;
            case eChildDerivationInstruction:        generate( dynamic_cast<  const ChildDerivationInstruction            & >( ins ) , generator, os ); break;
            case eEnumDerivationInstruction:         generate( dynamic_cast<  const EnumDerivationInstruction             & >( ins ) , generator, os ); break;
            case eEnumerationInstruction:            generate( dynamic_cast<  const EnumerationInstruction                & >( ins ) , generator, os ); break;
            case eDimensionReferenceReadInstruction: generate( dynamic_cast<  const DimensionReferenceReadInstruction     & >( ins ) , generator, os ); break;
            case eMonoReferenceInstruction:          generate( dynamic_cast<  const MonomorphicReferenceInstruction       & >( ins ) , generator, os ); break;
            case ePolyReferenceInstruction:          generate( dynamic_cast<  const PolymorphicReferenceBranchInstruction & >( ins ) , generator, os ); break;
            case ePolyCaseInstruction:               generate( dynamic_cast<  const PolymorphicCaseInstruction            & >( ins ) , generator, os ); break;
                                                                                                                            
            case eCallOperation:                     generate( dynamic_cast<  const CallOperation                         & >( ins ) , generator, os ); break;
            case eStartOperation:                    generate( dynamic_cast<  const StartOperation                        & >( ins ) , generator, os ); break;
            case eStopOperation:                     generate( dynamic_cast<  const StopOperation                         & >( ins ) , generator, os ); break;
            case ePauseOperation:                    generate( dynamic_cast<  const PauseOperation                        & >( ins ) , generator, os ); break;
            case eResumeOperation:                   generate( dynamic_cast<  const ResumeOperation                       & >( ins ) , generator, os ); break;
            case eDoneOperation:                     generate( dynamic_cast<  const DoneOperation                         & >( ins ) , generator, os ); break;
            case eWaitActionOperation:               generate( dynamic_cast<  const WaitActionOperation                   & >( ins ) , generator, os ); break;
            case eWaitDimensionOperation:            generate( dynamic_cast<  const WaitDimensionOperation                & >( ins ) , generator, os ); break;
            case eGetActionOperation:                generate( dynamic_cast<  const GetActionOperation                    & >( ins ) , generator, os ); break;
            case eGetDimensionOperation:             generate( dynamic_cast<  const GetDimensionOperation                 & >( ins ) , generator, os ); break;
            case eReadOperation:                     generate( dynamic_cast<  const ReadOperation                         & >( ins ) , generator, os ); break;
            case eWriteOperation:                    generate( dynamic_cast<  const WriteOperation                        & >( ins ) , generator, os ); break;
            case eWriteLinkOperation:                generate( dynamic_cast<  const WriteLinkOperation                    & >( ins ) , generator, os ); break;
            case eRangeOperation:                    generate( dynamic_cast<  const RangeOperation                        & >( ins ) , generator, os ); break;
                              
            default:
            case eRootInstruction:                      
            case eFailureInstruction:                   
            case eEliminationInstruction:               
            case ePruneInstruction:   
            
            case TOTAL_AST_TYPES:
            case eInstanceVariable:
            case eReferenceVariable:
            case eDimensionVariable:
            case eContextVariable:
                THROW_RTE( "Invalid instruction type ");
            
            
        }
    }

    void generateInstructions( std::ostream& os, PrinterFactory& printerFactory, const RootInstruction* pRootInstruction, const Layout& layout )
    {
        CodeGenerator codeGenerator( layout, 2, "eg::Event()", printerFactory );
        generate( *pRootInstruction, codeGenerator, os );
    }


}