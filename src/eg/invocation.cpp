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
    std::ostream& operator<<( std::ostream& os, const InvocationSolution::InvocationID& invocationID )
    {
        {
            const InvocationSolution::Context& context = 
                std::get< InvocationSolution::Context >( invocationID );
            if( context.size() > 1 )
                os << "Var< ";
            for( const abstract::Element* pContextElement : context )
            {
                if( pContextElement != context.front() )
                    os << ", ";
                std::vector< const abstract::Element* > path = getPath( pContextElement );
                for( const abstract::Element* pPathElement : path )
                {
                    if( pPathElement != path.front() )
                        os << "::";
                    os << pPathElement->getIdentifier();
                }
            }
            if( context.size() > 1 )
                os << " >";
            else if( context.empty() )
                os << "no context";
        }
        {
            os << "[";
            const InvocationSolution::TypePath& typePath = 
                std::get< InvocationSolution::TypePath >( invocationID );
            bool bFirst = true;
            for( const std::vector< const abstract::Element* >& step : typePath )
            {
                if( bFirst ) 
                    bFirst = false;
                else
                    os << ".";
                if( !step.empty() )
                    os << step.front()->getIdentifier();
                else
                    os << "_";
            }
            os << "]";
        }
        {
            os << getOperationString( std::get< OperationID >( invocationID ) );
        }
        
        return os;
    }

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
        loader.loadObjectVector( std::get< Context >( m_invocationID ) );
        loader.loadObjectVectorVector( std::get< TypePath >( m_invocationID ) );
        loader.load( std::get< OperationID >( m_invocationID ) );
        loader.load( m_implicitTypePath );
        loader.loadObjectVector( m_finalPathTypes );
        loader.loadObjectVector( m_targetTypes );
        loader.loadObjectVector( m_contextElements );
        loader.loadObjectVectorVector( m_concreteTypePath );
        
        {
            ASSERT( !m_pRoot );
            ASTSerialiser astSerialiser;
            m_pRoot = new RootInstruction;
            m_pRoot->load( astSerialiser, loader );
        }
        
    }
    void InvocationSolution::store( Storer& storer ) const
    {      
        storer.storeObjectVector( std::get< Context >( m_invocationID ) );
        storer.storeObjectVectorVector( std::get< TypePath >( m_invocationID ) );
        storer.store( std::get< OperationID >( m_invocationID ) );
        storer.store( m_implicitTypePath );
        storer.storeObjectVector( m_finalPathTypes );
        storer.storeObjectVector( m_targetTypes );
        storer.storeObjectVector( m_contextElements );
        storer.storeObjectVectorVector( m_concreteTypePath );
        
        {
            ASSERT( m_pRoot );
            ASTSerialiser astSerialiser;
            m_pRoot->store( astSerialiser, storer );
        }  
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
    
    
    
    class GenericOperationVisitor 
    {
        InvocationSolution& m_solution;
        const NameResolution& m_resolution;
        
    public:
        GenericOperationVisitor( InvocationSolution& solution, const NameResolution& resolution )
            :   m_solution( solution ),
                m_resolution( resolution )
        {
            
        }
        
        void commonRootDerivation( const concrete::Element* pFrom, const concrete::Element* pTo, 
            Instruction*& pInstruction, InstanceVariable*& pVariable )
        {
            if( pFrom && pFrom != pTo )
            {
                const concrete::Element* pCommon = findCommonRoot( pFrom, pTo );
                
                while( pFrom != pCommon )
                {
                    const concrete::Action* pParent = 
                        dynamic_cast< const concrete::Action* >( pFrom->getParent() );
                    ASSERT( pParent );
                    //generate parent derivation instruction
                    InstanceVariable* pInstanceVariable = 
                        new InstanceVariable( pVariable, pParent );
                    
                    ParentDerivationInstruction* pParentDerivation = 
                        new ParentDerivationInstruction( pVariable, pInstanceVariable );
                    pInstruction->append( pParentDerivation );
                    
                    pVariable = pInstanceVariable;
                    pInstruction = pParentDerivation;
                    pFrom = pFrom->getParent();
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
                    if( pIter->getLocalDomainSize() != 1 )
                    {
                        FailureInstruction* pFailure = new FailureInstruction;
                        pInstruction->append( pFailure );
                        pInstruction = pFailure;
                        return;
                    }
                    else
                    {
                        if( const concrete::Action* pAction = 
                            dynamic_cast< const concrete::Action* >( pIter ) )
                        {
                            InstanceVariable* pInstanceVariable = 
                                new InstanceVariable( pVariable, pAction );
                            
                            ChildDerivationInstruction* pChildDerivation = 
                                new ChildDerivationInstruction( pVariable, pInstanceVariable );
                            pInstruction->append( pChildDerivation );
                            
                            pVariable = pInstanceVariable;
                            pInstruction = pChildDerivation;
                        }
                        else if( const concrete::Dimension_User* pUserDimension =
                            dynamic_cast< const concrete::Dimension_User* >( pIter ) )
                        {
                            //just ignor this - always only want the parent instance variable..
                        }
                        else
                        {
                            THROW_RTE( "Unreachable" );
                        }
                    }
                }
            }
        }
        
        void buildTerminal( const Name& prev, const Name& current, 
            Instruction* pInstruction, InstanceVariable* pVariable )
        {
            const concrete::Element* pElement = current.getElement();
            if( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pElement ) )
            {
                switch( m_solution.getOperation() )
                {
                    case id_Imp_NoParams        :
                    case id_Imp_Params          :
                        {
                            //only derive the parent for the starter
                            commonRootDerivation( prev.getElement(), current.getElement()->getParent(), 
                                pInstruction, pVariable );
                            StartOperation* pStart = new StartOperation( pVariable, pAction );
                            pInstruction->append( pStart );
                        }
                        break;
                    case id_Get                 :
                        {
                            commonRootDerivation( prev.getElement(), current.getElement(), 
                                pInstruction, pVariable );
                            GetActionOperation* pGetAction = new GetActionOperation( pVariable, pAction );
                            pInstruction->append( pGetAction );
                        }
                        break;
                    case id_Update              :
                    case id_Old                 :
                        THROW_INVOCATION_EXCEPTION( "Invalid invocation operation on action: " << m_solution.getID() );
                        break;
                    case id_Stop                :
                        {
                            commonRootDerivation( prev.getElement(), current.getElement(), 
                                pInstruction, pVariable );
                            StopOperation* pStop = new StopOperation( pVariable, pAction );
                            pInstruction->append( pStop );
                        }
                        break;
                    case id_Pause               :
                        {
                            commonRootDerivation( prev.getElement(), current.getElement(), 
                                pInstruction, pVariable );
                            PauseOperation* pPause = new PauseOperation( pVariable, pAction );
                            pInstruction->append( pPause );
                        }
                        break;
                    case id_Resume              :
                        {
                            commonRootDerivation( prev.getElement(), current.getElement(), 
                                pInstruction, pVariable );
                            ResumeOperation* pResume = new ResumeOperation( pVariable, pAction );
                            pInstruction->append( pResume );
                        }
                        break;
                    case id_Defer               :
                        THROW_INVOCATION_EXCEPTION( "Defer not supported: " << m_solution.getID() );
                    default:
                        THROW_RTE( "Unreachable" );
                }
            }
            else if( const concrete::Dimension_User* pUserDimension = 
                dynamic_cast< const concrete::Dimension_User* >( pElement ) )
            {
                commonRootDerivation( prev.getElement(), current.getElement(), pInstruction, pVariable );
            
                switch( m_solution.getOperation() )
                {
                    case id_Imp_NoParams        :
                        {
                            ReadOperation* pRead = new ReadOperation( pVariable, pUserDimension );
                            pInstruction->append( pRead );
                        }
                        break;
                    case id_Imp_Params          :
                        {
                            WriteOperation* pWrite = new WriteOperation( pVariable, pUserDimension );
                            pInstruction->append( pWrite );
                        }
                        break;
                    case id_Get                 :
                    case id_Update              :
                    case id_Old                 :
                        break;
                    case id_Stop                :
                    case id_Pause               :
                    case id_Resume              :
                        THROW_INVOCATION_EXCEPTION( "Invalid invocation operation on dimension: " << m_solution.getID() );
                    case id_Defer               :
                        THROW_INVOCATION_EXCEPTION( "Defer not supported: " << m_solution.getID() );
                    default:
                        THROW_RTE( "Unreachable" );
                }
            }
            else
            {
                THROW_INVOCATION_EXCEPTION( "Invalid invocation target: " << m_solution.getID() );
            }
            
        }
            
        void buildPolymorphicCase( const Name& prev, const Name& current, 
            Instruction* pInstruction, ReferenceVariable* pVariable )
        {
            const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getElement() );
            ASSERT( pType );
            
            InstanceVariable* pInstance = new InstanceVariable( pVariable, pType );
            PolymorphicCaseInstruction* pCase = new PolymorphicCaseInstruction( pVariable, pInstance );
            pInstruction->append( pCase );
            pInstruction = pCase;
            
            if( current.getChildren().size() > 1U )
            {
                EliminationInstruction* pElim = new EliminationInstruction;
                pInstruction->append( pElim );
                pInstruction = pElim;
            }
            
            if( current.isTerminal() )
            {
                buildTerminal( prev, current, pInstruction, pInstance );
            }
            else
            {
                ASSERT( !current.getChildren().empty() );
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildGenerateName( current, next, pInstruction, pInstance );
                }
            }
        }
        
        void buildMonoDereference( const Name& prev, const Name& current,
            Instruction* pInstruction, ReferenceVariable* pVariable )
        {
            const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getElement() );
            ASSERT( pType );
            
            InstanceVariable* pInstance = new InstanceVariable( pVariable, pType );
            {
                MonomorphicReferenceInstruction* pMono = 
                    new MonomorphicReferenceInstruction( pVariable, pInstance );
                pInstruction->append( pMono );
                pInstruction = pMono;
            }
            
            if( current.getChildren().size() > 1U )
            {
                EliminationInstruction* pElim = new EliminationInstruction;
                pInstruction->append( pElim );
                pInstruction = pElim;
            }
            
            if( current.isTerminal() )
            {
                buildTerminal( prev, current, pInstruction, pInstance );
            }
            else
            {
                ASSERT( !current.getChildren().empty() );
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildGenerateName( current, next, pInstruction, pInstance );
                }
            }
        }
        
        void buildGenerateName( const Name& prev, const Name& current, 
            Instruction* pInstruction, InstanceVariable* pVariable )
        {
            if( current.isTerminal() )
            {
                buildTerminal( prev, current, pInstruction, pVariable );
            }
            else if( current.isReference() )
            {
                commonRootDerivation( prev.getElement(), current.getElement(), pInstruction, pVariable );
                
                std::vector< const concrete::Element* > types;
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    types.push_back( next.getElement() );
                }
                
                const concrete::Dimension* pDimension = 
                    dynamic_cast< const concrete::Dimension* >( current.getElement() );
                ASSERT( pDimension );
                
                DimensionReferenceVariable* pReferenceVariable = 
                    new DimensionReferenceVariable( pVariable, types );
                    
                {
                    DimensionReferenceReadInstruction* pDimensionRead = 
                        new DimensionReferenceReadInstruction( 
                            pVariable, pReferenceVariable, pDimension );
                    pInstruction->append( pDimensionRead );
                    pInstruction = pDimensionRead;
                }
                
                if( types.size() > 1U )
                {
                    {
                        PolymorphicReferenceBranchInstruction* pBranch = 
                            new PolymorphicReferenceBranchInstruction( pReferenceVariable );
                        pInstruction->append( pBranch );
                        pInstruction = pBranch;
                    }
                    
                    for( std::size_t index : current.getChildren() )
                    {
                        const Name& next = m_resolution.getNodes()[ index ];
                        buildPolymorphicCase( current, next, pInstruction, pReferenceVariable );
                    }
                }
                else
                {
                    ASSERT( !types.empty() );
                    for( std::size_t index : current.getChildren() )
                    {
                        const Name& next = m_resolution.getNodes()[ index ];
                        buildMonoDereference( current, next, pInstruction, pReferenceVariable );
                    }
                }
            }
            else
            {
                commonRootDerivation( prev.getElement(), current.getElement(), pInstruction, pVariable );
                
                if( current.getChildren().size() > 1U )
                {
                    EliminationInstruction* pElim = new EliminationInstruction;
                    pInstruction->append( pElim );
                    pInstruction = pElim;
                }
                
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildGenerateName( current, next, pInstruction, pVariable );
                }
            }
        }
        
        void operator()( Instruction* pInstruction, ContextVariable* pVariable )
        {
            const std::vector< const concrete::Element* >& contextTypes = pVariable->getTypes();
            if( contextTypes.size() > 1U )
            {
                PolymorphicReferenceBranchInstruction* pBranch = 
                    new PolymorphicReferenceBranchInstruction( pVariable );
                pInstruction->append( pBranch );
                
                //start the recursion
                const Name& current = m_resolution.getNodes()[ 0U ];
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildPolymorphicCase( current, next, pBranch, pVariable );
                }
            }
            else
            {
                const Name& current = m_resolution.getNodes()[ 0U ];
                ASSERT( current.getChildren().size() == 1U );
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildMonoDereference( current, next, pInstruction, pVariable );
                }
            }
        }
        
    };
    
    /*
    class EnumerationOperationVisitor 
    {
        InvocationSolution& m_solution;
        const NameResolution& m_resolution;
        
    public:
        EnumerationOperationVisitor( InvocationSolution& solution, const NameResolution& resolution )
            :   m_solution( solution ),
                m_resolution( resolution )
        {
            
        }
        
        void commonRootDerivation( const concrete::Element* pFrom, const concrete::Element* pTo, 
            Instruction*& pInstruction, InstanceVariable*& pVariable )
        {
            if( pFrom && pFrom != pTo )
            {
                const concrete::Element* pCommon = findCommonRoot( pFrom, pTo );
                
                while( pFrom != pCommon )
                {
                    const concrete::Action* pParent = 
                        dynamic_cast< const concrete::Action* >( pFrom->getParent() );
                    ASSERT( pParent );
                    //generate parent derivation instruction
                    InstanceVariable* pInstanceVariable = 
                        new InstanceVariable( pVariable, pParent );
                    
                    ParentDerivationInstruction* pParentDerivation = 
                        new ParentDerivationInstruction( pVariable, pInstanceVariable );
                    pInstruction->append( pParentDerivation );
                    
                    pVariable = pInstanceVariable;
                    pInstruction = pParentDerivation;
                    pFrom = pFrom->getParent();
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
                    if( pIter->getLocalDomainSize() != 1 )
                    {
                        FailureInstruction* pFailure = new FailureInstruction;
                        pInstruction->append( pFailure );
                        pInstruction = pFailure;
                        return;
                    }
                    else
                    {
                        if( const concrete::Action* pAction = 
                            dynamic_cast< const concrete::Action* >( pIter ) )
                        {
                            InstanceVariable* pInstanceVariable = 
                                new InstanceVariable( pVariable, pAction );
                            
                            ChildDerivationInstruction* pChildDerivation = 
                                new ChildDerivationInstruction( pVariable, pInstanceVariable );
                            pInstruction->append( pChildDerivation );
                            
                            pVariable = pInstanceVariable;
                            pInstruction = pChildDerivation;
                        }
                        else if( const concrete::Dimension_User* pUserDimension =
                            dynamic_cast< const concrete::Dimension_User* >( pIter ) )
                        {
                            //just ignor this - always only want the parent instance variable..
                        }
                        else
                        {
                            THROW_RTE( "Unreachable" );
                        }
                    }
                }
            }
        }
            
        void buildPolymorphicCase( const Name& prev, const Name& current, 
            PolymorphicReferenceBranchInstruction* pInstruction, ReferenceVariable* pVariable )
        {
            const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getElement() );
            ASSERT( pType );
            
            InstanceVariable* pInstance = new InstanceVariable( pVariable, pType );
            PolymorphicCaseInstruction* pCase = new PolymorphicCaseInstruction( pVariable, pInstance );
            
            for( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildGenerateName( current, next, pCase, pInstance );
            }
        }
        
        void buildMonoDereference( const Name& prev, const Name& current,
            Instruction* pInstruction, ReferenceVariable* pVariable )
        {
            const concrete::Action* pType = dynamic_cast< const concrete::Action* >( current.getElement() );
            ASSERT( pType );
            
            InstanceVariable* pInstance = new InstanceVariable( pVariable, pType );
            
            MonomorphicReferenceInstruction* pMono = 
                new MonomorphicReferenceInstruction( pVariable, pInstance );
            pInstruction->append( pMono );
            
            for( std::size_t index : current.getChildren() )
            {
                const Name& next = m_resolution.getNodes()[ index ];
                buildGenerateName( current, next, pMono, pInstance );
            }
        }
        
        void buildGenerateName( const Name& prev, const Name& current, 
            Instruction* pInstruction, InstanceVariable* pVariable )
        {
            if( current.isTerminal() )
            {
                const concrete::Element* pElement = current.getElement();
                if( const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pElement ) )
                {
                    switch( m_solution.getOperation() )
                    {
                        case id_Size        :
                        case id_Range       :
                            {
                            }
                            break;
                        default:
                            THROW_RTE( "Unreachable" );
                    }
                }
                else
                {
                    THROW_INVOCATION_EXCEPTION( "Invalid invocation target: " << m_solution.getID() );
                }
                
            }
            else if( current.isReference() )
            {
                commonRootDerivation( prev.getElement(), current.getElement(), pInstruction, pVariable );
                
                std::vector< const concrete::Element* > types;
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    types.push_back( next.getElement() );
                }
                
                DimensionReferenceVariable* pReferenceVariable = 
                    new DimensionReferenceVariable( pVariable, types );
                
                if( types.size() > 1U )
                {
                    PolymorphicReferenceBranchInstruction* pBranch = 
                        new PolymorphicReferenceBranchInstruction( pReferenceVariable );
                    pInstruction->append( pBranch );
                    
                    for( std::size_t index : current.getChildren() )
                    {
                        const Name& next = m_resolution.getNodes()[ index ];
                        buildPolymorphicCase( current, next, pBranch, pReferenceVariable );
                    }
                }
                else
                {
                    ASSERT( !types.empty() );
                    for( std::size_t index : current.getChildren() )
                    {
                        const Name& next = m_resolution.getNodes()[ index ];
                        buildMonoDereference( current, next, pInstruction, pReferenceVariable );
                    }
                }
            }
            else
            {
                commonRootDerivation( prev.getElement(), current.getElement(), pInstruction, pVariable );
                
                if( current.getChildren().size() > 1U )
                {
                    EliminationInstruction* pElim = new EliminationInstruction;
                    pInstruction->append( pElim );
                    pInstruction = pElim;
                }
                
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildGenerateName( current, next, pInstruction, pVariable );
                }
            }
        }
        
        void operator()( Instruction* pInstruction, ContextVariable* pVariable )
        {
            const std::vector< const concrete::Element* >& contextTypes = pVariable->getTypes();
            if( contextTypes.size() > 1U )
            {
                PolymorphicReferenceBranchInstruction* pBranch = 
                    new PolymorphicReferenceBranchInstruction( pVariable );
                pInstruction->append( pBranch );
                
                //start the recursion
                const Name& current = m_resolution.getNodes()[ 0U ];
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildPolymorphicCase( current, next, pBranch, pVariable );
                }
            }
            else
            {
                const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( contextTypes.front() );
                InstanceVariable* pInstance = new InstanceVariable( pVariable, pAction );
                //start the recursion
                const Name& current = m_resolution.getNodes()[ 0U ];
                for( std::size_t index : current.getChildren() )
                {
                    const Name& next = m_resolution.getNodes()[ index ];
                    buildGenerateName( current, next, pInstruction, pInstance );
                }
            }
        }
        
    };*/
    
    void InvocationSolution::build( const DerivationAnalysis& analysis, const NameResolution& resolution )
    {
        ASSERT( !m_pRoot );
        ContextVariable* pContextVariable = new ContextVariable( m_contextElements );
        m_pRoot = new RootInstruction( pContextVariable );
        
        switch( getOperation() )
        {
            case id_Imp_NoParams        :
            case id_Imp_Params          :
            case id_Get                 :
            case id_Update              :
            case id_Old                 :
            case id_Stop                :
            case id_Pause               :
            case id_Resume              :
            case id_Defer               :
                {
                    GenericOperationVisitor builder( *this, resolution );
                    builder( m_pRoot, pContextVariable );
                }
                break;
            case id_Size                :
            case id_Range               :
                {
                    //EnumerationOperationVisitor builder( *this, resolution );
                    //builder( m_pRoot, pContextVariable );
                }
                break;
            case HIGHEST_OPERATION_TYPE :
                break;
        }
        
        switch( m_pRoot->eliminate() )
        {
            case Instruction::eSuccess   :
                {
                    std::vector< const Operation* > operations;
                    m_pRoot->getOperations( operations );
                    for( const Operation* pOperation : operations )
                        pOperation->getTargetAbstractTypes( m_targetTypes );
                    m_targetTypes = uniquify_without_reorder( m_targetTypes );
                }
                break;
            case Instruction::eFailure   :
                THROW_INVOCATION_EXCEPTION( "No possible derivation for: " << m_invocationID );
            case Instruction::eAmbiguous :
                THROW_INVOCATION_EXCEPTION( "Ambiguous derivation for: " << m_invocationID );
        }
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
    /*
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
    }*/
    
    reference InvocationSolution::evaluate( RuntimeEvaluator& evaluator, const reference& context ) const
    {
        int priority = 0;
        //return evaluate( evaluator, context, m_pRoot, priority );
        reference r;
        return r;
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////////////
    InvocationSolution* InvocationSolutionMap::constructInvocation( 
        const InvocationSolution::InvocationID& invocationID, const std::vector< TypeID >& implicitTypePath )
    {
        //construct new invocation
        InvocationSolution* pInvocation = 
            m_session.construct< InvocationSolution >( invocationID, implicitTypePath );
        
        //calcualate the concrete context
        {
            for( const abstract::Element* pContext : std::get< InvocationSolution::Context >( invocationID ) )
            {
                if( const abstract::Action* pContextAction = dynamic_cast< const abstract::Action* >( pContext ) )
                    m_analysis.getInstances( pContextAction, pInvocation->m_contextElements, true );
            }
        }
        
        //for range enumerations we want to enumerate all deriving types
        const bool bDerivingPathElements = isOperationEnumeration( std::get< OperationID >( invocationID ) );
        
        //calculate the concrete type path
        {
            for( const std::vector< const abstract::Element* >& typePathElement : 
                std::get< InvocationSolution::TypePath >( invocationID ) )
            {
                std::vector< const concrete::Element* > instances;
                for( const abstract::Element* pElement : typePathElement )
                    m_analysis.getInstances( pElement, instances, bDerivingPathElements );
                
                if( instances.empty() )
                {
                    THROW_NAMERESOLUTION_EXCEPTION( "Invalid type path. " << invocationID );
                }
                
                pInvocation->m_concreteTypePath.emplace_back( std::move( instances ) );
            }
        }
        
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
        
        InvocationSolution* pInvocation = constructInvocation( invocationID, implicitTypePath );
        
        NameResolution nameResolution( m_analysis, invocationID, 
            pInvocation->m_contextElements, pInvocation->m_concreteTypePath );
        
        pInvocation->build( m_analysis, nameResolution );
        
        m_invocations.insert( std::make_pair( invocationID, pInvocation ) );
                                    
        return pInvocation;
    }
}