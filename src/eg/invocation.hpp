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
        using InvocationID = std::tuple< Context, TypePath, OperationID >;
        using InvocationMap = std::map< InvocationID, InvocationSolution* >;
        
        using ElementPair = std::pair< const interface::Element*, const concrete::Element* >;
        using ElementPairVector = std::vector< ElementPair >;
        using ElementPairVectorVector = std::vector< ElementPairVector >;
        
    protected:
        InvocationSolution( const IndexedObject& object )
            :   IndexedObject( object ),
                m_bDimensions( false ),
                m_bHomogeneousDimensions( false )
        {
        }
        InvocationSolution( const IndexedObject& object, const InvocationID& invocationID, const std::vector< TypeID >& implicitTypePath )
            :   IndexedObject( object ),
                m_invocationID( invocationID ),
                m_implicitTypePath( implicitTypePath ),
                m_bDimensions( false ),
                m_bHomogeneousDimensions( false )
        {
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
        
        static ElementPairVector getElementVector( const DerivationAnalysis& analysis, 
            const std::vector< const interface::Element* >& interfaceElements, bool bIncludeInherited );
            
        static ElementPairVector getElementVector( const DerivationAnalysis& analysis, 
            const std::vector< interface::Action* >& interfaceElements, bool bIncludeInherited );
            
    private:
        void build( const DerivationAnalysis& analysis, const NameResolution& resolution );
        void analyseReturnTypes();
        
    public:
        void evaluate( RuntimeEvaluator& evaluator, const reference& context ) const;
   
    public:
        const InvocationID& getID() const { return m_invocationID; }
        OperationID getOperation() const { return std::get< OperationID >( m_invocationID ); }
        const TypeIDVector& getImplicitTypePath() const { return m_implicitTypePath; }
        const ElementPairVector& getContextElements() const { return m_context; }
        const ElementPairVectorVector& getTypePathElements() const { return m_typePath; }
        const Context& getContext() const { return std::get< Context >( m_invocationID ); }
        const Context& getReturnTypes() const { return m_returnTypes; }
        bool isImplicitStarter() const { return !m_bDimensions; }
        bool isReturnTypeDimensions() const { return m_bDimensions; }
        bool isDimensionReturnTypeHomogeneous() const { return m_bHomogeneousDimensions; }
        const RootInstruction* getRoot() const { return m_pRoot; }
    protected:
     
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
     
        InvocationID m_invocationID;
        TypeIDVector m_implicitTypePath;
        
        ElementPairVector m_context;
        ElementPairVectorVector m_typePath;
        
        Context m_returnTypes;
        bool m_bDimensions;
        bool m_bHomogeneousDimensions;
        
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