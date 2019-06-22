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
        m_pAction = loader.loadObjectRef< interface::Action >();
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
    
    void Element::load( Loader& loader )
    {
        m_pParent = loader.loadObjectRef< Element >();
        m_pElement = loader.loadObjectRef< interface::Element >();
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
    
    void Dimension_User::print( std::ostream& os, std::string& strIndent ) const
    {
        os << strIndent << "dim(" << getIndex() << "): " << getDimension()->getIdentifier() << "\n";
    }
    
    void Dimension_User::printType( std::ostream& os ) const
    {
        const interface::Dimension* pNodeDimension = dynamic_cast< const interface::Dimension* >( m_pElement );
        if( pNodeDimension->getActionTypes().empty() )
        {
            os << pNodeDimension->getCanonicalType();
        }
        else if( pNodeDimension->getActionTypes().size() == 1U )
        {
            const interface::Action* pAction = pNodeDimension->getActionTypes().front();
            os << pAction->getStaticType();
        }
        else
        {
            os << EG_VARIANT_TYPE << "< ";
            for( const interface::Action* pAction : pNodeDimension->getActionTypes() )
            {
                if( pAction != pNodeDimension->getActionTypes().front() )
                    os << ", ";
                os << pAction->getStaticType();
            }
            os << " >";
        }
    }
    int Dimension_User::getDataSize() const
    {
        const interface::Dimension* pNodeDimension = dynamic_cast< const interface::Dimension* >( m_pElement );
        if( pNodeDimension->getActionTypes().empty() )
        {
            return pNodeDimension->getSize();
        }
        else if( pNodeDimension->getActionTypes().size() == 1U )
        {
            const interface::Action* pAction = pNodeDimension->getActionTypes().front();
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
        static const std::string strIndent = "        ";
        
        //use the traits
        const interface::Dimension* pNodeDimension = dynamic_cast< const interface::Dimension* >( m_pElement );
        if( pNodeDimension->getActionTypes().empty() )
        {
            os << strIndent << "::eg::DimensionTraits< " << pNodeDimension->getCanonicalType() << " >::initialise( ";
            printer.printVariableAccess( os, strIndex );
            os << ");\n";
        }
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
    
    void Dimension_Generated::print( std::ostream& os, std::string& strIndent ) const
    {
        THROW_RTE( "Unreachable" );
    }
    
    void Dimension_Generated::printType( std::ostream& os ) const
    {
        switch( m_type )
        {
            case eDimensionTimestamp :
                os << EG_TIME_STAMP;
                break;
            case eActionStopCycle    :
                os << EG_TIME_STAMP;
                break;
            case eActionState        :
                os << EG_ACTION_STATE;
                break;
            case eActionFiber        :
                os << EG_FIBER_TYPE;
                break;
            case eActionObject       :
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
                    
                    os << EG_OBJECT_TRAITS << "< ";
                    
                    os << pAction->getBaseType();
                    
                    os << " >::PtrType";
                }
                break;
            case eActionReference    :
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
                    os << pAction->getStaticType();
                }
                break;
            case eActionAllocatorData:
                os << EG_INSTANCE;
                break;
            case eActionAllocatorHead:
                os << "std::atomic< std::uint64_t >";
                break;
            case eRingIndex:
                os << EG_INSTANCE;
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
                return 4;
            case eActionStopCycle    :
                return 4;
            case eActionState        :
                return 4;
            case eActionFiber        :
                return 4;
            case eActionObject       :
                //use the type traits in the interface
                //TODO - need to report this in link analysis
                return 4;
            case eActionReference    :
                return 12;
            case eActionAllocatorData:
                return 4;
            case eActionAllocatorHead:
                return 8;
            case eRingIndex:
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
                {
                    os << strIndent; 
                    printer.printVariableAccess( os, strIndex ); 
                    os << " = " << EG_INVALID_TIMESTAMP << ";\n";
                }
                break;
            case eActionStopCycle   :
                {
                    os << strIndent; 
                    printer.printVariableAccess( os, strIndex ); 
                    os << " = " << EG_INVALID_TIMESTAMP << ";\n";
                }
                break;
            case eActionState       :
                {
                    os << strIndent; 
                    printer.printVariableAccess( os, strIndex ); 
                    os << " = " << getActionState( action_stopped ) << ";\n";
                }
                break;
            case eActionFiber       : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
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
                    const interface::Action* pAction = m_pAction->getAction();
                    os << strIndent;
                    printer.printVariableAccess( os, strIndex );
                    os << " = " << pAction->getStaticType();
                    os << "( " << EG_REFERENCE_TYPE << " { i, " << m_pAction->getIndex() << ", 0 } );\n";
                }
                break;
            case eActionAllocatorData   : os << strIndent; printer.printVariableAccess( os, strIndex ); os << " = i;\n";   break;
            case eActionAllocatorHead   : os << strIndent; printer.printVariableAccess( os, strIndex ); os << " = 0UL;\n"; break;
            case eRingIndex             : os << strIndent; printer.printVariableAccess( os, strIndex ); os << " = i;\n";   break;
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
            case eActionStopCycle    :
            case eActionState        :
                break;
            case eActionFiber       : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
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
            case eRingIndex             : break;
            default:
                THROW_RTE( "Unknown generated dimension type" );
        }
    }
    void Dimension_Generated::printStart( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const
    {
        static const std::string strIndent = "             ";
        
        switch( m_type )
        {
            case eDimensionTimestamp    :
            case eActionStopCycle       :
            case eActionState           :
            case eActionFiber           : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
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
            case eRingIndex             : break;
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
            case eActionStopCycle       :
            case eActionState           :
            case eActionFiber           : 
                break;
            case eActionObject          : 
                //use the type traits in the interface
                {
                    VERIFY_RTE( m_pAction );
                    const interface::Action* pAction = m_pAction->getAction();
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
            case eRingIndex             : break;
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
        m_pStopCycle        = loader.loadObjectRef< Dimension_Generated >();
        m_pState            = loader.loadObjectRef< Dimension_Generated >();
        m_pFiber            = loader.loadObjectRef< Dimension_Generated >();
        m_pMappedObject     = loader.loadObjectRef< Dimension_Generated >();
        m_pReference        = loader.loadObjectRef< Dimension_Generated >();
        m_pAllocatorData    = loader.loadObjectRef< Dimension_Generated >();
        m_pRingIndex        = loader.loadObjectRef< Dimension_Generated >();
        loader.loadObjectMap( m_allocators );
        m_pDependencyProvider    = loader.loadObjectRef< Action >();
    }
    
    void Action::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.storeObjectRef( m_inheritance );
        storer.store( m_strName );
        storer.store( m_totalDomainSize );
        storer.storeObjectRef( m_pStopCycle     );
        storer.storeObjectRef( m_pState         );
        storer.storeObjectRef( m_pFiber         );
        storer.storeObjectRef( m_pMappedObject  );
        storer.storeObjectRef( m_pReference     );
        storer.storeObjectRef( m_pAllocatorData );
        storer.storeObjectRef( m_pRingIndex     );
        storer.storeObjectMap( m_allocators );
        storer.storeObjectRef( m_pDependencyProvider );
    }

    
    void Action::print( std::ostream& os, std::string& strIndent ) const
    {
        const interface::Action* pAction = getAction();
        if( pAction->isLink() )
        {
            os << strIndent << "link(" << getIndex() << ") " << pAction->getIdentifier() << "\n";
        }
        else
        {
            os << strIndent << "action(" << getIndex() << ") " << pAction->getIdentifier() << "\n";
        }
        
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
        os << getAction()->getStaticType();
    }
    
    int Action::getDataSize() const
    {
        return 0;
    }
    int Action::getLocalDomainSize() const
    {
        const interface::Action* pAction = getAction();
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
