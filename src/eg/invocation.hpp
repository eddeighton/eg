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
    struct Name;
    class NameResolution;
    
    
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
    protected:
        InvocationSolution( const IndexedObject& object )
            :   IndexedObject( object )
        {
        }
        
    public:
        using Context = std::vector< const abstract::Element* >;
        using TypePath = std::vector< std::vector< const abstract::Element* > >;
        using TargetTypes = std::vector< const abstract::Element* >;
        
        struct DerivationStep
        {
            std::size_t id;
            const concrete::Element* pInstance  = nullptr;
            DerivationStep* pParent = nullptr;
            enum Type
            {
                eParent,
                eChild,
                eLink,
                eDeReference,
                eTarget,
                eRoot,
                eEnum,
                eFailed
            };
            Type type;
            int domain;    
            std::vector< DerivationStep* > next;        
        };
    protected:
        std::vector< DerivationStep* > m_steps;
        DerivationStep* m_pRoot;
        
        DerivationStep* addStep( 
            const concrete::Element* pInstance, 
            DerivationStep* pParent,
            DerivationStep::Type type,
            int domain )
        {
            for( DerivationStep* pExisting : m_steps )
            {
                if( pExisting->pInstance == pInstance && 
                    pExisting->pParent == pParent &&
                    pExisting->type == type )
                    return pExisting;
            }
            DerivationStep* pNewVertex = new DerivationStep{ m_steps.size(), pInstance, pParent, type, domain };
            m_steps.push_back( pNewVertex );
            if( pParent )
            {
                pParent->next.push_back( pNewVertex );
            }
            return pNewVertex;
        }
        
        DerivationStep* buildDerivation( const DerivationAnalysis& analysis, 
            const concrete::Element* pFrom, 
            const concrete::Element* pTo,
            DerivationStep* pStep,
            bool bFinal );
            
        void buildSolutionGraph_recurse( const DerivationAnalysis& analysis, 
            const NameResolution& resolution,
            const Name* pLast,
            const Name& name,
            DerivationStep* pStep );
            
            
    public:
        using InvocationID = std::tuple< Context, TypePath, OperationID >;
        using InvocationMap = std::map< InvocationID, InvocationSolution* >;
    
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
            
    private:
        void propagate_failed( DerivationStep* pStep );
    
        reference evaluate( RuntimeEvaluator& evaluator, const reference& context, const DerivationStep* pStep, int& iPriority ) const;
    public:
        OperationID getOperation() const { return m_operationType; }
        const Context& getContext() const { return m_context; }
        const TargetTypes& getTargetTypes() const { return m_targetTypes; }
        const TargetTypes& getFinalPathTypes() const { return m_finalPathTypes; }
        const TypeIDVector& getImplicitTypePath() const { return m_implicitTypePath; }
        const DerivationStep* getRoot() const { return m_pRoot; }
    protected:
     
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
     
        OperationID m_operationType;
        Context m_context;
        TargetTypes m_targetTypes;
        TargetTypes m_finalPathTypes;
        TypeIDVector m_implicitTypePath;
    };
    
    inline std::ostream& operator<<( std::ostream& os, const InvocationSolution::InvocationID& invocationID )
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