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



#include "interface_session.hpp"

#include "interface.hpp"

namespace eg
{

    InterfaceSession::InterfaceSession( const boost::filesystem::path& treePath )
        :   AppendingSession( treePath, IndexedObject::MASTER_FILE )
    {
        m_pDerivationAnalysis = oneOpt< DerivationAnalysis >( getMaster() );
        if( !m_pDerivationAnalysis )
            m_pDerivationAnalysis = construct< DerivationAnalysis >();
        
        m_pTranslationUnitAnalysis = oneOpt< TranslationUnitAnalysis >( getMaster() );
        if( !m_pTranslationUnitAnalysis )
            m_pTranslationUnitAnalysis = construct< TranslationUnitAnalysis >();
    }
    
    
    void InterfaceSession::store( const boost::filesystem::path& filePath ) const
    {
        VERIFY_RTE( m_fileMap.size() == 1U );
        IndexedFile* pFile = m_fileMap.find( IndexedObject::MASTER_FILE )->second;
        VERIFY_RTE( pFile );
        IndexedFile::FileIDtoPathMap files;
        files.insert( std::make_pair( IndexedObject::MASTER_FILE, filePath ) );
        IndexedFile::store( filePath, files, pFile->getObjects() );
    }

    void InterfaceSession::linkAnalysis()
    {
        //calculate inheritance
        {
            std::vector< interface::Action* > actions = many< interface::Action >( getMaster() );
            for( interface::Action* pAction : actions )
            {
                for( interface::Action* pBase : pAction->m_baseActions )
                {
                    if( pAction->isLink() )
                    {
                        pBase->m_linkers.push_back( pBase );
                    }
                    else
                    {
                        pBase->m_inheriters.push_back( pBase );
                    }
                }
            }
        }
    }

    
    void collateChildren( const interface::Action* pAction, 
            std::vector< const interface::Action* >& actions, 
            std::vector< const interface::Dimension* >& dimensions )
    {
        for( const interface::Element* pObject : pAction->getChildren() )
        {
            switch( pObject->getType() )
            {
                case eAbstractDimension : 
                    dimensions.push_back( dynamic_cast< const interface::Dimension* >( pObject ) );
                    break;
                case eAbstractRoot      : 
                case eAbstractAction    :  
                    if( const interface::Action* pChildAction = dynamic_cast< const interface::Action* >( pObject ) )
                    {
                        if( !pChildAction->isAbstract() )
                        {
                            actions.push_back( dynamic_cast< const interface::Action* >( pObject ) );
                        }
                    }
                    break;
            }
        }
    }
    
    concrete::Inheritance_Node* InterfaceSession::constructInheritanceNode( concrete::Action* pConcreteAction, 
            concrete::Inheritance_Node* pParent, const interface::Action* pAbstractAction )
    {
        concrete::Inheritance_Node* pInheritanceNode    = construct< concrete::Inheritance_Node >(); 
        pInheritanceNode->m_pRootConcreteAction         = pConcreteAction;
        pInheritanceNode->m_pParent                     = pParent;
        pInheritanceNode->m_pAction                     = pAbstractAction;
        
        if( pParent )
            pParent->m_children.push_back( pInheritanceNode );
        
        m_pDerivationAnalysis->m_inheritanceMap.insert( std::make_pair( pAbstractAction, pInheritanceNode ) );
        
        return pInheritanceNode;
    }
    
    concrete::Inheritance_Node* InterfaceSession::constructInheritanceTree( concrete::Action* pConcreteAction,  
                concrete::Inheritance_Node* pParent, const interface::Action* pAbstractAction )
    {
        concrete::Inheritance_Node* pInheritanceNode  
            = constructInheritanceNode( pConcreteAction, pParent, pAbstractAction );
        
        for( const interface::Action* pBaseAbstractAction : pAbstractAction->m_baseActions )
        {
            constructInheritanceTree( pConcreteAction, pInheritanceNode, pBaseAbstractAction );
        }
        
        return pInheritanceNode;
    }
    
    void InterfaceSession::constructInheritanceTree( concrete::Action* pConcreteAction )
    {
        pConcreteAction->m_inheritance = constructInheritanceTree( pConcreteAction, nullptr, pConcreteAction->getAction() );
    }
    
    template< typename T >
    struct CompareNodeIdentity
    {
        bool operator()( const T* pLeft, const T* pRight ) const
        {
            return pLeft->getIdentifier() < pRight->getIdentifier();
        }
    };
    
    using ActionOverrideMap = 
        std::map< interface::Action*, concrete::Action*, CompareNodeIdentity< interface::Action > >;
    using DimensionOverrideMap = 
        std::map< interface::Dimension*, concrete::Dimension*, CompareNodeIdentity< interface::Dimension > >;
        
    void InterfaceSession::calculateInstanceActionName( concrete::Action* pAction )
    {
        if( pAction->m_pParent )
        {
            concrete::Action* pParent = dynamic_cast< concrete::Action* >( pAction->m_pParent );
            if( pParent->m_strName.empty() )
                pAction->m_strName = pAction->getAction()->getIdentifier();
            else
                pAction->m_strName = pParent->m_strName + "_" + pAction->getAction()->getIdentifier();
        }
    }
    
    void InterfaceSession::collateOverrides( concrete::Action* pInstance, concrete::Inheritance_Node* pInheritanceNode,
            ActionOverrideMap& actionInstances, DimensionOverrideMap& dimensionInstances )
    {
        std::vector< const interface::Action* > actions;
        std::vector< const interface::Dimension* > dimensions;
        collateChildren( pInheritanceNode->m_pAction, actions, dimensions );
        
        for( const interface::Action* pChildAction : actions )
        {
            std::map< const interface::Action*, concrete::Action* >::iterator iFind = 
                actionInstances.find( pChildAction );
            if( iFind == actionInstances.end() )
            {
                //create the child instance node 
                concrete::Action* pChildInstance = construct< concrete::Action >();
                pInstance->m_children.push_back( pChildInstance );
                pChildInstance->m_pElement = pChildAction;
                pChildInstance->m_pParent = pInstance;
                calculateInstanceActionName( pChildInstance );
                
                m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pChildAction, pChildInstance ) );
                
                //record it in the inheritance node
                pInheritanceNode->m_actions.push_back( pChildInstance );
                
                actionInstances.insert( std::make_pair( pChildAction, pChildInstance ) );
                
                pChildInstance->m_pStopCycle             = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pStopCycle->m_type     = concrete::Dimension_Generated::eActionStopCycle;
                pChildInstance->m_pStopCycle->m_pAction  = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pStopCycle );
                
                pChildInstance->m_pState             = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pState->m_type     = concrete::Dimension_Generated::eActionState;
                pChildInstance->m_pState->m_pAction  = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pState );
                
                pChildInstance->m_pFiber             = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pFiber->m_type     = concrete::Dimension_Generated::eActionFiber;
                pChildInstance->m_pFiber->m_pAction  = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pFiber );
                
                //does the action have an object mapping?
                if( !pChildAction->getBaseType().empty() )
                {
                    pChildInstance->m_pMappedObject                 = construct< concrete::Dimension_Generated >();
                    pChildInstance->m_pMappedObject->m_type         = concrete::Dimension_Generated::eActionObject;
                    pChildInstance->m_pMappedObject->m_pAction      = pChildInstance;
                    pChildInstance->m_children.push_back( pChildInstance->m_pMappedObject );
                }
                
                pChildInstance->m_pReference                    = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pReference->m_type            = concrete::Dimension_Generated::eActionReference;
                pChildInstance->m_pReference->m_pAction         = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pReference );
                
                pChildInstance->m_pRingIndex                    = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pRingIndex->m_type            = concrete::Dimension_Generated::eRingIndex;
                pChildInstance->m_pRingIndex->m_pAction         = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pRingIndex );
            }
            else
            {
                pInheritanceNode->m_actions.push_back( iFind->second );
            }
        }
        
        for( const interface::Dimension* pChildDimension : dimensions )
        {
            std::map< const interface::Dimension*, concrete::Dimension* >::iterator iFind = 
                dimensionInstances.find( pChildDimension );
            if( iFind == dimensionInstances.end() )
            {
                concrete::Dimension_User* pChildInstance = construct< concrete::Dimension_User >();
                pInstance->m_children.push_back( pChildInstance );
                pChildInstance->m_pElement = pChildDimension;
                pChildInstance->m_pParent = pInstance;
                
                m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pChildDimension, pChildInstance ) );

                pInheritanceNode->m_dimensions.push_back( pChildInstance );
                
                dimensionInstances.insert( std::make_pair( pChildDimension, pChildInstance ) );
                
                pChildInstance->m_pTimestamp = 
                    construct< concrete::Dimension_Generated >();
                pChildInstance->m_pTimestamp->m_type = concrete::Dimension_Generated::eDimensionTimestamp;
                pChildInstance->m_pTimestamp->m_pUserDimension = pChildInstance;
    
                pInstance->m_children.push_back( pChildInstance->m_pTimestamp );

            }
            else
            {
                pInheritanceNode->m_dimensions.push_back( iFind->second );
            }
        }
        
        for( concrete::Inheritance_Node* pChildInheritance : pInheritanceNode->m_children )
        {
            collateOverrides( pInstance, pChildInheritance, actionInstances, dimensionInstances );
        }
    }
    
    void InterfaceSession::constructInstance( concrete::Action* pInstance )
    {
        constructInheritanceTree( pInstance );
        
        ActionOverrideMap      actionInstances;
        DimensionOverrideMap   dimensionInstances;
        
        collateOverrides( pInstance, pInstance->m_inheritance, actionInstances, dimensionInstances );
        
        for( concrete::Element* pChild : pInstance->m_children )
        {
            if( concrete::Action* pAction = dynamic_cast< concrete::Action* >( pChild ) )
            {
                constructInstance( pAction );
            }
        }
    }
    
    void InterfaceSession::constructAllocator( concrete::Action* pInstance )
    {
        if( pInstance->m_pParent && pInstance->m_pParent->m_pParent )
        {
            pInstance->m_pAllocatorData                = construct< concrete::Dimension_Generated >();
            pInstance->m_pAllocatorData->m_type        = concrete::Dimension_Generated::eActionAllocatorData;
            pInstance->m_pAllocatorData->m_pAction     = pInstance;
            pInstance->m_children.push_back( pInstance->m_pAllocatorData );
        }
        
        std::vector< concrete::Element* > temp = pInstance->m_children;
        for( concrete::Element* pChild : temp )
        {
            if( concrete::Action* pChildAction = dynamic_cast< concrete::Action* >( pChild ) )
            {
                if( pInstance->m_pParent )
                {
                    concrete::Dimension_Generated* pHead = construct< concrete::Dimension_Generated >();
                    pHead->m_type        = concrete::Dimension_Generated::eActionAllocatorHead;
                    pHead->m_pAction     = pChildAction;
                    pInstance->m_allocators.insert( std::make_pair( pChildAction, pHead ) );
                    pInstance->m_children.push_back( pHead );
                }
                
                constructAllocator( pChildAction );
            }
        }
    }
    
    concrete::Action* InterfaceSession::instanceAnalysis()
    {
        const interface::Action* pActionRoot = root< const interface::Action >( getAppendingObjects() );
        
        concrete::Action* pRoot = construct< concrete::Action >();
        pRoot->m_pElement = pActionRoot;
        pRoot->m_pParent = nullptr;
        calculateInstanceActionName( pRoot );
        
        m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pActionRoot, pRoot ) );
        
        constructInstance( pRoot );
        constructAllocator( pRoot );
        
        return pRoot;
    }
    
    
    void InterfaceSession::dependencyAnalysis_recurse( concrete::Action* pAction )
    {
        const interface::Action* pNodeAction = pAction->getAction();
        
        if( !pNodeAction->m_strDependency.empty() && pNodeAction->m_strDependency != "void" )
        {
            //solve dependency - just search for the type
            pAction->m_pDependencyProvider = nullptr;
            std::size_t szDomain = pAction->getLocalDomainSize();
            concrete::Action* pParent = dynamic_cast< concrete::Action* >( pAction->m_pParent );
            for( ; pParent; pParent = dynamic_cast< concrete::Action* >( pParent->m_pParent ) )
            {
                const interface::Action* pParentNodeAction = pParent->getAction();
                if( pParentNodeAction->m_strBaseType == pNodeAction->m_strDependency )
                {
                    pAction->m_pDependencyProvider = pParent;
                    break;
                }
                else
                {
                    szDomain = szDomain * pParent->getLocalDomainSize();
                }
            }
            VERIFY_RTE_MSG( pAction->m_pDependencyProvider, "Failed to locate dependency provider for: " << pNodeAction->m_strDependency );
            VERIFY_RTE( pAction->m_pMappedObject );
            VERIFY_RTE( pAction->m_pDependencyProvider->m_pMappedObject );
            pAction->m_pMappedObject->m_pDependency = pAction->m_pDependencyProvider->m_pMappedObject;
            pAction->m_pMappedObject->dependencyDomain = szDomain;
            
            
        }
        
        for( concrete::Element* pChild : pAction->m_children )
        {
            if( concrete::Action* pChildAction = dynamic_cast< concrete::Action* >( pChild ) )
            {
                dependencyAnalysis_recurse( pChildAction );
            }
        }
    }
    
    void InterfaceSession::dependencyAnalysis()
    {
        concrete::Action* pRoot = root< concrete::Action >( getAppendingObjects() );  
        dependencyAnalysis_recurse( pRoot );
    }
    
    void InterfaceSession::translationUnitAnalysis_recurse( concrete::Action* pAction )
    {
        const interface::Action* pInterfaceAction = pAction->getAction();
        
        if( std::optional< boost::filesystem::path > definitionFileOpt =
                pInterfaceAction->getDefinitionFile() )
        {
            m_pTranslationUnitAnalysis->m_translationUnits[ definitionFileOpt.value() ].insert( pInterfaceAction );
        }
        
        for( concrete::Element* pChild : pAction->m_children )
        {
            if( concrete::Action* pChildAction = dynamic_cast< concrete::Action* >( pChild ) )
            {
                translationUnitAnalysis_recurse( pChildAction );
            }
        }
    }
    
    void InterfaceSession::translationUnitAnalysis()
    {
        concrete::Action* pRoot = root< concrete::Action >( getAppendingObjects() );  
        translationUnitAnalysis_recurse( pRoot );
    }
    
    
}
