

#include "interface_session.hpp"

#include "abstract.hpp"

namespace eg
{

    InterfaceSession::InterfaceSession( const boost::filesystem::path& treePath )
        :   AppendingSession( treePath, IndexedObject::MASTER_FILE )
    {
        m_pDerivationAnalysis = construct< DerivationAnalysis >();
    }
    
    
    void InterfaceSession::interfaceAnalysis()
    {
        
    }

    void InterfaceSession::linkAnalysis()
    {
        //calculate inheritance
        {
            std::vector< abstract::Action* > actions = many< abstract::Action >( getMaster() );
            for( abstract::Action* pAction : actions )
            {
                for( abstract::Action* pBase : pAction->m_baseActions )
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

    
    void collateChildren( const abstract::Action* pAction, 
            std::vector< const abstract::Action* >& actions, 
            std::vector< const abstract::Dimension* >& dimensions )
    {
        for( const abstract::Element* pObject : pAction->getChildren() )
        {
            switch( pObject->getType() )
            {
                case eAbstractDimension : 
                    dimensions.push_back( dynamic_cast< const abstract::Dimension* >( pObject ) );
                    break;
                case eAbstractRoot      : 
                case eAbstractAction    :  
                    if( const abstract::Action* pChildAction = dynamic_cast< const abstract::Action* >( pObject ) )
                    {
                        if( !pChildAction->isAbstract() )
                        {
                            actions.push_back( dynamic_cast< const abstract::Action* >( pObject ) );
                        }
                    }
                    break;
            }
        }
    }
    
    void InterfaceSession::collateBases( concrete::Action* pInstance, concrete::Inheritance_Node* pInheritanceNode )
    {
        for( abstract::Action* pBase : pInheritanceNode->m_pAction->m_baseActions )
        {
            concrete::Inheritance_Node* pChildInheritanceNode = construct< concrete::Inheritance_Node >(); 
            pInheritanceNode->m_children.push_back( pChildInheritanceNode );
            
            pChildInheritanceNode->m_pInstance  = pInstance;
            pChildInheritanceNode->m_pParent    = pInheritanceNode;
            pChildInheritanceNode->m_pAction    = pBase;
            
            m_pDerivationAnalysis->m_inheritanceMap.insert( std::make_pair( pBase, pChildInheritanceNode ) );
            
            collateBases( pInstance, pChildInheritanceNode );
        }
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
        std::map< abstract::Action*, concrete::Action*, CompareNodeIdentity< abstract::Action > >;
    using DimensionOverrideMap = 
        std::map< abstract::Dimension*, concrete::Dimension*, CompareNodeIdentity< abstract::Dimension > >;
        
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
        std::vector< const abstract::Action* > actions;
        std::vector< const abstract::Dimension* > dimensions;
        collateChildren( pInheritanceNode->m_pAction, actions, dimensions );
        
        for( const abstract::Action* pChildAction : actions )
        {
            std::map< const abstract::Action*, concrete::Action* >::iterator iFind = 
                actionInstances.find( pChildAction );
            if( iFind == actionInstances.end() )
            {
                //create the child instance node 
                concrete::Action* pChildInstance = construct< concrete::Action >();
                pInstance->m_children.push_back( pChildInstance );
                pChildInstance->m_pElement = pChildAction;
                pChildInstance->m_pParent = pInstance;
                calculateInstanceActionName( pChildInstance );
                
                pChildInstance->m_inheritance = construct< concrete::Inheritance_Node >(); 
                pChildInstance->m_inheritance->m_pInstance = pChildInstance;
                pChildInstance->m_inheritance->m_pAction = pChildAction;
                
                m_pDerivationAnalysis->m_inheritanceMap.insert( std::make_pair( pChildAction, pChildInstance->m_inheritance ) );
                m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pChildAction, pChildInstance ) );
                
                //record it in the inheritance node
                pInheritanceNode->m_actions.push_back( pChildInstance );
                
                actionInstances.insert( std::make_pair( pChildAction, pChildInstance ) );
                
                pChildInstance->m_pRunningTimestamp             = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pRunningTimestamp->m_type     = concrete::Dimension_Generated::eActionRunning;
                pChildInstance->m_pRunningTimestamp->m_pAction  = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pRunningTimestamp );
                
                pChildInstance->m_pPauseTimestamp               = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pPauseTimestamp->m_type       = concrete::Dimension_Generated::eActionPaused;
                pChildInstance->m_pPauseTimestamp->m_pAction    = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pPauseTimestamp );
                
                pChildInstance->m_pCoroutine                    = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pCoroutine->m_type            = concrete::Dimension_Generated::eActionCoroutine;
                pChildInstance->m_pCoroutine->m_pAction         = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pCoroutine );
                
                pChildInstance->m_pEventIterator                = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pEventIterator->m_type        = concrete::Dimension_Generated::eActionEventIter;
                pChildInstance->m_pEventIterator->m_pAction     = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pEventIterator );
                
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
                
                
                pChildInstance->m_pStopTimestamp                = construct< concrete::Dimension_Generated >();
                pChildInstance->m_pStopTimestamp->m_type       = concrete::Dimension_Generated::eActionStopTimestamp;
                pChildInstance->m_pStopTimestamp->m_pAction    = pChildInstance;
                pChildInstance->m_children.push_back( pChildInstance->m_pStopTimestamp );
                
            }
            else
            {
                pInheritanceNode->m_actions.push_back( iFind->second );
            }
        }
        
        for( const abstract::Dimension* pChildDimension : dimensions )
        {
            std::map< const abstract::Dimension*, concrete::Dimension* >::iterator iFind = 
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
        collateBases( pInstance, pInstance->m_inheritance );
        
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
        
        for( concrete::Element* pChild : pInstance->m_children )
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
        const abstract::Action* pActionRoot = root< const abstract::Action >( getAppendingObjects() );
        
        concrete::Action* pRoot = construct< concrete::Action >();
        pRoot->m_pElement = pActionRoot;
        pRoot->m_pParent = nullptr;
        calculateInstanceActionName( pRoot );
        pRoot->m_inheritance = construct< concrete::Inheritance_Node >(); 
        pRoot->m_inheritance->m_pInstance = pRoot;
        pRoot->m_inheritance->m_pAction = pActionRoot;
        
        m_pDerivationAnalysis->m_inheritanceMap.insert( std::make_pair( pActionRoot, pRoot->m_inheritance ) );
        m_pDerivationAnalysis->m_instanceMap.insert( std::make_pair( pActionRoot, pRoot ) );
        
        constructInstance( pRoot );
        constructAllocator( pRoot );
        
        return pRoot;
    }
    
    
    void InterfaceSession::dependencyAnalysis_recurse( concrete::Action* pAction )
    {
        const abstract::Action* pNodeAction = pAction->getAction();
        
        if( !pNodeAction->m_strDependency.empty() && pNodeAction->m_strDependency != "void" )
        {
            //solve dependency - just search for the type
            pAction->m_pDependencyProvider = nullptr;
            std::size_t szDomain = pAction->getLocalDomainSize();
            concrete::Action* pParent = dynamic_cast< concrete::Action* >( pAction->m_pParent );
            for( ; pParent; pParent = dynamic_cast< concrete::Action* >( pParent->m_pParent ) )
            {
                const abstract::Action* pParentNodeAction = pParent->getAction();
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
    
}
