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


#include "eg_compiler/concrete.hpp"
#include "eg_compiler/allocator.hpp"
#include "eg_compiler/link.hpp"

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
        m_pContext = loader.loadObjectRef< interface::Context >();
        m_pParent = loader.loadObjectRef< Inheritance_Node >();
        loader.loadObjectVector( m_children );
        loader.loadObjectVector( m_actions );
        loader.loadObjectVector( m_dimensions );
    }
    
    void Inheritance_Node::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_pRootConcreteAction );
        storer.storeObjectRef( m_pContext );
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
    
    void Dimension_User::load( Loader& loader )
    {
        Dimension::load( loader );
        m_pLinkGroup = loader.loadObjectRef< LinkGroup >();
    }
    
    void Dimension_User::store( Storer& storer ) const
    {
        Dimension::store( storer );
        storer.storeObjectRef( m_pLinkGroup );
    }
    
    void Dimension_User::print( std::ostream& os, std::string& strIndent ) const
    {
        os << strIndent << "dim(" << getIndex() << "): " << getDimension()->getIdentifier() << "\n";
    }
    
    void Dimension_Generated::load( Loader& loader )
    {
        Dimension::load( loader );
        loader.load( m_type );
        m_pContext = loader.loadObjectRef< Action >();
        m_pLinkGroup = loader.loadObjectRef< LinkGroup >();
    }
    
    void Dimension_Generated::store( Storer& storer ) const
    {
        Dimension::store( storer );
        storer.store( m_type );
        storer.storeObjectRef( m_pContext );
        storer.storeObjectRef( m_pLinkGroup );
    }
    
    void Dimension_Generated::print( std::ostream& os, std::string& strIndent ) const
    {
        //do nothing
    }
		
    void Action::load( Loader& loader )
    {
        Element::load( loader );
        m_pObject = loader.loadObjectRef< Action >();
        m_inheritance = loader.loadObjectRef< Inheritance_Node >();
        m_pAllocator = loader.loadObjectRef< Allocator >();
        loader.load( m_strName );
        loader.load( m_totalDomainSize );
        m_pStopCycle        = loader.loadObjectRef< Dimension_Generated >();
        m_pState            = loader.loadObjectRef< Dimension_Generated >();
        m_pReference        = loader.loadObjectRef< Dimension_Generated >();
        m_pLinkRefCount     = loader.loadObjectRef< Dimension_Generated >();
        loader.loadObjectMap( m_allocators );
        loader.loadKeyObjectMap( m_links );
    }
    
    void Action::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.storeObjectRef( m_pObject );
        storer.storeObjectRef( m_inheritance );
        storer.storeObjectRef( m_pAllocator );
        storer.store( m_strName );
        storer.store( m_totalDomainSize );
        storer.storeObjectRef( m_pStopCycle     );
        storer.storeObjectRef( m_pState         );
        storer.storeObjectRef( m_pReference     );
        storer.storeObjectRef( m_pLinkRefCount  );
        storer.storeObjectMap( m_allocators );
        storer.storeKeyObjectMap( m_links );
    }

    void Action::print( std::ostream& os, std::string& strIndent ) const
    {
        const interface::Context* pContext = getContext();
        VERIFY_RTE( pContext );
        
        os << strIndent << pContext->getContextType() << 
            "(" << getIndex() << ") " << pContext->getIdentifier() << "[ " << getTotalDomainSize() << " ]";
            
        if( const interface::Root* pIsRoot = dynamic_cast< const interface::Root* >( getContext() ) )
        {
            os << " " << pIsRoot->getRootType();
        }
        os << "\n";
        
        if( !m_children.empty() )
        {
            std::ostringstream osNested;
            strIndent.push_back( ' ' );
            strIndent.push_back( ' ' );
            for( Element* pChild : m_children )
            {
                pChild->print( osNested, strIndent );
            }
            strIndent.pop_back();
            strIndent.pop_back();
            std::string strNested = osNested.str();
            if( !strNested.empty() )
            {
                os << strIndent << "[\n";
                os << strNested;
                os << strIndent << "]\n";
            }
        }
    }
    
    int Action::getLocalDomainSize() const
    {
        const interface::Context* pAction = getContext();
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
    int Action::getObjectDomainFactor() const
    {
        VERIFY_RTE( m_pObject );
        
        int iDomainFactor = 1;
        for( const concrete::Action* pIter = this; pIter != m_pObject; pIter = dynamic_cast< const concrete::Action* >( pIter->getParent() ) )
        {
            VERIFY_RTE( pIter );
            iDomainFactor *= pIter->getLocalDomainSize();
        }
        
        return iDomainFactor;
    }
    
    const Dimension_User* Action::getLinkBaseDimension() const
    {
        const Dimension_User* pDimension = nullptr;
        
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eConcreteAction:
                case eConcreteDimensionGenerated:
                    break;
                case eConcreteDimensionUser :  
                    {
                        Dimension_User* pDim = dynamic_cast< Dimension_User* >( pElement );
                        if( pDim->getDimension()->getIdentifier() == EG_LINK_DIMENSION )
                        {
                            VERIFY_RTE( pDimension == nullptr );
                            pDimension = pDim;
                        }
                    }
                    break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
        
        return pDimension;
    }
    
    bool Action::hasUserDimensions() const
    {
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eConcreteAction:
                case eConcreteDimensionGenerated:
                    break;
                case eConcreteDimensionUser :  
                    return true;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
        return false;
    }

} //namespace concrete
} //namespace eg
