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
        
        void linkAnalysis();
        concrete::Action* instanceAnalysis();
        void dependencyAnalysis();
        
        const abstract::Root* getTreeRoot() const { return eg::root_cst< eg::abstract::Root >( getMaster() ); }
        abstract::Root* getTreeRoot() { return eg::root< eg::abstract::Root >( getAppendingObjects() ); }
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
            
        concrete::Inheritance_Node* constructInheritanceNode( concrete::Action* pRootInstance, 
            concrete::Inheritance_Node* pParent, const abstract::Action* pAction );
        concrete::Inheritance_Node* constructInheritanceTree( concrete::Action* pInstance, 
            concrete::Inheritance_Node* pInheritanceNode, const abstract::Action* pAction );
        void constructInheritanceTree( concrete::Action* pInstance );
        
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