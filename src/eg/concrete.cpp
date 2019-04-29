
#include "concrete.hpp"
#include "codegen.hpp"

namespace eg
{
namespace concrete
{
    std::vector< Element* > getPath( Element* pNode, Element* pFrom /*= nullptr*/ )
    {
        std::vector< Element* > path;
        Element* pNodeIter = pNode; 
        do
        {   VERIFY_RTE( pNodeIter );
            path.push_back( pNodeIter );
            pNodeIter = pNodeIter->getParent();
        }while( pNodeIter != pFrom );
        
        std::reverse( path.begin(), path.end() );
        return path;
    }
    
    std::vector< const Element* > getPath( const Element* pNode, const Element* pFrom /*= nullptr*/ )
    {
        std::vector< const Element* > path;
        const Element* pNodeIter = pNode; 
        do
        {   VERIFY_RTE( pNodeIter );
            path.push_back( pNodeIter );
            pNodeIter = pNodeIter->getParent();
        }while( pNodeIter != pFrom );
        
        std::reverse( path.begin(), path.end() );
        return path;
    }

    void Inheritance_Node::load( Loader& loader )
    {
        m_pRootConcreteAction = loader.loadObjectRef< Action >();
        m_pAction = loader.loadObjectRef< abstract::Action >();
        m_pParent = loader.loadObjectRef< Inheritance_Node >();
        loader.loadObjectVector( m_children );
        loader.loadObjectVector( m_actions );
        loader.loadObjectVector( m_dimensions );
    }
    
    void Inheritance_Node::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_pRootConcreteAction );
        storer.storeObjectRef( m_pAction );
        storer.storeObjectRef( m_pParent );
        storer.storeObjectVector( m_children );
        storer.storeObjectVector( m_actions );
        storer.storeObjectVector( m_dimensions );
    }
    
    const Inheritance_Node* Inheritance_Node::findInstance( const Element* pInstance, 
        const Dimension*& pDimensionResult, Inheritance_Node_SetCst& visited ) const
    {
        if( visited.count( this ) != 0 )
            return nullptr;
        visited.insert( this );
        
        //is the instance this?
        if( m_pAction == pInstance->getAbstractElement() )
            return this;
        
        for( const Dimension* pDimension : m_dimensions )
        {
            if( pDimension == pInstance )
            {
                pDimensionResult = pDimension;
                return nullptr;
            }
        }
        
        for( Action* pAction : m_actions )
        {
            if( pAction == pInstance )
            {
                //return the root inheritance node
                return pAction->m_inheritance;
            }
        }
        
        if( m_pParent )
        {
            const Inheritance_Node* pResult = m_pParent->findInstance( pInstance, pDimensionResult, visited );
            if( pResult || pDimensionResult )
                return pResult;
        }
        
        for( const Inheritance_Node* pChild : m_children )
        {
            const Inheritance_Node* pResult = pChild->findInstance( pInstance, pDimensionResult, visited );
            if( pResult || pDimensionResult )
                return pResult;
        }
        
        return nullptr;
    }
    
    void Element::load( Loader& loader )
    {
        m_pParent = loader.loadObjectRef< Element >();
        m_pElement = loader.loadObjectRef< abstract::Element >();
        loader.loadObjectVector( m_children );
    }
    
    void Element::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_pParent );
        storer.storeObjectRef( m_pElement );
        storer.storeObjectVector( m_children );
    }
    
    void Dimension::load( Loader& loader )
    {
        Element::load( loader );
    }
    
    void Dimension::store( Storer& storer ) const
    {
        Element::store( storer );
    }
    
    int Dimension::getTotalDomainSize() const
    {
        const Action* pParentAction = dynamic_cast< const Action* >( m_pParent );
        VERIFY_RTE( pParentAction );
        return pParentAction->getTotalDomainSize();
    }
    
    void Dimension_User::load( Loader& loader )
    {
        Dimension::load( loader );
        m_pTimestamp = loader.loadObjectRef< Dimension_Generated >();
    }
    
    void Dimension_User::store( Storer& storer ) const
    {
        Dimension::store( storer );
        storer.storeObjectRef( m_pTimestamp );
        
    }
    
    void printActionType( std::ostream& os, const abstract::Action* pElement )
    {
        std::vector< const abstract::Element* > path = ::eg::abstract::getPath( pElement );
        for( const abstract::Element* pNodeIter : path )
        {
            if( pNodeIter != *path.begin())
                os << "::";
            os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
        }
    }
    
    void Dimension_User::print( std::ostream& os, std::string& strIndent ) const
    {
        os << strIndent << "dim(" << getIndex() << "): " << getDimension()->getIdentifier() << "\n";
    }
    
    void Dimension_User::printType( std::ostream& os ) const
    {
        const abstract::Dimension* pNodeDimension = dynamic_cast< const abstract::Dimension* >( m_pElement );
        if( pNodeDimension->getActionTypes().empty() )
        {
            os << pNodeDimension->getCanonicalType();
        }
        else if( pNodeDimension->getActionTypes().size() == 1U )
        {
            printActionType( os, pNodeDimension->getActionTypes().front() );
        }
        else
        {
            os << "__eg_variant< ";
            for( const abstract::Action* pAction : pNodeDimension->getActionTypes() )
            {
                if( pAction != pNodeDimension->getActionTypes().front() )
                    os << ", ";
                printActionType( os, pAction );
            }
            os << " >";
        }
    }
    int Dimension_User::getDataSize() const
    {
        const abstract::Dimension* pNodeDimension = dynamic_cast< const abstract::Dimension* >( m_pElement );
        if( pNodeDimension->getActionTypes().empty() )
        {
            return pNodeDimension->getSize();
        }
        else if( pNodeDimension->getActionTypes().size() == 1U )
        {
            const abstract::Action* pAction = pNodeDimension->getActionTypes().front();
            if( pAction->isIndirectlyAbstract() )
                return 12;
            else
                return 8;
        }
        else
        {
            return 12;
        }
    }
    
    void Dimension_User::printAllocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
    }
    void Dimension_User::printDeallocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
    }
    void Dimension_User::printStart( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
    }
    void Dimension_User::printStop( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
    }
    
    void Dimension_Generated::load( Loader& loader )
    {
        Dimension::load( loader );
        m_pUserDimension = loader.loadObjectRef< Dimension_User >();
        loader.load( m_type );
        m_pAction = loader.loadObjectRef< Action >();
        loader.load( dependencyDomain );
        m_pDependency = loader.loadObjectRef< Dimension_Generated >();
    }
    
    void Dimension_Generated::store( Storer& storer ) const
    {
        Dimension::store( storer );
        storer.storeObjectRef( m_pUserDimension );
        storer.store( m_type );
        storer.storeObjectRef( m_pAction );
        storer.store( dependencyDomain );
        storer.storeObjectRef( m_pDependency );
    }
    
    const std::string& Dimension_Generated::getIdentifier() const
    {
        /*switch( m_type )
        {
            case eDimensionTimestamp :
                break;
            case eActionRunning      :
                break;
            case eActionPaused       :
                break;
            case eActionCoroutine    :
                break;
            case eActionEventIter    :
                break;
            case eActionObject       :
                break;
            case eActionReference    :
                break;
            case eActionAllocatorData:
                break;
            case eActionAllocatorHead:
                break;
            case eActionStopTimestamp:
                break;
        }*/
        THROW_RTE( "Unknown generated dimension type" );
    }
    
    void Dimension_Generated::print( std::ostream& os, std::string& strIndent ) const
    {
        //os << strIndent << "gen dim( " << getIndex() << ")\n";
    }
    void Dimension_Generated::printType( std::ostream& os ) const
    {
        switch( m_type )
        {
            case eDimensionTimestamp :
                os << EG_TIME_STAMP;
                break;
            case eActionRunning      :
            case eActionPaused       :
                os << EG_TIME_STAMP;
                break;
            case eActionCoroutine    :
                os << EG_COROUTINE_TYPE;
                break;
            case eActionEventIter    :
                os << EG_EVENT_ITERATOR;
                break;
            case eActionObject       :
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const abstract::Action* pAction = m_pAction->getAction();
                    
                    os << EG_OBJECT_TRAITS << "< ";
                    
                    os << pAction->getBaseType();
                    
                    //only support single object inheritance for now so it is always __eg_Base_0< void >
                    //printActionType( os, pAction );
                    //os << "::__interface_Base_0< void >::Type";
                    
                    os << " >::PtrType";
                    
                }
                break;
            case eActionReference    :
                {
                    VERIFY_RTE( m_pAction );
                    const abstract::Action* pAction = m_pAction->getAction();
                    printActionType( os, pAction );
                }
                break;
            case eActionAllocatorData:
                os << EG_INSTANCE;
                break;
            case eActionAllocatorHead:
                os << "std::atomic< std::uint64_t >";
                break;
            case eActionStopTimestamp:
                os << EG_TIME_STAMP;
                break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    
    int Dimension_Generated::getDataSize() const
    {
        switch( m_type )
        {
            case eDimensionTimestamp :
            case eActionRunning      :
            case eActionPaused       :
                return 4;
            case eActionCoroutine    :
                return 8;
            case eActionEventIter    :
                return 8;
            case eActionObject       :
                //use the type traits in the interface
                //TODO - need to report this in link analysis
                //{
                //    VERIFY_RTE( m_pAction );
                //    const abstract::Action* pAction = dynamic_cast< const abstract::Action* >( m_pAction->m_pElement );
                //    printActionType( os, pAction );
                //    //only support single object inheritance for now so it is always __interface_Base_0< void >
                //    os << "__interface_Base_0< void >::Size";
                //}
                return 4;
            case eActionReference    :
                return 12;
            case eActionAllocatorData:
                return 4;
            case eActionAllocatorHead:
                return 8;
            case eActionStopTimestamp:
                return 4;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
        
    
    void Dimension_Generated::printAllocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
        static const std::string strIndent = "        ";
        switch( m_type )
        {
            case eDimensionTimestamp :
            case eActionRunning      :
            case eActionPaused       :
                {
                    os << strIndent; 
                    printer.printVariableAccess( os, strIndex ); 
                    os << " = " << EG_INVALID_TIMESTAMP << ";\n";
                }
                break;
            case eActionCoroutine       : 
                break;
            case eActionEventIter       : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const abstract::Action* pAction = m_pAction->getAction();
                    os << strIndent;
                    printer.printVariableAccess( os, strIndex );
                    os << " = " << EG_OBJECT_TRAITS << "< ";
                    os << pAction->getBaseType();
                    if( m_pDependency )
                    {
                        os << " >::Allocate( ";
                        printer.printDependencyVar( os, strIndex );
                        os << " );\n";
                    }
                    else
                    {
                        os << " >::Allocate();\n";
                    }
                }
                break;
            case eActionReference       :
                {
                    VERIFY_RTE( m_pAction );
                    const abstract::Action* pAction = m_pAction->getAction();
                    os << strIndent;
                    printer.printVariableAccess( os, strIndex );
                    os << " = ";
                    printActionType( os, pAction );
                    os << "( " << EG_REFERENCE_TYPE << " { i, " << m_pAction->getIndex() << ", 0 } );\n";
                }
                break;
            case eActionAllocatorData   : os << strIndent; printer.printVariableAccess( os, strIndex ); os << " = i;\n";   break;
            case eActionAllocatorHead   : os << strIndent; printer.printVariableAccess( os, strIndex ); os << " = 0UL;\n"; break;
            case eActionStopTimestamp   : os << strIndent; printer.printVariableAccess( os, strIndex ); os << " = 0U;\n"; break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    
    void Dimension_Generated::printDeallocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
        static const std::string strIndent = "        ";
        switch( m_type )
        {
            case eDimensionTimestamp :
            case eActionRunning      :
            case eActionPaused       :
                break;
            case eActionCoroutine       : 
                break;
            case eActionEventIter       : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const abstract::Action* pAction = m_pAction->getAction();
                    os << strIndent << EG_OBJECT_TRAITS << "< ";
                    os << pAction->getBaseType();
                    if( m_pDependency )
                    {
                        os << " >::Deallocate( "; 
                        printer.printDependencyVar( os, strIndex ); 
                        os << ", "; 
                        printer.printVariableAccess( os, strIndex ); 
                        os << " );\n";
                    }
                    else
                    {
                        os << " >::Deallocate( "; printer.printVariableAccess( os, strIndex ); os << " );\n";
                    }
                }
                break;
            case eActionReference       : break;
            case eActionAllocatorData   : break;
            case eActionAllocatorHead   : break;
            case eActionStopTimestamp   : break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    void Dimension_Generated::printStart( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
        static const std::string strIndent = "                 ";
        
        switch( m_type )
        {
            case eDimensionTimestamp    :
            case eActionRunning         :
            case eActionPaused          :
            case eActionCoroutine       : 
            case eActionEventIter       : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const abstract::Action* pAction = m_pAction->getAction();
                    os << strIndent << EG_OBJECT_TRAITS << "< ";
                    os << pAction->getBaseType();
                    if( m_pDependency )
                    {
                        os << " >::Start( "; 
                        printer.printDependencyVar( os, strIndex ); 
                        os << ", "; 
                        printer.printVariableAccess( os, strIndex ); 
                        os << " );\n";
                    }
                    else
                    {
                        os << " >::Start( "; 
                        printer.printVariableAccess( os, strIndex ); 
                        os << " );\n";
                    }
                }
                break;
            case eActionReference       : break;
            case eActionAllocatorData   : break;
            case eActionAllocatorHead   : break;
            case eActionStopTimestamp   : break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    void Dimension_Generated::printStop( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
        static const std::string strIndent = "         ";
        switch( m_type )
        {
            case eDimensionTimestamp    :
            case eActionRunning         :
            case eActionPaused          :
            case eActionCoroutine       : 
            case eActionEventIter       : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const abstract::Action* pAction = m_pAction->getAction();
                    os << strIndent << EG_OBJECT_TRAITS << "< ";
                    os << pAction->getBaseType();
                    if( m_pDependency )
                    {
                        os << " >::Stop( ";
                        printer.printDependencyVar( os, strIndex ); 
                        os << ", "; 
                        printer.printVariableAccess( os, strIndex ); 
                        os << " );\n";
                    }
                    else
                    {
                        os << " >::Stop( ";
                        printer.printVariableAccess( os, strIndex ); 
                        os << " );\n";
                    }
                }
                break;
            case eActionReference       : break;
            case eActionAllocatorData   : break;
            case eActionAllocatorHead   : break;
            case eActionStopTimestamp   : break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    
    
    void Action::load( Loader& loader )
    {
        Element::load( loader );
        m_inheritance = loader.loadObjectRef< Inheritance_Node >();
        loader.load( m_strName );
        loader.load( m_totalDomainSize );
        m_pRunningTimestamp = loader.loadObjectRef< Dimension_Generated >();
        m_pPauseTimestamp   = loader.loadObjectRef< Dimension_Generated >();
        m_pCoroutine        = loader.loadObjectRef< Dimension_Generated >();
        m_pEventIterator    = loader.loadObjectRef< Dimension_Generated >();
        m_pMappedObject     = loader.loadObjectRef< Dimension_Generated >();
        m_pReference        = loader.loadObjectRef< Dimension_Generated >();
        m_pAllocatorData    = loader.loadObjectRef< Dimension_Generated >();
        m_pStopTimestamp    = loader.loadObjectRef< Dimension_Generated >();
        loader.loadObjectMap( m_allocators );
        m_pDependencyProvider    = loader.loadObjectRef< Action >();
    }
    
    void Action::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.storeObjectRef( m_inheritance );
        storer.store( m_strName );
        storer.store( m_totalDomainSize );
        storer.storeObjectRef( m_pRunningTimestamp );
        storer.storeObjectRef( m_pPauseTimestamp   );
        storer.storeObjectRef( m_pCoroutine        );
        storer.storeObjectRef( m_pEventIterator    );
        storer.storeObjectRef( m_pMappedObject     );
        storer.storeObjectRef( m_pReference        );
        storer.storeObjectRef( m_pAllocatorData    );
        storer.storeObjectRef( m_pStopTimestamp    );
        storer.storeObjectMap( m_allocators );
        storer.storeObjectRef( m_pDependencyProvider );
    }

    
    void Action::print( std::ostream& os, std::string& strIndent ) const
    {
        const abstract::Action* pAction = getAction();
        os << strIndent << "action(" << getIndex() << ") " << pAction->getIdentifier() << "\n";
        
        if( !m_children.empty() )
        {
            os << strIndent << "[\n";
            strIndent.push_back( ' ' );
            strIndent.push_back( ' ' );
            
            for( Element* pChild : m_children )
            {
                pChild->print( os, strIndent );
            }
            
            strIndent.pop_back();
            strIndent.pop_back();
            os << strIndent << "]\n";
        }
    }
    
    void Action::printType( std::ostream& os ) const
    {
        std::vector< const ::eg::abstract::Element* > path = 
            ::eg::abstract::getPath( const_cast< const ::eg::abstract::Element* >( m_pElement ) );
        for( const abstract::Element* pNodeIter : path )
        {
            if( pNodeIter != *path.begin())
                os << "::";
            os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
        }
    }
    
    int Action::getDataSize() const
    {
        return 0;
    }
    int Action::getLocalDomainSize() const
    {
        const abstract::Action* pAction = getAction();
        VERIFY_RTE( pAction );
        return pAction->getSize();
    }
    int Action::getTotalDomainSize() const
    {
        if( 0 == m_totalDomainSize )
        {
            if( const Action* pParentAction = 
                dynamic_cast< const Action* >( m_pParent ) )
            {
                m_totalDomainSize = pParentAction->getTotalDomainSize() * getLocalDomainSize();
            }
            else
            {
                VERIFY_RTE( !m_pParent );
                m_totalDomainSize = getLocalDomainSize();
            }
        }
        return m_totalDomainSize;
    }


} //namespace concrete
} //namespace eg
