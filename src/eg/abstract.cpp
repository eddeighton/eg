
#include "abstract.hpp"

namespace eg
{
namespace abstract
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
    
    void Element::print( std::ostream& os, std::string& strIndent ) const
    {
        if( m_pElement )
        {
            switch( m_pElement->getType() )
            {
                case eInputOpaque    :
                case eInputDimension :
                case eInputInclude   :
                    m_pElement->print( os, strIndent );
                    break;
                case eInputRoot      :
                case eInputAction    :
                    {
                        input::Action* pAction = dynamic_cast< input::Action* >( m_pElement );
                        VERIFY_RTE( pAction );
                        
                        pAction->printDeclaration( os, strIndent );
                        
                        os << "\n" << strIndent << "{\n";

                        strIndent.push_back( ' ' );
                        strIndent.push_back( ' ' );

                        for( const Element* pChildNode : m_children )
                        {
                            pChildNode->print( os, strIndent );
                        }

                        strIndent.pop_back();
                        strIndent.pop_back();

                        os << "\n" << strIndent << "}\n";
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
                pChildNode->print( os, strIndent );
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
                case eInputRoot      :  return dynamic_cast< input::Root* >(       m_pElement )->getIdentifier();
                case eInputAction    :  return dynamic_cast< input::Action* >(     m_pElement )->getIdentifier();
                    break;
                case eInputOpaque    :
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
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
        loader.loadObjectVector( m_actionTypes );
        loader.load( m_canonicalType );
        loader.load( m_size );
    }
    void Dimension::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.storeObjectVector( m_actionTypes );
        storer.store( m_canonicalType );
        storer.store( m_size );
    }
    const std::string& Dimension::getType() const
    {
        return m_pDimension->getType()->getStr();
    }
    bool Dimension::isHomogenous( const std::vector< const Dimension* >& dimensions )
    {
        if( dimensions.size() > 1 )
        {
            const Dimension* pFirst = dimensions.front();
            std::set< const Action* > cmp( 
                pFirst->m_actionTypes.begin(), 
                pFirst->m_actionTypes.end() );
            for( std::vector< const Dimension* >::const_iterator 
                i       = dimensions.begin() + 1,
                iEnd    = dimensions.end(); i!=iEnd; ++i )
            {
                if( pFirst->m_canonicalType != (*i)->m_canonicalType )
                    return false;
                else 
                {
                    std::set< const Action* > cmp2( 
                        (*i)->m_actionTypes.begin(), 
                        (*i)->m_actionTypes.end() );
                    if( cmp != cmp2 )
                        return false;
                }
            }
        }
        return true;
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
    
    
    Action::Action( const IndexedObject& indexedObject )
        :   Element( indexedObject, nullptr, nullptr )
    {
    }
    Action::Action( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Element( indexedObject, pParent, pElement ),
            m_pAction( dynamic_cast< input::Action* >( pElement ) )
    {
        VERIFY_RTE( m_pAction );
    }

    void Action::load( Loader& loader )
    {
        Element::load( loader );
        if( m_pElement )
        {
            m_pAction = dynamic_cast< input::Action* >( m_pElement );
            VERIFY_RTE( m_pAction );
        }
        loader.load( m_size );
        loader.loadObjectVector( m_baseActions );
        loader.load( m_strBaseType );
        loader.load( m_strDependency );
        loader.loadObjectVector( m_inheriters );
        loader.loadObjectVector( m_linkers );
    }
    
    void Action::store( Storer& storer ) const
    {
        Element::store( storer );
        storer.store( m_size );
        storer.storeObjectVector( m_baseActions );
        storer.store( m_strBaseType );
        storer.store( m_strDependency );
        storer.storeObjectVector( m_inheriters );
        storer.storeObjectVector( m_linkers );
    }
    
    std::string Action::getFriendlyName() const
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
    
    void Action::getDimensions( std::vector< Dimension* >& dimensions ) const
    {
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eAbstractDimension :  dimensions.push_back( dynamic_cast< Dimension* >( pElement ) ); break;
                case eAbstractInclude   :  break;
                case eAbstractRoot      :  break;
                case eAbstractAction    :  break;
                case eAbstractOpaque    :  break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
    }
    
    std::size_t Action::getBaseCount() const
    {
        if( m_pAction )
        {
            return m_pAction->getInheritance().size();
        }
        else
        {
            return 0U;
        }
    }
        
    void Action::getChildActions( std::vector< Action* >& actions ) const
    {
        for( Element* pElement : m_children )
        {
            switch( pElement->getType() )
            {
                case eAbstractDimension :  break;
                case eAbstractInclude   :  break;
                case eAbstractRoot      :  actions.push_back( dynamic_cast< Action* >( pElement ) ); break;
                case eAbstractAction    :  actions.push_back( dynamic_cast< Action* >( pElement ) ); break;
                case eAbstractOpaque    :  break;
                default:
                    THROW_RTE( "Unsupported type" );
                    break;
            }
        }
    }
    
    bool Action::isAbstract() const
    {
        return m_pAction ? m_pAction->isAbstract() : false;
    }
    
    bool Action::isLink() const
    {
        return m_pAction ? m_pAction->isLink() : false;
    }
    
    bool Action::isIndirectlyAbstract() const
    {
        if( !m_bIndirectlyAbstract )
        {
            if( isAbstract() )
            {
                m_bIndirectlyAbstract = true;
            }
            else if( const Action* pParent = 
                dynamic_cast< const Action* >( m_pParent ) )
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
    
    bool Action::isSingular() const
    {
        return ( m_size == 1U ) ? true : false;
    }
    
    
    Root::Root( const IndexedObject& indexedObject )
        :   Action( indexedObject )
    {
    }
    Root::Root( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement )
        :   Action( indexedObject, pParent, pElement ),
            m_pRoot( dynamic_cast< input::Root* >( pElement ) )
    {
        VERIFY_RTE( m_pRoot );
    }
        
    void Root::load( Loader& loader )
    {
        Action::load( loader );
        if( m_pElement )
        {
            m_pRoot = dynamic_cast< input::Root* >( m_pElement );
            VERIFY_RTE( m_pRoot );
        }
    }
    void Root::store( Storer& storer ) const
    {
        Action::store( storer );
    }

    std::optional< boost::filesystem::path > Root::getPath() const
    {
        std::optional< boost::filesystem::path > result;
        if( m_pRoot )
        {
            result = m_pRoot->getPath();
        }
        return result;
    }
    bool Root::isMainFile() const
    {
        return m_pRoot ? m_pRoot->isMainFile() : false;
    }

    
} //namespace abstract
} //namespace eg
