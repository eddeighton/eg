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


#ifndef CONCRETE_TREE_19_04_2019
#define CONCRETE_TREE_19_04_2019

#include "interface.hpp"

#include "eg/common.hpp"

namespace eg
{
    class InterfaceSession;
    class ObjectFactoryImpl;
	class LinkGroup;
    
namespace concrete
{
    class Element;
    class Action;
    class Dimension;
    class RangeAllocator;
    class Inheritance_Node : public IndexedObject
    {
        friend class ::eg::InterfaceSession;
        friend class ::eg::ObjectFactoryImpl;
    public:
        static const ObjectType Type = eInheritanceNode;
    protected:
        Inheritance_Node( const IndexedObject& indexedObject )
            :   IndexedObject( indexedObject )
        {
        }
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
    public:
        using Inheritance_Node_Vector = std::vector< Inheritance_Node* >;
        using Inheritance_Node_Set = std::set< Inheritance_Node*, CompareIndexedObjects >;
        using Inheritance_Node_SetCst = std::set< const Inheritance_Node*, CompareIndexedObjects >;
            
        //getStaticDerived and getDynamicDerived used by compatibility routines in code gen
        inline void getStaticDerived( std::set< const interface::Context*, CompareIndexedObjects >& derived ) const
        {
            derived.insert( m_pContext );
            for( const Inheritance_Node* p : m_children )
            {
                p->getStaticDerived( derived );
            }
        }
        inline void getDynamicDerived( std::set< const concrete::Action*, CompareIndexedObjects >& derived ) const
        {
            derived.insert( m_pRootConcreteAction );
            for( const Inheritance_Node* p : m_children )
            {
                p->getDynamicDerived( derived );
            }
        }
        
        Action* getRootConcreteAction() const { return m_pRootConcreteAction; }
        const ::eg::interface::Context* getAbstractAction() const { return m_pContext; }
        Inheritance_Node* getParent() const { return m_pParent; }
        const Inheritance_Node_Vector& getChildren() const { return m_children; }
        const std::vector< Action* > getActions() const { return m_actions; }
        const std::vector< Dimension* >& getDimensions() const { return m_dimensions; }
        
    private:
        Action* m_pRootConcreteAction; //the actual concrete action this inheritance node tree is entirely for
        const ::eg::interface::Context* m_pContext; //the abstract action this node is for
        Inheritance_Node* m_pParent = nullptr;
        Inheritance_Node_Vector m_children; //the nested nodes that describe how this node inherits other abstract actions
        std::vector< Action* > m_actions;
        std::vector< Dimension* > m_dimensions;
    };
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class Element : public IndexedObject
    {
        friend class ::eg::InterfaceSession;
        friend class ::eg::ObjectFactoryImpl;
    protected:
        Element( const IndexedObject& indexedObject )
            :   IndexedObject( indexedObject )
        {
        }
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
    public:
        Element* getParent() const { return m_pParent; }
        const ::eg::interface::Element* getAbstractElement() const { return m_pElement; }
        const std::vector< Element* >& getChildren() const { return m_children; }
    
        virtual void print( std::ostream& os, std::string& strIndent ) const = 0;
        
        inline bool isMember( const Element* pElement ) const
        {
            return std::find( m_children.begin(), m_children.end(), pElement ) != m_children.end();
        }
    protected:
        Element* m_pParent = nullptr;
        const ::eg::interface::Element* m_pElement = nullptr;
        std::vector< Element* > m_children;
    };
    
    std::vector< Element* > getPath( Element* pNode, Element* pFrom = nullptr );
    std::vector< const Element* > getPath( const Element* pNode, const Element* pFrom = nullptr );
        
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class Dimension : public Element
    {
        friend class ::eg::InterfaceSession;
        friend class ::eg::ObjectFactoryImpl;
    protected:
        Dimension( const IndexedObject& indexedObject )
            :   Element( indexedObject )
        {
        }
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    public:
        virtual bool isSimple() const = 0;
    };
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class Dimension_Generated;
    
    class Dimension_User : public Dimension
    {
        friend class ::eg::InterfaceSession;
        friend class ::eg::ObjectFactoryImpl;
    public:
        static const ObjectType Type = eConcreteDimensionUser;
    protected:
        Dimension_User( const IndexedObject& indexedObject )
            :   Dimension( indexedObject )
        {
        }
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
    public:
        virtual void print( std::ostream& os, std::string& strIndent ) const;
        
    public:
        bool isEGType() const
        {
            return !getContextTypes().empty();
        }
        
        virtual bool isSimple() const { return getDimension()->isSimple(); }
        
        const std::vector< ::eg::interface::Context* >& getContextTypes() const
        {
            return getDimension()->getContextTypes();
        }
        
        const ::eg::interface::Dimension* getDimension() const { return dynamic_cast< const ::eg::interface::Dimension* >( m_pElement ); }
        
		const LinkGroup* getLinkGroup() const { return m_pLinkGroup; }
		
	private:
		LinkGroup* m_pLinkGroup = nullptr;
    };
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class Action;
    class Dimension_Generated : public Dimension
    {
        friend class ::eg::InterfaceSession;
        friend class ::eg::ObjectFactoryImpl;
        friend class ::eg::concrete::RangeAllocator;
        friend void constructRuntimeDimensions( InterfaceSession&, Action* );
    public:
        static const ObjectType Type = eConcreteDimensionGenerated;
        
        enum DimensionType
        {
            eActionStopCycle,
            eActionReference,
            eActionState,
            eActionAllocator,
			eLinkReference,
			eLinkReferenceCount
        };
        
    protected:
        Dimension_Generated( const IndexedObject& indexedObject )
            :   Dimension( indexedObject )
        {
        }
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
        virtual void print( std::ostream& os, std::string& strIndent ) const;
        
    public:
        DimensionType getDimensionType() const { return m_type; }
        Action* getAction() const { return m_pContext; }
		LinkGroup* getLinkGroup() const { return m_pLinkGroup; }
        virtual bool isSimple() const 
        { 
            switch( m_type )
            {
                case eActionState:          return false;
                default:                    return true; 
            }
        }
    private:
        DimensionType m_type;
        Action* m_pContext = nullptr;
		LinkGroup* m_pLinkGroup = nullptr;
    };
    
    
    
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class Allocator;
    
    class Action : public Element
    {
        friend class ::eg::InterfaceSession;
        friend class Inheritance_Node;
        friend class ::eg::ObjectFactoryImpl;
        friend class ::eg::concrete::RangeAllocator;
        friend void constructRuntimeDimensions( InterfaceSession&, Action* );
    public:
        static const ObjectType Type = eConcreteAction;
    protected:
        Action( const IndexedObject& indexedObject )
            :   Element( indexedObject )
        {
        }
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
    public:
        using AllocatorMap = std::map< const Action*, const Allocator*, CompareIndexedObjects >;
		using LinkMap = std::map< std::string, const Dimension_Generated* >;
        
        const Action* getObject() const { return m_pObject; }
        const ::eg::interface::Context* getContext() const { return dynamic_cast< const ::eg::interface::Context* >( m_pElement ); }
        const Inheritance_Node* getInheritance() const { return m_inheritance; }
        const std::string& getName() const { return m_strName; }
        
        const Dimension_Generated* getStopCycle    () const { return m_pStopCycle     ; } //timestamp when stopped
        const Dimension_Generated* getState        () const { return m_pState         ; }
        const Dimension_Generated* getReference    () const { return m_pReference     ; }
        const Dimension_Generated* getLinkRefCount () const { return m_pLinkRefCount  ; }
    
        const Allocator* getAllocator() const { return m_pAllocator; }
        const Allocator* getAllocator( const Action* pAction ) const 
        {
            AllocatorMap::const_iterator iFind = m_allocators.find( pAction );
            VERIFY_RTE( iFind != m_allocators.end() );
            return iFind->second;
        }
        const AllocatorMap& getAllocators() const { return m_allocators; }
        
		const Dimension_Generated* getLink( const std::string& strLinkName ) const
		{
            LinkMap::const_iterator iFind = m_links.find( strLinkName );
            VERIFY_RTE( iFind != m_links.end() );
            return iFind->second;
		}
        const LinkMap& getLinks() const { return m_links; }
        const Dimension_User* getLinkBaseDimension() const;
        bool hasUserDimensions() const;
    
        virtual int getLocalDomainSize() const;
        virtual int getTotalDomainSize() const;
        int getObjectDomainFactor() const;
        
        std::string getFriendlyName() const { return getContext()->getFriendlyName(); }
        void print( std::ostream& os, std::string& strIndent ) const;
        
    private:
        Action* m_pObject = nullptr;
        Inheritance_Node* m_inheritance;
        Allocator* m_pAllocator = nullptr;
        std::string m_strName;
        mutable int m_totalDomainSize = 0;
        
        Dimension_Generated* m_pStopCycle     = nullptr;
        Dimension_Generated* m_pState         = nullptr;
        Dimension_Generated* m_pReference     = nullptr;
        Dimension_Generated* m_pLinkRefCount  = nullptr;
        AllocatorMap m_allocators;
		LinkMap m_links;
    };

} //namespace concrete
} //namespace eg

#endif //CONCRETE_TREE_19_04_2019