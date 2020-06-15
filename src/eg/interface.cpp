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


#include "eg_compiler/interface.hpp"
#include "eg_compiler/eg.hpp"

namespace eg
{
namespace interface
{
    std::vector< Element* > getPath( Element* pNode, Element* pFrom /*= nullptr*/ )
    {
        std::vector< Element* > path;
        if( Element* pNodeIter = pNode )
        {
            do
            {   
                path.push_back( pNodeIter );
                pNodeIter = pNodeIter->getParent();
            }while( pNodeIter && ( pNodeIter != pFrom ) && pNodeIter->getInputElement() );
            std::reverse( path.begin(), path.end() );
        }
        return path;
    }
    
    std::vector< const Element* > getPath( const Element* pNode, const Element* pFrom /*= nullptr*/ )
    {
        std::vector< const Element* > path;
        if( const Element* pNodeIter = pNode )
        {
            do
            {   
                path.push_back( pNodeIter );
                pNodeIter = pNodeIter->getParent();
            }while( pNodeIter && ( pNodeIter != pFrom ) && pNodeIter->getInputElement() );
            std::reverse( path.begin(), path.end() );
        }
        return path;
    }
    
    Element::Element( const IndexedObject& object, Element* pParent, input::Element* pElement )
        :   IndexedObject( object ),
            m_pElement( pElement ),
            m_pParent( pParent ),
            pIncludeIdentifier( nullptr )
    {

    }
    
    void Element::load( Loader& loader )
    {
        m_pElement          = loader.loadObjectRef< input::Element >();
        m_pParent           = loader.loadObjectRef< Element >();
        pIncludeIdentifier  = loader.loadObjectRef< input::Include >();
        loader.loadObjectVector( m_children );
    }

    void Element::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_pElement );
        storer.storeObjectRef( m_pParent );
        storer.storeObjectRef( pIncludeIdentifier );
        storer.storeObjectVector( m_children );
    }
    
    bool CompareElements( const Element* pLeft, const Element* pRight )
    {
        if( pLeft->getType() != pRight->getType() )
        {
            return pLeft->getType() < pRight->getType();
        }
        else if( pLeft->getIdentifier() != pRight->getIdentifier() )
        {
            return pLeft->getIdentifier() < pRight->getIdentifier(); 
        }
        else
        {
            THROW_RTE( "Duplicate identifiers: " << pLeft->getIdentifier() );
        }
    }
    /*
    bool Element::update( const Element* pNewElement )
    {
        bool bModified = false;
        
        std::vector< Opaque* > opaques;
        std::vector< Element* > children;
        {
            for( Element* pElement : m_children )
            {
                if( Opaque* pOpaque = dynamic_cast< Opaque* >( pElement ) )
                {
                    if( !pOpaque->isSemantic() )
                    {
                        opaques.push_back( pOpaque );
                    }
                }
                else
                {
                    children.push_back( pElement );
                }
            }
            std::sort( children.begin(), children.end(), CompareElements);
        }
        
        std::vector< const Opaque* > newOpaques;
        std::vector< const Element* > newChildren;
        {
            const std::vector< Element* >& allNewChildren = pNewElement->getChildren();
            for( const Element* pElement : allNewChildren )
            {
                if( const Opaque* pOpaque = dynamic_cast< const Opaque* >( pElement ) )
                {
                    if( !pOpaque->isSemantic() )
                    {
                        newOpaques.push_back( pOpaque );
                    }
                }
                else
                {
                    newChildren.push_back( pElement );
                }
            }
            std::sort( newChildren.begin(), newChildren.end(), CompareElements );
        }
        
        if( std::equal( children.begin(), children.end(), newChildren.begin(), newChildren.end(), 
            []( Element* pOldElement, const Element* pNewElement )
            {
                if( ( pOldElement->getType() == pNewElement->getType() ) && 
                    ( pOldElement->getIdentifier() == pNewElement->getIdentifier() ) )
                {
                    return !pOldElement->update( pNewElement );
                }
                else
                {
                    return false;
                }
            } ) )
        {
            //structure of the tree is equal so we merge the opaques
            if( opaques.size() == 1U && newOpaques.size() == 1U ) 
            {
                Opaque* pOldOpaque = opaques[ 0U ];
                const Opaque* pNewOpaque = newOpaques[ 0U ];
                pOldOpaque->modify( pNewOpaque );
            }
            else
            {
                VERIFY_RTE_MSG( opaques.size() == 0U && newOpaques.size() == 0U,
                    "Incorrect number of body elements found" );
            }
        }
        else
        {
            bModified = true;
        }
            
        
        return bModified;
    }*/
    
    void Element::print( std::ostream& os, std::string& strIndent, bool bIncludeOpaque ) const
    {
        if( m_pElement )
        {
            switch( m_pElement->getType() )
            {
                case eInputOpaque    :
                    if( bIncludeOpaque )
                    {
                        std::ostringstream osAnnotation;
                        osAnnotation << getIndex();
                        m_pElement->print( os, strIndent, osAnnotation.str() );
                    }
                    break;
                case eInputDimension :
                case eInputInclude   :
                case eInputUsing     :
                case eInputExport    :
                    {
                        std::ostringstream osAnnotation;
                        osAnnotation << getIndex();
                        m_pElement->print( os, strIndent, osAnnotation.str() );
                        break;
                    }
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
        else
        {
            for( const Element* pChildNode : m_children )
            {
                pChildNode->print( os, strIndent, bIncludeOpaque );
            }
        }
    }

    const std::string& Element::getIdentifier() const
    {
        if( pIncludeIdentifier )
        {
            return pIncludeIdentifier->getIdentifier();
        }
        else if( !m_pElement )
        {
            static const std::string strEmpty;
            return strEmpty;
        }
        else
        {
            switch( m_pElement->getType() )
            {
                case eInputDimension :  return dynamic_cast< input::Dimension* >(  m_pElement )->getIdentifier();
                case eInputInclude   :  return dynamic_cast< input::Include* >(    m_pElement )->getIdentifier();
                case eInputUsing     :  return dynamic_cast< input::Using* >(      m_pElement )->getIdentifier();
                case eInputExport    :  return dynamic_cast< input::Export* >(     m_pElement )->getIdentifier();
                case eInputRoot      :  return dynamic_cast< input::Root* >(       m_pElement )->getIdentifier();
                case eInputContext   :  return dynamic_cast< input::Context* >(    m_pElement )->getIdentifier();
                    break;
                case eInputOpaque    :
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
    }
    
    std::string Element::getFriendlyName() const
    {
        std::ostringstream os;
        
        const std::vector< const Element* > path = getPath( this );
        for( const Element* pElement : path )
        {
            if( pElement != *path.begin())
                os << "::";
            os << pElement->getIdentifier();
        }
        
        return os.str();
    }
    
    
    Opaque::Opaque( const IndexedObject& indexedObject )
        :   Element( indexedObject, nullptr, nullptr )
    {
    }
    Opaque::Opaque( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Element( indexedObject, pParent, pElement ),
            m_pOpaque( dynamic_cast< input::Opaque* >( pElement ) )
    {
        VERIFY_RTE( m_pOpaque );
    }
    
    
    void Opaque::load( Loader& loader )
    {
        Element::load( loader );
        if( m_pElement )
        {
            m_pOpaque = dynamic_cast< input::Opaque* >( m_pElement );
            VERIFY_RTE( m_pOpaque );
        }
    }

    void Opaque::store( Storer& storer ) const
    {
        Element::store( storer );
    }
    
    
    Dimension::Dimension( const IndexedObject& indexedObject )
        :   Element( indexedObject, nullptr, nullptr )
    {
    }
    Dimension::Dimension( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Element( indexedObject, pParent, pElement ),
            m_pDimension( dynamic_cast< input::Dimension* >( pElement ) )
    {
        VERIFY_RTE( m_pDimension );
    }

    void Dimension::load( Loader& loader )
    {
        Element::load( loader );
        if( m_pElement )
        {
            m_pDimension = dynamic_cast< input::Dimension* >( m_pElement );
            VERIFY_RTE( m_pDimension );
        }
        loader.loadObjectVector( m_contextTypes );
        loader.load( m_canonicalType );
        loader.load( m_size );
    }
    void Dimension::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.storeObjectVector( m_contextTypes );
        storer.store( m_canonicalType );
        storer.store( m_size );
    }
    
    /*bool Dimension::update( const Element* pElement )
    {
        if( const Dimension* pNewDimension = dynamic_cast< const Dimension* >( pElement ) )
        {
            //require the opaque is equal
            if( m_pDimension->equal( *pNewDimension->m_pDimension ) )
            {
                return Element::update( pElement );
            }
        }
        return true;
    }*/
    
    const std::string& Dimension::getType() const
    {
        return m_pDimension->getType()->getStr();
    }
    bool Dimension::isHomogenous( const std::vector< const Dimension* >& dimensions )
    {
        if( dimensions.size() > 1 )
        {
            const Dimension* pFirst = dimensions.front();
            std::set< const Context* > cmp( 
                pFirst->m_contextTypes.begin(), 
                pFirst->m_contextTypes.end() );
            for( std::vector< const Dimension* >::const_iterator 
                i       = dimensions.begin() + 1,
                iEnd    = dimensions.end(); i!=iEnd; ++i )
            {
                if( pFirst->m_canonicalType != (*i)->m_canonicalType )
                    return false;
                else 
                {
                    std::set< const Context* > cmp2( 
                        (*i)->m_contextTypes.begin(), 
                        (*i)->m_contextTypes.end() );
                    if( cmp != cmp2 )
                        return false;
                }
            }
        }
        return true;
    }

    
    Using::Using( const IndexedObject& indexedObject )
        :   Element( indexedObject, nullptr, nullptr )
    {
    }
    Using::Using( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Element( indexedObject, pParent, pElement ),
            m_pUsing( dynamic_cast< input::Using* >( pElement ) )
    {
        VERIFY_RTE( m_pUsing );
    }

    void Using::load( Loader& loader )
    {
        Element::load( loader );
        if( m_pElement )
        {
            m_pUsing = dynamic_cast< input::Using* >( m_pElement );
            VERIFY_RTE( m_pUsing );
        }
        loader.load( m_canonicalType );
    }
    void Using::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.store( m_canonicalType );
    }
    /*bool Using::update( const Element* pElement )
    {
        if( const Using* pNewUsing = dynamic_cast< const Using* >( pElement ) )
        {
            //require the opaque is equal
            if( m_pUsing->equal( *pNewUsing->m_pUsing ) )
            {
                return Element::update( pElement );
            }
        }
        return true;
    }*/
    const std::string& Using::getType() const
    {
        return m_pUsing->getType()->getStr();
    }

    
    Export::Export( const IndexedObject& indexedObject )
        :   Element( indexedObject, nullptr, nullptr )
    {
    }
    Export::Export( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Element( indexedObject, pParent, pElement ),
            m_pExport( dynamic_cast< input::Export* >( pElement ) )
    {
        VERIFY_RTE( m_pExport );
    }

    void Export::load( Loader& loader )
    {
        Element::load( loader );
        if( m_pElement )
        {
            m_pExport = dynamic_cast< input::Export* >( m_pElement );
            VERIFY_RTE( m_pExport );
        }
    }
    void Export::store( Storer& storer ) const
    {
        Element::store( storer );
    }
    /*bool Export::update( const Element* pElement )
    {
        if( const Export* pNewExport = dynamic_cast< const Export* >( pElement ) )
        {
            //require the opaque is equal
            if( m_pExport->equal( *pNewExport->m_pExport ) )
            {
                return Element::update( pElement );
            }
        }
        return true;
    }*/
    const std::string& Export::getReturnType() const
    {
        return m_pExport->getReturnType()->getStr();
    }
    const std::string& Export::getParameters() const
    {
        return m_pExport->getParameters()->getStr();
    }
    const std::string& Export::getBody() const
    {
        return m_pExport->getBody()->getStr();
    }
    
    Include::Include( const IndexedObject& indexedObject )
        :   Element( indexedObject, nullptr, nullptr )
    {
    }
    Include::Include( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Element( indexedObject, pParent, pElement ),
            m_pInclude( dynamic_cast< input::Include* >( pElement ) )
    {
        VERIFY_RTE( m_pInclude );
    }
    
    void Include::load( Loader& loader )
    {
        Element::load( loader );
        if( m_pElement )
        {
            m_pInclude = dynamic_cast< input::Include* >( m_pElement );
            VERIFY_RTE( m_pInclude );
        }
    }

    void Include::store( Storer& storer ) const
    {
        Element::store( storer );
    }
    /*bool Include::update( const Element* pElement )
    {
        if( const Include* pNewInclude = dynamic_cast< const Include* >( pElement ) )
        {
            //require the opaque is equal
            if( m_pInclude->equal( *pNewInclude->m_pInclude ) )
            {
                return Element::update( pElement );
            }
        }
        return true;
    }*/
    
    
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    
    Context::Context( const IndexedObject& indexedObject )
        :   Element( indexedObject, nullptr, nullptr )
    {
    }
    Context::Context( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Element( indexedObject, pParent, pElement ),
            m_pContext( dynamic_cast< input::Context* >( pElement ) )
    {
        if( pElement )
        {
            VERIFY_RTE( m_pContext );
        }
    }

    void Context::load( Loader& loader )
    {
        Element::load( loader );
        if( m_pElement )
        {
            m_pContext = dynamic_cast< input::Context* >( m_pElement );
            VERIFY_RTE( m_pContext );
        }
        loader.loadObjectVector( m_baseContexts );
        loader.load( m_parameterTypes );
        loader.load( m_size );
        loader.loadOptional( m_definitionFile );
        loader.loadOptional( m_bIndirectlyAbstract );
    }
    
    void Context::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.storeObjectVector( m_baseContexts );
        storer.store( m_parameterTypes );
        storer.store( m_size );
        storer.storeOptional( m_definitionFile );
        storer.storeOptional( m_bIndirectlyAbstract );
    }
    void Context::getDimensions( std::vector< Dimension* >& dimensions ) const
    {
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eAbstractOpaque    :  break;
                case eAbstractDimension :  dimensions.push_back( dynamic_cast< Dimension* >( pElement ) ); break;
                case eAbstractInclude   :  break;
                case eAbstractUsing     :  break;
                case eAbstractExport    :  break;
                case eAbstractAbstract  :  break;
                case eAbstractEvent     :  break;
                case eAbstractFunction  :  break;
                case eAbstractAction    :  break;
                case eAbstractObject    :  break;
                case eAbstractLink      :  break;
                case eAbstractRoot      :  break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
    }
    
    void Context::getUsings( std::vector< Using* >& usings ) const
    {
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eAbstractOpaque    :  break;
                case eAbstractDimension :  break;
                case eAbstractInclude   :  break;
                case eAbstractUsing     :  usings.push_back( dynamic_cast< Using* >( pElement ) ); break;
                case eAbstractExport    :  break;
                case eAbstractAbstract  :  break;
                case eAbstractEvent     :  break;
                case eAbstractFunction  :  break;
                case eAbstractAction    :  break;
                case eAbstractObject    :  break;
                case eAbstractLink      :  break;
                case eAbstractRoot      :  break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
    }
    
    void Context::getExports( std::vector< Export* >& exports ) const
    {
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eAbstractOpaque    :  break;
                case eAbstractDimension :  break;
                case eAbstractInclude   :  break;
                case eAbstractUsing     :  break;
                case eAbstractExport    :  exports.push_back( dynamic_cast< Export* >( pElement ) ); break;
                case eAbstractAbstract  :  break;
                case eAbstractEvent     :  break;
                case eAbstractFunction  :  break;
                case eAbstractAction    :  break;
                case eAbstractObject    :  break;
                case eAbstractLink      :  break;
                case eAbstractRoot      :  break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
    }
    std::size_t Context::getInputBaseCount() const
    {
        if( m_pContext )
        {
            return m_pContext->getInheritance().size();
        }
        else
        {
            return 0U;
        }
    }
    void Context::getChildContexts( std::vector< Context* >& actions ) const
    {
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eAbstractOpaque    :  break;
                case eAbstractDimension :  break;
                case eAbstractInclude   :  break;
                case eAbstractUsing     :  break;
                case eAbstractExport    :  break;
                case eAbstractAbstract  :  
                case eAbstractEvent     :  
                case eAbstractFunction  :  
                case eAbstractAction    :  
                case eAbstractObject    :  
                case eAbstractLink      :  
                case eAbstractRoot      :  actions.push_back( dynamic_cast< Context* >( pElement ) ); break;
                
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
    }
    bool Context::isIndirectlyAbstract() const
    {
        if( !m_bIndirectlyAbstract )
        {
            if( isAbstract() )
            {
                m_bIndirectlyAbstract = true;
            }
            else if( const Context* pParent = 
                dynamic_cast< const Context* >( m_pParent ) )
            {
                m_bIndirectlyAbstract = pParent->isIndirectlyAbstract();
            }
            else
            {
                m_bIndirectlyAbstract = false;
            }
        }
        
        return m_bIndirectlyAbstract.value();
    }
    
    bool Context::isAbstract() const
    {
		return false;
    }
	bool Context::isExecutable() const
	{
		return true;
	}
		
	bool Context::isMainExecutable() const
	{
		return false;
	}
    const interface::Dimension* Context::getLinkBaseDimension() const
    {
        const interface::Dimension* pDimension = nullptr;
        
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eAbstractOpaque    :  break;
                case eAbstractDimension :  
                    {
                        Dimension* pDim = dynamic_cast< Dimension* >( pElement );
                        if( pDim->getIdentifier() == EG_LINK_DIMENSION )
                        {
                            VERIFY_RTE( pDimension == nullptr );
                            pDimension = pDim;
                        }
                    }
                    break;
                case eAbstractInclude   :  break;
                case eAbstractUsing     :  break;
                case eAbstractExport    :  break;
                case eAbstractAbstract  :  break;
                case eAbstractEvent     :  break;
                case eAbstractFunction  :  break;
                case eAbstractAction    :  break;
                case eAbstractObject    :  break;
                case eAbstractLink      :  break;
                case eAbstractRoot      :  break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
        
        return pDimension;
    }			
    
    void Context::print( std::ostream& os, std::string& strIndent, bool bIncludeOpaque ) const
    {
        if( m_pElement )
        {
            switch( m_pElement->getType() )
            {
                case eAbstractOpaque    :  break;
                case eAbstractDimension :  break;
                case eAbstractInclude   :  break;
                case eAbstractUsing     :  break;
                case eAbstractExport    :  break;
                case eAbstractAbstract  :  
                case eAbstractEvent     :  
                case eAbstractFunction  :  
                case eAbstractAction    :  
                case eAbstractObject    :  
                case eAbstractLink      :  
                case eAbstractRoot      :  
                
                    {
                        input::Context* pContext = dynamic_cast< input::Context* >( m_pElement );
                        VERIFY_RTE( pContext );
                        {
                            std::ostringstream osAnnotation;
                            osAnnotation << getIndex();
                            if( m_definitionFile )
                            {
                                osAnnotation << " " << m_definitionFile.value();
                            }
							if( const Root* pIsRoot = dynamic_cast< const Root* >( this ) )
							{
                                osAnnotation << " " << pIsRoot->getRootType();
							}
                            
                            input::printDeclaration( os, strIndent, pContext->getContextType(), getIdentifier(), 
                                pContext->getReturnType(), pContext->getParams(), 
                                pContext->getSize(), pContext->getInheritance(), osAnnotation.str() );
                        }
                        
                        std::ostringstream osNested;
                        {
                            strIndent.push_back( ' ' );
                            strIndent.push_back( ' ' );
                            for( const Element* pChildNode : m_children )
                            {
                                pChildNode->print( osNested, strIndent, bIncludeOpaque );
                            }
                            strIndent.pop_back();
                            strIndent.pop_back();
                        }
                        
                        const std::string str = osNested.str();
                        if( !str.empty() )
                        {
                            os << "\n" << strIndent << "{\n" << str << strIndent << "}\n";
                        }
                        else
                        {
                            os << "\n";
                        }
                    }
                    break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
        else
        {
            for( const Element* pChildNode : m_children )
            {
                pChildNode->print( os, strIndent, bIncludeOpaque );
            }
        }
    }
    
    /*bool Action::update( const Element* pElement )
    {
        if( const Action* pNewAction = dynamic_cast< const Action* >( pElement ) )
        {
            //require the opaque is equal
            if( m_pContext->equal( *pNewAction->m_pContext ) )
            {
                return Element::update( pElement );
            }
        }
        return true;
    }*/
    
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    Abstract::Abstract( const IndexedObject& indexedObject )
        :   Context( indexedObject, nullptr, nullptr )
    {
    }
    Abstract::Abstract( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Context( indexedObject, pParent, pElement )
    {
    }
    void Abstract::load( Loader& loader )
    {
        Context::load( loader );
    }
    void Abstract::store( Storer& storer ) const
    {
        Context::store( storer );
    }
    bool Abstract::isAbstract() const
    {
        return true;
    }
    
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    Event::Event( const IndexedObject& indexedObject )
        :   Context( indexedObject, nullptr, nullptr )
    {
    }
    Event::Event( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Context( indexedObject, pParent, pElement )
    {
    }
    void Event::load( Loader& loader )
    {
        Context::load( loader );
    }
    void Event::store( Storer& storer ) const
    {
        Context::store( storer );
    }
    
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    Function::Function( const IndexedObject& indexedObject )
        :   Context( indexedObject, nullptr, nullptr )
    {
    }
    Function::Function( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Context( indexedObject, pParent, pElement )
    {
    }
    void Function::load( Loader& loader )
    {
        Context::load( loader );
    }
    void Function::store( Storer& storer ) const
    {
        Context::store( storer );
    }
    
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    Action::Action( const IndexedObject& indexedObject )
        :   Context( indexedObject, nullptr, nullptr )
    {
    }
    Action::Action( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Context( indexedObject, pParent, pElement )
    {
    }

    void Action::load( Loader& loader )
    {
        Context::load( loader );
    }
    
    void Action::store( Storer& storer ) const
    {
        Context::store( storer );
    }
    
    
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    Object::Object( const IndexedObject& indexedObject )
        :   Context( indexedObject, nullptr, nullptr )
    {
    }
    Object::Object( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Context( indexedObject, pParent, pElement )
    {
    }
    void Object::load( Loader& loader )
    {
        Context::load( loader );
    }
    void Object::store( Storer& storer ) const
    {
        Context::store( storer );
    }
    
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    Link::Link( const IndexedObject& indexedObject )
        :   Context( indexedObject, nullptr, nullptr )
    {
    }
    Link::Link( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Context( indexedObject, pParent, pElement )
    {
    }
    void Link::load( Loader& loader )
    {
        Context::load( loader );
    }
    void Link::store( Storer& storer ) const
    {
        Context::store( storer );
    }
    
    /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////
    Root::Root( const IndexedObject& indexedObject )
        :   Object( indexedObject ),
			m_rootType( eInterfaceRoot )
    {
    }
    Root::Root( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Object( indexedObject, pParent, pElement ),
            m_pRoot( dynamic_cast< input::Root* >( pElement ) ),
			m_rootType( m_pRoot->getRootType() )
    {
        VERIFY_RTE( m_pRoot );
    }
        
    void Root::load( Loader& loader )
    {
        Object::load( loader );
        if( m_pElement )
        {
            m_pRoot = dynamic_cast< input::Root* >( m_pElement );
            VERIFY_RTE( m_pRoot );
        }
        loader.load( m_rootType );
    }
    void Root::store( Storer& storer ) const
    {
        Object::store( storer );
        storer.store( m_rootType );
    }
    /*bool Root::update( const Element* pElement )
    {
        if( const Root* pNewRoot = dynamic_cast< const Root* >( pElement ) )
        {
            //require the opaque is equal
            if( ( m_pRoot == nullptr ) || ( pNewRoot->m_pRoot == nullptr ) )
            {
                if( ( m_pRoot == nullptr ) && ( pNewRoot->m_pRoot == nullptr ) )
                {
                    return Element::update( pElement );
                }
            }
            else if( m_pRoot->equal( *pNewRoot->m_pRoot ) )
            {
                return Element::update( pElement );
            }
        }
        return true;
    }*/
    
	bool Root::isExecutable() const
	{
		switch( m_rootType )
		{
			case eInterfaceRoot:
				break;
			case eFile         :
			case eFileRoot     :
				return true;
			case eMegaRoot     :
			case eCoordinator  :
			case eHostName     :
			case eProjectName  :
			case eSubFolder    :
				break;
		}
		return false;
	}
		
	bool Root::isMainExecutable() const
	{
		switch( m_rootType )
		{
			case eInterfaceRoot:
				break;
			case eFile         :
				break;
			case eFileRoot     :
				return true;
			case eMegaRoot     :
			case eCoordinator  :
			case eHostName     :
			case eProjectName  :
			case eSubFolder    :
				break;
		}
		return false;
	}
} //namespace interface
} //namespace eg
