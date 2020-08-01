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
#include "eg_compiler/interface.hpp"
#include "eg_compiler/concrete.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/link.hpp"
#include "eg_compiler/translation_unit.hpp"

#include <map>

namespace eg
{

    
    class InterfaceSession : public AppendingSession
    {
    public:
        InterfaceSession( const boost::filesystem::path& treePath );
        
        using TUFileIDIfExistsFPtr = 
            std::function< IndexedObject::FileID( const std::string& strName  ) >;
        
        void linkAnalysis();
        concrete::Action* instanceAnalysis();
        void translationUnitAnalysis( const boost::filesystem::path& rootFolder, TUFileIDIfExistsFPtr pTUFileIDIfExists );
        
        //allow saving the interface session to new file
        void store() const { AppendingSession::store(); }
        void store( const boost::filesystem::path& filePath ) const;
        
        const interface::Root* getTreeRoot() const { return eg::root_cst< eg::interface::Root >( getMaster() ); }
        interface::Root* getTreeRoot() { return eg::root< eg::interface::Root >( getAppendingObjects() ); }
        
        const TranslationUnitAnalysis& getTranslationUnitAnalysis() const { return *m_pTranslationUnitAnalysis; }
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
            std::map< const interface::Context*, concrete::Action*, CompareNodeIdentity< const interface::Context > >;
        using DimensionOverrideMap = 
            std::map< const interface::Dimension*, concrete::Dimension*, CompareNodeIdentity< const interface::Dimension > >;
            
        concrete::Inheritance_Node* constructInheritanceNode( concrete::Action* pRootInstance, 
            concrete::Inheritance_Node* pParent, const interface::Context* pAction );
        concrete::Inheritance_Node* constructInheritanceTree( concrete::Action* pInstance, 
            concrete::Inheritance_Node* pInheritanceNode, const interface::Context* pAction );
        void constructInheritanceTree( concrete::Action* pInstance );
        
        void calculateInstanceActionName( concrete::Action* pAction );
        void collateOverrides( concrete::Action* pInstance, 
                concrete::Inheritance_Node* pInheritanceNode,
                ActionOverrideMap& actionInstances, 
                DimensionOverrideMap& dimensionInstances, 
                const std::vector< const interface::Object* >& objects, 
                std::set< const interface::Object* >& constructedObjects );
        void constructInstance( concrete::Action* pInstance, 
                const std::vector< const interface::Object* >& objects, 
                std::set< const interface::Object* >& constructedObjects );
        void constructAllocator( concrete::Action* pInstance, concrete::Action* pObject, std::vector< concrete::Allocator* >& allocators  );
		
        using TranslationUnitMap = std::map< TranslationUnit::CoordinatorHostnameDefinitionFile, TranslationUnit::ActionSet >;
        void translationUnitAnalysis_recurse( concrete::Action* pAction, TranslationUnitMap& translationUnitMap );
        
    private:
        DerivationAnalysis* m_pDerivationAnalysis;
        TranslationUnitAnalysis* m_pTranslationUnitAnalysis;
		LinkAnalysis* m_pLinkAnalysis;
    };

}

#endif //INTERFACE_SESSION_18_04_2019