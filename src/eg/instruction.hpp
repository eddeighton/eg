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

#ifndef INSTRUCTION_08_05_2019
#define INSTRUCTION_08_05_2019

#include "concrete.hpp"

#include "eg/common.hpp"

#include "eg_runtime/eg_runtime.hpp"

#include "io/indexed_object.hpp"

#include <map>

namespace eg
{
class Variable;

struct RangeDescription
{
    struct SubRange
    {
        TypeID runtimeType;
        Instance begin, end;
    };
    std::vector< SubRange > ranges;
};

class RuntimeEvaluator
{
public:
    virtual ~RuntimeEvaluator(){}
    
    
    using VariableValueMap = std::map< const Variable*, reference >;
    
    void initialise()
    {
        m_variables.clear();
    }
    
    inline void setVarValue( const Variable* pVariable, const reference& value )
    {
        VariableValueMap::const_iterator iFind = m_variables.find( pVariable );
        VERIFY_RTE( iFind == m_variables.end() );
        m_variables.insert( std::make_pair( pVariable, value ) );
    }
    inline const reference& getVarValue( const Variable* pVariable ) const
    {
        VariableValueMap::const_iterator iFind = m_variables.find( pVariable );
        VERIFY_RTE( iFind != m_variables.end() );
        return iFind->second;
    }
    
    virtual reference dereferenceDimension( const reference& action, const TypeID& dimensionType ) = 0;
    virtual void doRead(    const reference& reference, TypeID dimensionType ) = 0;
    virtual void doWrite(   const reference& reference, TypeID dimensionType ) = 0;
    virtual void doCall(   const reference& reference, TypeID dimensionType ) = 0;
    virtual void doStart(   const reference& reference, TypeID dimensionType ) = 0;
    virtual void doStop(    const reference& reference ) = 0;
    virtual void doPause(   const reference& reference ) = 0;
    virtual void doResume(  const reference& reference ) = 0;
    virtual void doDone(    const reference& reference ) = 0;
    virtual void doWaitAction(    const reference& reference ) = 0;
    virtual void doWaitDimension(    const reference& reference, TypeID dimensionType ) = 0;
    virtual void doGetAction(    const reference& reference ) = 0;
    virtual void doGetDimension(    const reference& reference, TypeID dimensionType ) = 0;
    virtual void doRange( const RangeDescription& range ) = 0;
    
private:
    VariableValueMap m_variables;
};

enum ASTElementType //for serialisation
{
    eInstanceVariable,
    eReferenceVariable,
    eDimensionVariable,
    eContextVariable,
    
    eRootInstruction,
    eParentDerivationInstruction,
    eChildDerivationInstruction,
    eEnumDerivationInstruction,
    eEnumerationInstruction,
    eFailureInstruction,
    eEliminationInstruction,
    ePruneInstruction,
    eDimensionReferenceReadInstruction,
    eMonoReferenceInstruction,
    ePolyReferenceInstruction,
    ePolyCaseInstruction,
    
    eCallOperation,
    eStartOperation,
    eStopOperation,
    ePauseOperation,
    eResumeOperation,
    eDoneOperation,
    eWaitActionOperation,
    eWaitDimensionOperation,
    eGetActionOperation,
    eGetDimensionOperation,
    eReadOperation,
    eWriteOperation,
    eRangeOperation,
    
    TOTAL_AST_TYPES
};


class ASTSerialiser;

class ASTElement
{
public:
    virtual ~ASTElement()
    {
    }
    virtual ASTElementType getType() const = 0;
    virtual void load( ASTSerialiser& serialiser, Loader& loader ) = 0;
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const = 0;
};

class ASTSerialiser
{
    using ASTElementMap = std::map< const ASTElement*, std::size_t >;
    using ASTElementMapBack = std::map< std::size_t, const ASTElement* >;
public:
    
    void load( Loader& loader, const ASTElement*& pElement );
    void store( Storer& storer, const ASTElement* pElement );
    
    template< class TDerived >
    inline void load( Loader& loader, TDerived*& pDerived )
    {
        const ASTElement* pElement = nullptr;
        load( loader, pElement );
        if( pElement )
        {
            ASTElement* pElementNonConst = const_cast< ASTElement* >( pElement );
            pDerived = dynamic_cast< TDerived* >( pElementNonConst );
            VERIFY_RTE( pDerived );
        }
        else
        {
            pDerived = nullptr;
        }
    }
    
private:
    ASTElementMap m_addressMap;
    ASTElementMapBack m_addressMapBack;
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class Variable : public ASTElement
{
public:
    Variable()
        :   m_pParent( nullptr )
    {
    }
    Variable( const Variable* pParent )
        :   m_pParent( pParent )
    {
    }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    
    const Variable* m_pParent;
};

class InstanceVariable : public Variable
{
public:
    InstanceVariable()
    {
    }
    InstanceVariable( const Variable* pParent, const concrete::Action* pType )
        :   Variable( pParent ),
            m_pType( pType )
    {
    }

    virtual ASTElementType getType() const { return eInstanceVariable; }
    
    const concrete::Action* getConcreteType() const { return m_pType; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    
    const concrete::Action* m_pType;
};

class ReferenceVariable : public Variable
{
public:
    ReferenceVariable()
    {
    }
    ReferenceVariable( const Variable* pParent, std::vector< const concrete::Element* >& types )
        :   Variable( pParent ),
            m_types( types )
    {
    }
    
    virtual ASTElementType getType() const { return eReferenceVariable; }
    const std::vector< const concrete::Element* >& getTypes() const { return m_types; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    
    std::vector< const concrete::Element* > m_types;
};

class DimensionReferenceVariable : public ReferenceVariable
{
public:
    DimensionReferenceVariable()
    {
    }
    DimensionReferenceVariable( const Variable* pParent, std::vector< const concrete::Element* >& types )
        :   ReferenceVariable( pParent, types )
    {
    }
    virtual ASTElementType getType() const { return eDimensionVariable; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
};

class ContextVariable : public ReferenceVariable
{
public:
    ContextVariable( )
    {
    }
    ContextVariable( std::vector< const concrete::Element* >& types )
        :   ReferenceVariable( nullptr, types )
    {
    }
    virtual ASTElementType getType() const { return eContextVariable; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
};

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
class Operation;
class CodeGenerator;

class Instruction : public ASTElement
{
public:
    Instruction()
    {
    }
    ~Instruction()
    {
        for( Instruction* pChild : m_children )
            delete pChild;
    }
    using Vector = std::vector< Instruction* >;
    
    void append( Instruction* pInstruction ) { m_children.push_back( pInstruction ); }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    
public:
    enum Elimination
    {
        eSuccess,
        eFailure,
        eAmbiguous
    };
    Elimination eliminate();
    Elimination secondStageElimination( const std::vector< const Operation* >& candidateOperations );
    void getOperations( std::vector< const Operation* >& operations ) const;
    virtual int setReturnTypes( const std::vector< const interface::Element* >& targets );
    virtual void setMaxRanges( int iMaxRanges );
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const = 0;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const = 0;
protected:
    Vector m_children;
};

class RootInstruction : public Instruction
{
public:
    RootInstruction()
        :   m_pContext( nullptr )
    {
    }
    RootInstruction( ContextVariable* pContext )
        :   m_pContext( pContext )
    {
    }
    
    virtual ASTElementType getType() const { return eRootInstruction; }
    
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void setMaxRanges( int iMaxRanges );
    
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
    
    const ContextVariable* getContextVariable() const { return m_pContext; }
    int getMaxRanges() const { return m_iMaxRanges; }
    
protected:
    ContextVariable* m_pContext = nullptr;
    int m_iMaxRanges;
};

class ParentDerivationInstruction : public Instruction
{
public:
    ParentDerivationInstruction(){}
    ParentDerivationInstruction( InstanceVariable* pFrom, InstanceVariable* pTo )
        :   m_pFrom( pFrom ),
            m_pTo( pTo )
    {
        
    }
    
    virtual ASTElementType getType() const { return eParentDerivationInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pFrom = nullptr;
    InstanceVariable* m_pTo = nullptr;
};

class ChildDerivationInstruction : public Instruction
{
public:
    ChildDerivationInstruction(){}
    ChildDerivationInstruction( InstanceVariable* pFrom, InstanceVariable* pTo )
        :   m_pFrom( pFrom ),
            m_pTo( pTo )
    {
        
    }
    
    virtual ASTElementType getType() const { return eChildDerivationInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pFrom = nullptr;
    InstanceVariable* m_pTo = nullptr;
};

class EnumDerivationInstruction : public Instruction
{
public:
    EnumDerivationInstruction(){}
    EnumDerivationInstruction( InstanceVariable* pFrom, InstanceVariable* pTo )
        :   m_pFrom( pFrom ),
            m_pTo( pTo )
    {
        
    }
    
    virtual ASTElementType getType() const { return eEnumDerivationInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pFrom = nullptr;
    InstanceVariable* m_pTo = nullptr;
};

class EnumerationInstruction : public Instruction
{
public:
    EnumerationInstruction(){}
    EnumerationInstruction( InstanceVariable* pContext )
        :   m_pContext( pContext ),
            m_iMaxRanges( 1 )
    {
        
    }
    
    virtual ASTElementType getType() const { return eEnumerationInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
    virtual int setReturnTypes( const std::vector< const interface::Element* >& targets );
    virtual void setMaxRanges( int iMaxRanges );
private:
    InstanceVariable* m_pContext = nullptr;
    std::vector< const interface::Element* > m_returnTypes;
    int m_iMaxRanges;
};

class FailureInstruction : public Instruction
{
public:
    virtual ASTElementType getType() const { return eFailureInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const { THROW_RTE( "Unreachable" ); }
    virtual void evaluate( RuntimeEvaluator& evaluator ) const { THROW_RTE( "Unreachable" ); }
};

class EliminationInstruction : public Instruction
{
public:
    virtual ASTElementType getType() const { return eEliminationInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const { THROW_RTE( "Unreachable" ); }
    virtual void evaluate( RuntimeEvaluator& evaluator ) const { THROW_RTE( "Unreachable" ); }
};

class PruneInstruction : public Instruction
{
public:
    virtual ASTElementType getType() const { return ePruneInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const { THROW_RTE( "Unreachable" ); }
    virtual void evaluate( RuntimeEvaluator& evaluator ) const { THROW_RTE( "Unreachable" ); }
};

class DimensionReferenceReadInstruction : public Instruction
{
public:
    DimensionReferenceReadInstruction(){}
    DimensionReferenceReadInstruction( InstanceVariable* pInstance, 
                DimensionReferenceVariable* pReference, const concrete::Dimension* pDimension )
        :   m_pInstance( pInstance ),
            m_pReference( pReference ),
            m_pDimension( pDimension )
    {
        
    }
    virtual ASTElementType getType() const { return eDimensionReferenceReadInstruction; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    DimensionReferenceVariable* m_pReference = nullptr;
    const concrete::Dimension* m_pDimension = nullptr;
};

class MonomorphicReferenceInstruction : public Instruction 
{
public:
    MonomorphicReferenceInstruction(){}
    MonomorphicReferenceInstruction( ReferenceVariable* pFrom, InstanceVariable* pInstance )
        :   m_pFrom( pFrom ),
            m_pInstance( pInstance )
    {
        
    }
    virtual ASTElementType getType() const { return eMonoReferenceInstruction; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
    
    ReferenceVariable* m_pFrom = nullptr;
    InstanceVariable* m_pInstance = nullptr;
};

class PolymorphicReferenceBranchInstruction : public Instruction
{
public:
    PolymorphicReferenceBranchInstruction()
    {
    }
    PolymorphicReferenceBranchInstruction( ReferenceVariable* pFrom )
        :   m_pFrom( pFrom )
    {
    }
    
    virtual ASTElementType getType() const { return ePolyReferenceInstruction; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    ReferenceVariable* m_pFrom = nullptr;
};

class PolymorphicCaseInstruction : public Instruction
{
public:
    PolymorphicCaseInstruction(){}
    PolymorphicCaseInstruction( ReferenceVariable* pReference, InstanceVariable* pTo )
        :   m_pReference( pReference ),
            m_pTo( pTo )
    {
    }
    virtual ASTElementType getType() const { return ePolyCaseInstruction; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
public:
    const InstanceVariable* getTarget() const { return m_pTo; }
private:
    ReferenceVariable* m_pReference = nullptr;
    InstanceVariable* m_pTo = nullptr;
};

class Operation : public Instruction
{
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
public:
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const = 0;
};

class CallOperation : public Operation
{
public:
    CallOperation(){}
    CallOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eCallOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
public:
    const interface::Action* getInterfaceType() const { return m_pInterface; }
    const concrete::Action* getConcreteType() const { return m_pTarget; }
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class StartOperation : public Operation
{
public:
    StartOperation(){}
    StartOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eStartOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
public:
    const interface::Action* getInterfaceType() const { return m_pInterface; }
    const concrete::Action* getConcreteType() const { return m_pTarget; }
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class StopOperation : public Operation
{
public:
    StopOperation(){}
    StopOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eStopOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class PauseOperation : public Operation
{
public:
    PauseOperation(){}
    PauseOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return ePauseOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class ResumeOperation : public Operation
{
public:
    ResumeOperation(){}
    ResumeOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eResumeOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class DoneOperation : public Operation
{
public:
    DoneOperation(){}
    DoneOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eDoneOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;

};

class WaitActionOperation : public Operation
{
public:
    WaitActionOperation(){}
    WaitActionOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eWaitActionOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class WaitDimensionOperation : public Operation
{
public:
    WaitDimensionOperation(){}
    WaitDimensionOperation( InstanceVariable* pInstance, const interface::Dimension* pInterface, const concrete::Dimension_User* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eWaitDimensionOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Dimension* m_pInterface = nullptr;
    const concrete::Dimension_User* m_pTarget = nullptr;
};

class GetActionOperation : public Operation
{
public:
    GetActionOperation(){}
    GetActionOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eGetActionOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class GetDimensionOperation : public Operation
{
public:
    GetDimensionOperation(){}
    GetDimensionOperation( InstanceVariable* pInstance, const interface::Dimension* pInterface, const concrete::Dimension_User* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eGetDimensionOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Dimension* m_pInterface = nullptr;
    const concrete::Dimension_User* m_pTarget = nullptr;
};

class ReadOperation : public Operation
{
public:
    ReadOperation(){}
    ReadOperation( InstanceVariable* pInstance, const interface::Dimension* pInterface, const concrete::Dimension_User* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eReadOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Dimension* m_pInterface = nullptr;
    const concrete::Dimension_User* m_pTarget = nullptr;
};

class WriteOperation : public Operation
{
public:
    WriteOperation(){}
    WriteOperation( InstanceVariable* pInstance, const interface::Dimension* pInterface, const concrete::Dimension_User* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eWriteOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Dimension* m_pInterface = nullptr;
    const concrete::Dimension_User* m_pTarget = nullptr;

};

class RangeOperation : public Operation
{
public:
    RangeOperation(){}
    RangeOperation( InstanceVariable* pInstance, const interface::Action* pInterface, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pInterface( pInterface ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eRangeOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const interface::Element* >& abstracTypes ) const;
    virtual void generate( CodeGenerator& generator, std::ostream& os ) const;
    virtual void evaluate( RuntimeEvaluator& evaluator ) const;
public:
    const InstanceVariable* getInstance() const { return m_pInstance; }
    const concrete::Action* getTarget() const { return m_pTarget; }
private:
    InstanceVariable* m_pInstance = nullptr;
    const interface::Action* m_pInterface = nullptr;
    const concrete::Action* m_pTarget = nullptr;

};
    
}

#endif //INSTRUCTION_08_05_2019