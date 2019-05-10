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


#ifndef ABSTRACT_TREE_18_04_2019
#define ABSTRACT_TREE_18_04_2019

#include "input.hpp"

#include <set>
#include <vector>
#include <string>
#include <optional>

namespace clang
{
    class AbstractMutator;
    class ObjectFactoryImpl;
}

namespace eg
{
    class ParserSession;
    class InterfaceSession;

namespace abstract
{

    class Element : public IndexedObject
    {
        friend class ::eg::ObjectFactoryImpl;
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
            collectIfBase( const ArgType* pArg, std::vector< const Base* >& result )
            {
                result.push_back( pArg );
            }
        
            template< typename Base, typename ArgType >
            std::enable_if_t<!std::is_base_of< Base, ArgType >::value>
            collectIfBase( const ArgType* pArg, std::vector< const Base* >& result )
            {
            }
        public:
        
            template< class TArg >
            void operator() ( const TArg* pElement )
            {
                collectIfBase< T, TArg >( pElement, m_result );
            }
        
            const std::vector< const T* >& operator()() { return m_result; }
        private:
            std::vector< const T* > m_result;
        };

        template< class T >
        inline void visit( T& visitor ) const
        {
            if( m_pElement )
            {
                switch( m_pElement->getType() )
                {
                    case eInputOpaque    : visitor( dynamic_cast< const input::Opaque* >(     m_pElement ) ); break;
                    case eInputDimension : visitor( dynamic_cast< const input::Dimension* >(  m_pElement ) ); break;
                    case eInputInclude   : visitor( dynamic_cast< const input::Include* >(    m_pElement ) ); break;
                    case eInputRoot      : visitor( dynamic_cast< const input::Root* >(       m_pElement ) ); break;
                    case eInputAction    : visitor( dynamic_cast< const input::Action* >(     m_pElement ) ); break;
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
        inline void pushpop( T& visitor ) const
        {
            if( m_pElement )
            {
                switch( m_pElement->getType() )
                {
                    case eInputOpaque    : visitor.push( dynamic_cast< const input::Opaque* >(     m_pElement ), this ); break;
                    case eInputDimension : visitor.push( dynamic_cast< const input::Dimension* >(  m_pElement ), this ); break;
                    case eInputInclude   : visitor.push( dynamic_cast< const input::Include* >(    m_pElement ), this ); break;
                    case eInputRoot      : visitor.push( dynamic_cast< const input::Root* >(       m_pElement ), this ); break;
                    case eInputAction    : visitor.push( dynamic_cast< const input::Action* >(     m_pElement ), this ); break;
                        break;
                    default:
                        THROW_RTE( "Unsupported type" );
                        break;
                }
            }
            for( const Element* pChildNode : m_children )
            {
                pChildNode->pushpop( visitor );
            }
            if( m_pElement )
            {
                switch( m_pElement->getType() )
                {
                    case eInputOpaque    : visitor.pop( dynamic_cast< const input::Opaque* >(     m_pElement ), this ); break;
                    case eInputDimension : visitor.pop( dynamic_cast< const input::Dimension* >(  m_pElement ), this ); break;
                    case eInputInclude   : visitor.pop( dynamic_cast< const input::Include* >(    m_pElement ), this ); break;
                    case eInputRoot      : visitor.pop( dynamic_cast< const input::Root* >(       m_pElement ), this ); break;
                    case eInputAction    : visitor.pop( dynamic_cast< const input::Action* >(     m_pElement ), this ); break;
                        break;
                    default:
                        THROW_RTE( "Unsupported type" );
                        break;
                }
            }
        }

        input::Element* getInputElement() const { return m_pElement; }
        Element* getParent() const { return m_pParent; }
        
        void print( std::ostream& os, std::string& strIndent ) const;
        const std::string& getIdentifier() const;
        const std::vector< Element* >& getChildren() const { return m_children; }
        std::string getFriendlyName() const;
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
        friend class ::eg::ObjectFactoryImpl;
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
        friend class ::eg::ObjectFactoryImpl;
        friend class ::clang::AbstractMutator;
    public:
        static const ObjectType Type = eAbstractDimension;
    protected:
        Dimension( const IndexedObject& indexedObject );
        Dimension( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    public:
        const std::string& getType() const;
        const std::string& getCanonicalType() const { return m_canonicalType; }
        std::size_t getSize() const { return m_size; }
        const std::vector< Action* >& getActionTypes() const { return m_actionTypes; }
        static bool isHomogenous( const std::vector< const Dimension* >& dimensions );
        
    private:
        input::Dimension* m_pDimension = nullptr;
        std::vector< Action* > m_actionTypes;
        std::string m_canonicalType;
        std::size_t m_size;
    };
    
    class Include : public Element
    {
        friend class ::eg::ObjectFactoryImpl;
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
        friend class ::eg::InterfaceSession;
        friend class ::clang::AbstractMutator;
    public:
        static const ObjectType Type = eAbstractAction;
    protected:
        Action( const IndexedObject& indexedObject );
        Action( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;

    public:
        void getDimensions( std::vector< Dimension* >& dimensions ) const;
        std::size_t getBaseCount() const;
        const std::vector< Action* >& getBaseActions() const { return m_baseActions; }
        const std::string& getBaseType() const { return m_strBaseType; }
        void getChildActions( std::vector< Action* >& actions ) const;
        bool isAbstract() const;
        bool isLink() const;
        bool isIndirectlyAbstract() const;
        bool isSingular() const;
        std::size_t getSize() const { return m_size; }
        
    protected:
        input::Action* m_pAction = nullptr;
        std::size_t m_size = 1U;
        mutable std::optional< bool > m_bIndirectlyAbstract;
        std::vector< Action* > m_baseActions;
        std::string m_strBaseType;
        std::string m_strDependency;
        
        std::vector< Action* > m_inheriters;
        std::vector< Action* > m_linkers;
    };
        
    class Root : public Action
    {
        friend class ::eg::ObjectFactoryImpl;
    public:
        static const ObjectType Type = eAbstractRoot;
    protected:
        Root( const IndexedObject& indexedObject );
        Root( const IndexedObject& indexedObject, Element* pParent, input::Element* pElement );
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
        mutable std::string m_strTemp;
    public:
        
        std::optional< boost::filesystem::path > getPath() const;
        bool isMainFile() const;

        
    private:
        input::Root* m_pRoot = nullptr;
    };

} //namespace abstract
} //namespace eg

#endif //ABSTRACT_TREE_18_04_2019