

#ifndef INTERFACE_SESSION_18_04_2019
#define INTERFACE_SESSION_18_04_2019

#include "session.hpp"
#include "abstract.hpp"
#include "concrete.hpp"
#include "derivation.hpp"

#include <map>

namespace eg
{

    
    class InterfaceSession : public AppendingSession
    {
    public:
        InterfaceSession( const boost::filesystem::path& treePath );
        
        void interfaceAnalysis();
        
        
        void linkAnalysis();
        
        concrete::Action* instanceAnalysis();
        void dependencyAnalysis();
        
        
    private:
        template< typename T >
        struct CompareNodeIdentity
        {
            bool operator()( const T* pLeft, const T* pRight ) const
            {
                return pLeft->getIdentifier() < pRight->getIdentifier();
            }
        };
        using ActionOverrideMap = 
            std::map< const abstract::Action*, concrete::Action*, CompareNodeIdentity< const abstract::Action > >;
        using DimensionOverrideMap = 
            std::map< const abstract::Dimension*, concrete::Dimension*, CompareNodeIdentity< const abstract::Dimension > >;
            
        void collateBases( concrete::Action* pInstance, concrete::Inheritance_Node* pInheritanceNode );
        void calculateInstanceActionName( concrete::Action* pAction );
        void collateOverrides( concrete::Action* pInstance, concrete::Inheritance_Node* pInheritanceNode,
                ActionOverrideMap& actionInstances, DimensionOverrideMap& dimensionInstances );
        void constructInstance( concrete::Action* pInstance );
        void constructAllocator( concrete::Action* pInstance );
        void dependencyAnalysis_recurse( concrete::Action* pAction );
    private:
        DerivationAnalysis* m_pDerivationAnalysis;
    };

}

#endif //INTERFACE_SESSION_18_04_2019