
#ifndef DERIVATION_18_04_2019
#define DERIVATION_18_04_2019

#include "eg.hpp"
#include "objects.hpp"
#include "concrete.hpp"

#include "runtime/eg_common.hpp"

#include <map>
#include <vector>

namespace eg
{

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class DerivationAnalysis : public IndexedObject
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eDerivationAnalysis;
    protected:
        DerivationAnalysis( const IndexedObject& object )
            :   IndexedObject( object )
        {
    
        }
        
    public:
    
        using InstanceMap = std::multimap< const abstract::Element*, const concrete::Element* >;
        InstanceMap m_instanceMap;
        
        using InheritanceNodeMap = std::multimap< const abstract::Action*, const concrete::Inheritance_Node* >;
        InheritanceNodeMap m_inheritanceMap;
        
        void getInstances( const abstract::Element* pElement, std::vector< const concrete::Element* >& instances ) const
        {
            InstanceMap::const_iterator iLower = m_instanceMap.lower_bound( pElement );
            InstanceMap::const_iterator iUpper = m_instanceMap.upper_bound( pElement );
            for( ; iLower != iUpper; ++iLower )
                instances.push_back( iLower->second );
        }
        
        void getInheritanceNodes( const abstract::Action* pAction, std::vector< const concrete::Inheritance_Node* >& inheritanceNodes ) const
        {
            InheritanceNodeMap::const_iterator iLower = m_inheritanceMap.lower_bound( pAction );
            InheritanceNodeMap::const_iterator iUpper = m_inheritanceMap.upper_bound( pAction );
            for( ; iLower != iUpper; ++iLower )
                inheritanceNodes.push_back( iLower->second );
        }
        
        using InstancePair = std::pair< const concrete::Element*, const concrete::Element* >;
        using CommonRootMap = std::map< InstancePair, const concrete::Element* >;
        CommonRootMap m_commonRoots;
        
        struct NameResolution
        {
            const DerivationAnalysis& analysis;
            
            NameResolution( const DerivationAnalysis& analysis ) : analysis( analysis )
            {
            }
            
            struct Name
            {
                const concrete::Inheritance_Node* pInheritanceNode    = nullptr;
                const concrete::Dimension*        pDimension          = nullptr;
                int score;
                bool isTarget;
                std::vector< int > children;
                
                Name( const concrete::Inheritance_Node* pInheritanceNode, int score, bool isTarget )
                    :   pInheritanceNode( pInheritanceNode ),
                        score( score ),
                        isTarget( isTarget )
                {
                }
                Name( const concrete::Dimension* pDimension, int score, bool isTarget )
                    :   pDimension( pDimension ),
                        score( score ),
                        isTarget( isTarget )
                {
                }
                
                const concrete::Element* getInstance() const
                {
                    if( pInheritanceNode )
                        return pInheritanceNode->getRootConcreteAction();
                    else if( pDimension )
                        return pDimension;
                    else
                    {
                        THROW_RTE( "Invalid name" );
                    }
                }
            };
            
            Name* add( int iParent, const concrete::Inheritance_Node* pInheritanceNode, int score, bool bIsTarget = false );
            Name* add( int iParent, const concrete::Dimension* pDimension, int score, bool bIsTarget = false );
            
            std::vector< Name > nodes;
            
            void expandReferences();
            
            void addType( const std::vector< const concrete::Element* >& instances );
            
            void pruneBranches( Name* pNode );
            
            void resolve( 
                const std::vector< const concrete::Inheritance_Node* >& contextInstances,
                std::vector< std::vector< const concrete::Element* > > typePathInstances );
        };
        
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    };
    
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class RuntimeEvaluator
    {
    public:
        virtual ~RuntimeEvaluator(){}
        virtual reference getReference( const reference& dimension ) = 0;
    };
    
    class EGRuntimeImpl;
    
    class InvocationSolution : public IndexedObject
    {
        friend class ObjectFactoryImpl;
        friend class OperationsSession;
        friend class EGRuntimeImpl;
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
            const DerivationAnalysis::NameResolution& resolution,
            const DerivationAnalysis::NameResolution::Name* pLast,
            const DerivationAnalysis::NameResolution::Name& name,
            DerivationStep* pStep );
            
            
    public:
        void build( const DerivationAnalysis& analysis, const DerivationAnalysis::NameResolution& resolution );
        
        bool isImplicitStarter() const;
        
        reference evaluate( RuntimeEvaluator& evaluator, const reference& context ) const;
            
    private:
        void propagate_failed( DerivationStep* pStep );
    
        reference evaluate( RuntimeEvaluator& evaluator, const reference& context, const DerivationStep* pStep, int& iPriority ) const;
    public:
        TypeID getOperation() const { return m_operationType; }
        const Context& getContext() const { return m_context; }
        const TargetTypes& getTargetTypes() const { return m_targetTypes; }
        const TypeIDVector& getImplicitTypePath() const { return m_implicitTypePath; }
        const DerivationStep* getRoot() const { return m_pRoot; }
    protected:
     
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
     
        TypeID m_operationType;
        Context m_context;
        TargetTypes m_targetTypes;
        TypeIDVector m_implicitTypePath;
    };
}

#endif //DERIVATION_18_04_2019