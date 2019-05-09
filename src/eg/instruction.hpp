
#ifndef INSTRUCTION_08_05_2019
#define INSTRUCTION_08_05_2019

#include "concrete.hpp"

#include "io/indexed_object.hpp"

#include <map>

namespace eg
{
    

enum ASTElementType //for serialisation
{
    eInstanceVariable,
    eReferenceVariable,
    eDimensionVariable,
    eContextVariable,
    
    eRootInstruction,
    eParentDerivationInstruction,
    eChildDerivationInstruction,
    eFailureInstruction,
    eEliminationInstruction,
    eMonoReferenceInstruction,
    ePolyReferenceInstruction,
    ePolyCaseInstruction,
    
    eStartOperation,
    eStopOperation,
    ePauseOperation,
    eResumeOperation,
    eGetActionOperation,
    eReadOperation,
    eWriteOperation,
    
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
    InstanceVariable( const Variable* pParent, const concrete::Element* pType )
        :   Variable( pParent ),
            m_pType( pType )
    {
    }

    virtual ASTElementType getType() const { return eInstanceVariable; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    
    const concrete::Element* m_pType;
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
    void getOperations( std::vector< const Operation* >& operations ) const;
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
    
    
protected:
    ContextVariable* m_pContext = nullptr;
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
    
    InstanceVariable* m_pFrom = nullptr;
    InstanceVariable* m_pTo = nullptr;
};

class FailureInstruction : public Instruction
{
public:
    virtual ASTElementType getType() const { return eFailureInstruction; }
    
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
};

class EliminationInstruction : public Instruction
{
public:
    virtual ASTElementType getType() const { return eEliminationInstruction; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
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
    
    ReferenceVariable* m_pFrom = nullptr;
};

class PolymorphicCaseInstruction : public Instruction
{
public:
    PolymorphicCaseInstruction(){}
    PolymorphicCaseInstruction( InstanceVariable* pTo )
        :   m_pTo( pTo )
    {
    }
    virtual ASTElementType getType() const { return ePolyCaseInstruction; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    
    InstanceVariable* m_pTo = nullptr;
};

class Operation : public Instruction
{
public:
    
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const = 0;
};

class StartOperation : public Operation
{
public:
    StartOperation(){}
    StartOperation( InstanceVariable* pInstance, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eStartOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class StopOperation : public Operation
{
public:
    StopOperation(){}
    StopOperation( InstanceVariable* pInstance, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eStopOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class PauseOperation : public Operation
{
public:
    PauseOperation(){}
    PauseOperation( InstanceVariable* pInstance, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return ePauseOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class ResumeOperation : public Operation
{
public:
    ResumeOperation(){}
    ResumeOperation( InstanceVariable* pInstance, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eResumeOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class GetActionOperation : public Operation
{
public:
    GetActionOperation(){}
    GetActionOperation( InstanceVariable* pInstance, const concrete::Action* pTarget )
        :   m_pInstance( pInstance ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eGetActionOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    const concrete::Action* m_pTarget = nullptr;
};

class ReadOperation : public Operation
{
public:
    ReadOperation(){}
    ReadOperation( InstanceVariable* pInstance, const concrete::Dimension_User* pTarget )
        :   m_pInstance( pInstance ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eReadOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    const concrete::Dimension_User* m_pTarget = nullptr;
};

class WriteOperation : public Operation
{
public:
    WriteOperation(){}
    WriteOperation( InstanceVariable* pInstance, const concrete::Dimension_User* pTarget )
        :   m_pInstance( pInstance ),
            m_pTarget( pTarget )
    {
    }
    virtual ASTElementType getType() const { return eWriteOperation; }
protected:
    virtual void load( ASTSerialiser& serialiser, Loader& loader );
    virtual void store( ASTSerialiser& serialiser, Storer& storer ) const;
    virtual void getTargetAbstractTypes( std::vector< const abstract::Element* >& abstracTypes ) const;
    
    InstanceVariable* m_pInstance = nullptr;
    const concrete::Dimension_User* m_pTarget = nullptr;

};


}

#endif //INSTRUCTION_08_05_2019