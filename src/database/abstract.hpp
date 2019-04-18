
#ifndef ABSTRACT_TREE_18_04_2019
#define ABSTRACT_TREE_18_04_2019

#include "input.hpp"

#include <set>
#include <vector>
#include <string>
#include <optional>

namespace eg
{
    class ParserSession;

namespace abstract
{

    class Element : public IndexedObject
    {
        friend class ObjectFactoryImpl;
        friend class ::eg::ParserSession;
    protected:
        Element( const IndexedObject& object, Element* pParent, input::Element* pElement );
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;

        template< class T >
        class Collector
        {
            template< typename Base, typename ArgType >
            std::enable_if_t<std::is_base_of< Base, ArgType >::value>
            collectIfBase( ArgType* pArg, std::vector< Base* >& result )
            {
                result.push_back( pArg );
            }

            template< typename Base, typename ArgType >
            std::enable_if_t<!std::is_base_of< Base, ArgType >::value>
            collectIfBase( ArgType* pArg, std::vector< Base* >& result )
            {
            }
        public:

            template< class TArg >
            void operator() ( TArg* pElement )
            {
                collectIfBase< T, TArg >( pElement, m_result );
            }

            const std::vector< T* >& operator()() { return m_result; }
        private:
            std::vector< T* > m_result;
        };

        template< class T >
        inline void visit( T& visitor )
        {
            if( m_pElement )
            {
                switch( m_pElement->getType() )
                {
                    case eInputOpaque    : visitor( dynamic_cast< input::Opaque* >(     m_pElement ) ); break;
                    case eInputDimension : visitor( dynamic_cast< input::Dimension* >(  m_pElement ) ); break;
                    case eInputInclude   : visitor( dynamic_cast< input::Include* >(    m_pElement ) ); break;
                    case eInputRoot      : visitor( dynamic_cast< input::Root* >(       m_pElement ) ); break;
                    case eInputAction    : visitor( dynamic_cast< input::Action* >(     m_pElement ) ); break;
                        break;
                    default:
                        THROW_RTE( "Unsupported type" );
                        break;
                }
            }
            for( Element* pChildNode : m_children )
            {
                pChildNode->visit( visitor );
            }
        }

        template< class T >
        inline void pushpop( T& visitor )
        {
            if( m_pElement )
            {
                switch( m_pElement->getType() )
                {
                    case eInputOpaque    : visitor.push( dynamic_cast< input::Opaque* >(     m_pElement ), this ); break;
                    case eInputDimension : visitor.push( dynamic_cast< input::Dimension* >(  m_pElement ), this ); break;
                    case eInputInclude   : visitor.push( dynamic_cast< input::Include* >(    m_pElement ), this ); break;
                    case eInputRoot      : visitor.push( dynamic_cast< input::Root* >(       m_pElement ), this ); break;
                    case eInputAction    : visitor.push( dynamic_cast< input::Action* >(     m_pElement ), this ); break;
                        break;
                    default:
                        THROW_RTE( "Unsupported type" );
                        break;
                }
            }
            for( Element* pChildNode : m_children )
            {
                pChildNode->pushpop( visitor );
            }
            if( m_pElement )
            {
                switch( m_pElement->getType() )
                {
                    case eInputOpaque    : visitor.pop( dynamic_cast< input::Opaque* >(     m_pElement ), this ); break;
                    case eInputDimension : visitor.pop( dynamic_cast< input::Dimension* >(  m_pElement ), this ); break;
                    case eInputInclude   : visitor.pop( dynamic_cast< input::Include* >(    m_pElement ), this ); break;
                    case eInputRoot      : visitor.pop( dynamic_cast< input::Root* >(       m_pElement ), this ); break;
                    case eInputAction    : visitor.pop( dynamic_cast< input::Action* >(     m_pElement ), this ); break;
                        break;
                    default:
                        THROW_RTE( "Unsupported type" );
                        break;
                }
            }
        }

        //EGTypeID getElementIndex() const { return m_pElement ? m_pElement->getIndex() : getIndex(); }
        input::Element* getInputElement() const { return m_pElement; }
        Element* getParent() const { return m_pParent; }
        
        void print( std::ostream& os, std::string& strIndent ) const;
        const std::string& getIdentifier() const;
    protected:
        input::Element* m_pElement;
        Element* m_pParent;
        input::Include* pIncludeIdentifier;
        std::vector< Element* > m_children;
    };
    
    inline std::vector< Element* > getPath( Element* pNode, Element* pFrom = nullptr );
    inline std::vector< const Element* > getPath( const Element* pNode, const Element* pFrom = nullptr );

    
    class Opaque : public Element
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eAbstractOpaque;
    protected:
        Opaque( const IndexedObject& indexedObject );
        Opaque( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    public:
    
    private:
        input::Opaque* m_pOpaque = nullptr;
    };
    
    class Action;
    
    class Dimension : public Element
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eAbstractDimension;
    protected:
        Dimension( const IndexedObject& indexedObject );
        Dimension( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    public:
        const std::string& getType() const;
        static bool isHomogenous( const std::vector< const Dimension* >& dimensions );
        
    private:
        input::Dimension* m_pDimension = nullptr;
        std::vector< Action* > m_actionTypes;
        std::string m_canonicalType;
        std::size_t size;
    };
    
    class Include : public Element
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eAbstractInclude;
    protected:
        Include( const IndexedObject& indexedObject );
        Include( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    public:
    
    private:
        input::Include* m_pInclude = nullptr;
    };

    class Action : public Element
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eAbstractAction;
    protected:
        Action( const IndexedObject& indexedObject );
        Action( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;

    public:
        std::string getFriendlyName() const;
        void getDimensions( std::vector< Dimension* >& dimensions ) const;
        std::size_t getBaseCount() const;
        void getChildren( std::vector< Action* >& actions ) const;
        bool isAbstract() const;
        bool isLink() const;
        bool isIndirectlyAbstract() const;
        bool isSingular() const;
        
    protected:
        input::Action* m_pAction = nullptr;
        std::size_t size = 1U;
        mutable std::optional< bool > m_bIndirectlyAbstract;
        std::vector< Action* > m_baseActions;
        std::string m_strBaseType;
        std::string m_strDependency;
        
        std::vector< Action* > m_inheriters;
        std::vector< Action* > m_linkers;
    };
    
    class Root : public Action
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eAbstractRoot;
    protected:
        Root( const IndexedObject& indexedObject );
        Root( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
        mutable std::string m_strTemp;
    public:
        //static Root* get( const IndexedObject::Array& objects )
        //{
        //    for( IndexedObject* pObject : objects )
        //    {
        //        if( pObject->getType() == eNodeRoot )
        //        {
        //            Root* pNode = dynamic_cast< Root* >( pObject );
        //            if( pNode->m_pParent == nullptr )
        //                return pNode;
        //        }
        //    }
        //    THROW_RTE( "Failed to locate root node of tree" );
        //}
        
        std::optional< boost::filesystem::path > getPath() const;
        bool isMainFile() const;

        
    private:
        input::Root* m_pRoot = nullptr;
    };

} //namespace abstract
} //namespace eg

#endif //ABSTRACT_TREE_18_04_2019