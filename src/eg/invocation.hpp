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


#ifndef INVOCATION_05_05_2019
#define INVOCATION_05_05_2019

#include "derivation.hpp"
#include "instruction.hpp"

namespace eg
{
    class RuntimeEvaluator
    {
    public:
        virtual ~RuntimeEvaluator(){}
        virtual reference getReference( const reference& dimension ) = 0;
    };
    
    class EGRuntimeImpl;
    
    class Identifiers;
    class InvocationSolutionMap;
    class Name;
    class NameResolution;
    
    class InvocationException : public std::runtime_error
    {
    public:
        InvocationException( const std::string& strMessage )
            :   std::runtime_error( strMessage )
        {}
    };
    
    #define THROW_INVOCATION_EXCEPTION( msg ) \
        DO_STUFF_AND_REQUIRE_SEMI_COLON( \
            std::ostringstream _os; \
            _os << msg; \
            throw InvocationException( _os.str() );)
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class InvocationSolution : public IndexedObject
    {
        friend class ObjectFactoryImpl;
        friend class OperationsSession;
        friend class EGRuntimeImpl;
        friend class InvocationSolutionMap;
    public:
        static const ObjectType Type = eInvocationSolution;
        using Context = std::vector< const interface::Element* >;
        using TypePath = std::vector< std::vector< const interface::Element* > >;
        using TargetTypes = std::vector< const interface::Element* >;
        using InvocationID = std::tuple< Context, TypePath, OperationID >;
        using InvocationMap = std::map< InvocationID, InvocationSolution* >;
    protected:
        InvocationSolution( const IndexedObject& object )
            :   IndexedObject( object )
        {
        }
        InvocationSolution( const IndexedObject& object, const InvocationID& invocationID, const std::vector< TypeID >& implicitTypePath )
            :   IndexedObject( object ),
                m_invocationID( invocationID ),
                m_implicitTypePath( implicitTypePath )
        {
            if( !std::get< InvocationSolution::TypePath >( m_invocationID ).empty() )
                m_finalPathTypes = std::get< InvocationSolution::TypePath >( m_invocationID ).back();
        }
        
    public:
        static InvocationID invocationIDFromTypeIDs( 
            const IndexedObject::Array& objects, const Identifiers& identifiers, 
            const std::vector< TypeID >& contextTypes, 
            const std::vector< TypeID >& implicitTypePath, eg::OperationID operationTypeID );
            
        static InvocationID invocationIDFromTypeIDs( 
            const IndexedObject::Array& objects, const Identifiers& identifiers, 
            const TypeID runtimeContextType, 
            const std::vector< TypeID >& implicitTypePath, bool bHasParameters );
        
    private:
        void build( const DerivationAnalysis& analysis, const NameResolution& resolution );
        
    public:
        bool isImplicitStarter() const;
        
        reference evaluate( RuntimeEvaluator& evaluator, const reference& context ) const;
   
    public:
        const InvocationID& getID() const { return m_invocationID; }
        OperationID getOperation() const { return std::get< OperationID >( m_invocationID ); }
        const Context& getContext() const { return std::get< Context >( m_invocationID ); }
        const TargetTypes& getTargetTypes() const { return m_targetTypes; }
        const TargetTypes& getFinalPathTypes() const { return m_finalPathTypes; }
        const TypeIDVector& getImplicitTypePath() const { return m_implicitTypePath; }
        
        const std::vector< const concrete::Element* >& getConcreteContext() const { return m_contextElements; }
        const std::vector< std::vector< const concrete::Element* > >& getConcreteTypePath() const { return m_concreteTypePath; }
        
        const RootInstruction* getRoot() const { return m_pRoot; }
    protected:
     
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
     
        InvocationID m_invocationID;
        TypeIDVector m_implicitTypePath;
        TargetTypes m_finalPathTypes;
        
        TargetTypes m_targetTypes;
        std::vector< const concrete::Element* > m_contextElements;
        std::vector< std::vector< const concrete::Element* > > m_concreteTypePath;
        
        RootInstruction* m_pRoot = nullptr;
    };
    
    std::ostream& operator<<( std::ostream& os, const InvocationSolution::InvocationID& invocationID );
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class InvocationSolutionMap
    {
        friend class ObjectFactoryImpl;
        friend class OperationsSession;
        friend class EGRuntimeImpl;
    public:
        InvocationSolutionMap( CreatingSession& session, const DerivationAnalysis& analysis )
            :   m_session( session ),
                m_analysis( analysis )
        {
        }
        
        NameResolution doNameResolution( const InvocationSolution::InvocationID& invocationID );
    
        InvocationSolution* constructInvocation( const InvocationSolution::InvocationID& invocationID, 
            const std::vector< TypeID >& implicitTypePath );
            
        const InvocationSolution* getInvocation( 
            const InvocationSolution::InvocationID& invocationID, 
            const std::vector< TypeID >& implicitTypePath );
        
    private:
        CreatingSession& m_session;
        const DerivationAnalysis& m_analysis;
        InvocationSolution::InvocationMap m_invocations;
    };
}

#endif //INVOCATION_05_05_2019