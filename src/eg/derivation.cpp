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



#include "eg_compiler/derivation.hpp"
#include "eg_compiler/link.hpp"

namespace eg
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    
    void DerivationAnalysis::analyseCompatibility( 
            const std::vector< const interface::Action* >& interfaceActions,
            const std::vector< const concrete::Inheritance_Node* >& inheritanceNodes )
    {
        for( const interface::Action* pInterfaceAction : interfaceActions )
        {
            Compatibility compatibility;
            for( const concrete::Inheritance_Node* pINode : inheritanceNodes )
            {
                if( pINode->getAbstractAction() == pInterfaceAction )
                {
                    for( const concrete::Inheritance_Node* pINodeIter = pINode; pINodeIter; pINodeIter = pINodeIter->getParent() )
                    {
                        compatibility.staticCompatibleTypes.insert( pINodeIter->getAbstractAction() );
                        compatibility.dynamicCompatibleTypes.insert( pINodeIter->getRootConcreteAction() );
                    }
                    pINode->getStaticDerived( compatibility.staticCompatibleTypes );
                    pINode->getDynamicDerived( compatibility.dynamicCompatibleTypes );
                }
            }
            m_compatibility.insert( std::make_pair( pInterfaceAction, compatibility ) );
        }
    }
    
    void DerivationAnalysis::analyseLinkCompatibility( 
            const std::vector< const interface::Action* >& interfaceActions,
            const LinkGroup::Vector& linkGroups )
    {
        for( const LinkGroup* pLinkGroup : linkGroups )
        {
            const std::vector< interface::Action* >& links = pLinkGroup->getLinks();
            
            for( interface::Action* pLink : links )
            {
                CompatibilityMap::const_iterator iFindCst = m_compatibility.find( pLink );
                VERIFY_RTE( iFindCst != m_compatibility.end() );
                const Compatibility& linkCompatibility = iFindCst->second;
                
                interface::Action* pLinkTarget = LinkGroup::getLinkTarget( pLink );
                
                CompatibilityMap::const_iterator iFindCst2 = m_compatibility.find( pLinkTarget );
                VERIFY_RTE( iFindCst2 != m_compatibility.end() );
                const Compatibility& linkTargetCompatibility = iFindCst2->second;
                
                //for the link itself just copy the compatibility of the link target
                //the link will already include itself and CANNOT be derived from or linked itself
                {
                    CompatibilityMap::iterator iFind = m_compatibility.find( pLink );
                    VERIFY_RTE( iFind != m_compatibility.end() );
                    iFind->second.staticLinkCompatibleTypes.insert( 
                        linkTargetCompatibility.staticCompatibleTypes.begin(),
                        linkTargetCompatibility.staticCompatibleTypes.end() );
                        
                    iFind->second.dynamicCompatibleToLinkTypes.insert( 
                        linkTargetCompatibility.dynamicCompatibleTypes.begin(),
                        linkTargetCompatibility.dynamicCompatibleTypes.end() );
                }
                
                for( const interface::Action* pCompatibleType : linkTargetCompatibility.staticCompatibleTypes )
                {
                    CompatibilityMap::iterator iFind = m_compatibility.find( pCompatibleType );
                    VERIFY_RTE( iFind != m_compatibility.end() );
                    iFind->second.staticLinkCompatibleTypes.insert( pLink );
                    
                    //if a type can be converted to from a link - then the dynamicCompatibleFromLinkTypes
                    //for the type ARE the dynamicCompatibleTypes for the link
                    iFind->second.dynamicCompatibleFromLinkTypes.insert( 
                        linkCompatibility.dynamicCompatibleTypes.begin(),
                        linkCompatibility.dynamicCompatibleTypes.end()  );
                }
            }
        }
        
        //copy the staticCompatibleTypes to staticLinkCompatibleTypes
        for( CompatibilityMap::iterator i = m_compatibility.begin(),
            iEnd = m_compatibility.end(); i!=iEnd; ++i )
        {
            Compatibility& compatibility = i->second;
            compatibility.staticLinkCompatibleTypes.insert( 
                compatibility.staticCompatibleTypes.begin(),
                compatibility.staticCompatibleTypes.end() );
        }
    }
    
    const DerivationAnalysis::Compatibility& DerivationAnalysis::getCompatibility( const interface::Action* pAction ) const
    {
        CompatibilityMap::const_iterator iFind = m_compatibility.find( pAction );
        VERIFY_RTE( iFind != m_compatibility.end() );
        return iFind->second;
    }
    
    void DerivationAnalysis::getInstances( const interface::Element* pElement, std::vector< const concrete::Element* >& instances, bool bDeriving ) const
    {
        const interface::Action* pAction = dynamic_cast< const interface::Action* >( pElement );
        if( bDeriving && pAction )
        {
            InheritanceNodeMap::const_iterator iLower = m_inheritanceMap.lower_bound( pAction );
            InheritanceNodeMap::const_iterator iUpper = m_inheritanceMap.upper_bound( pAction );
            for( ; iLower != iUpper; ++iLower )
                instances.push_back( iLower->second->getRootConcreteAction() );
        }
        else
        {
            InstanceMap::const_iterator iLower = m_instanceMap.lower_bound( pElement );
            InstanceMap::const_iterator iUpper = m_instanceMap.upper_bound( pElement );
            for( ; iLower != iUpper; ++iLower )
                instances.push_back( iLower->second );
        }
    }
    
    void DerivationAnalysis::load( Loader& loader )
    {
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const interface::Element*     pElement   = loader.loadObjectRef< const interface::Element >();
                const concrete::Element*     pInstance  = loader.loadObjectRef< concrete::Element >();
                m_instanceMap.insert( std::make_pair( pElement, pInstance ) );
            }
        }
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const interface::Action*             pAction          = loader.loadObjectRef< const interface::Action >();
                const concrete::Inheritance_Node*   pInheritanceNode = loader.loadObjectRef< const concrete::Inheritance_Node >();
                m_inheritanceMap.insert( std::make_pair( pAction, pInheritanceNode ) );
            }
        }
        
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const interface::Action* pAction = loader.loadObjectRef< const interface::Action >();
                Compatibility compatibility;
                loader.loadObjectSet( compatibility.staticCompatibleTypes );
                loader.loadObjectSet( compatibility.staticLinkCompatibleTypes );
                loader.loadObjectSet( compatibility.dynamicCompatibleTypes );
                loader.loadObjectSet( compatibility.dynamicCompatibleFromLinkTypes );
                loader.loadObjectSet( compatibility.dynamicCompatibleToLinkTypes );
                m_compatibility.insert( std::make_pair( pAction, compatibility ) );
            }
        }
    }
    void DerivationAnalysis::store( Storer& storer ) const
    {
        {
            const std::size_t szSize = m_instanceMap.size();
            storer.store( szSize );
            for( InstanceMap::const_iterator 
                i = m_instanceMap.begin(),
                iEnd = m_instanceMap.end(); i!=iEnd; ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectRef( i->second );
            }
        }
        
        {
            const std::size_t szSize = m_inheritanceMap.size();
            storer.store( szSize );
            for( InheritanceNodeMap::const_iterator 
                i = m_inheritanceMap.begin(),
                iEnd = m_inheritanceMap.end(); i!=iEnd; ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectRef( i->second );
            }
        }
        {
            const std::size_t szSize = m_compatibility.size();
            storer.store( szSize );
            for( CompatibilityMap::const_iterator 
                i = m_compatibility.begin(),
                iEnd = m_compatibility.end(); i!=iEnd; ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectSet( i->second.staticCompatibleTypes );
                storer.storeObjectSet( i->second.staticLinkCompatibleTypes );
                storer.storeObjectSet( i->second.dynamicCompatibleTypes );
                storer.storeObjectSet( i->second.dynamicCompatibleFromLinkTypes );
                storer.storeObjectSet( i->second.dynamicCompatibleToLinkTypes );
            }
        }
    }
        

} //namespace eg