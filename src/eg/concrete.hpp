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

#include "abstract.hpp"

#include "runtime/eg_common.hpp"

namespace eg
{
    class InterfaceSession;
    class ObjectFactoryImpl;
    
namespace concrete
{
    class Element;
    class Action;
    class Dimension;
    
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
        
        const Inheritance_Node* findInstance( const Element* pInstance, 
            const Dimension*& pDimensionResult, Inheritance_Node_SetCst& visited) const;
            
        inline void getStaticDerived( std::set< const abstract::Action*, CompareIndexedObjects >& derived ) const
        {
            derived.insert( m_pAction );
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
        const ::eg::abstract::Action* getAbstractAction() const { return m_pAction; }
        Inheritance_Node* getParent() const { return m_pParent; }
        const Inheritance_Node_Vector& getChildren() const { return m_children; }
        const std::vector< Action* > getActions() const { return m_actions; }
        const std::vector< Dimension* >& getDimensions() const { return m_dimensions; }
        
    private:
        Action* m_pRootConcreteAction;
        const ::eg::abstract::Action* m_pAction;
        Inheritance_Node* m_pParent = nullptr;
        Inheritance_Node_Vector m_children;
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
        const ::eg::abstract::Element* getAbstractElement() const { return m_pElement; }
        const std::vector< Element* >& getChildren() const { return m_children; }
    
        virtual const std::string& getIdentifier() const = 0;
        virtual void print( std::ostream& os, std::string& strIndent ) const = 0;
        virtual int getLocalDomainSize() const = 0;
        virtual int getTotalDomainSize() const = 0;
        virtual int getDataSize() const = 0;
        
    protected:
        Element* m_pParent = nullptr;
        const ::eg::abstract::Element* m_pElement = nullptr;
        std::vector< Element* > m_children;
    };
    
    std::vector< Element* > getPath( Element* pNode, Element* pFrom = nullptr );
    std::vector< const Element* > getPath( const Element* pNode, const Element* pFrom = nullptr );
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    struct IPrintDimensions
    {
        virtual void printVariableAccess( std::ostream& os, const std::string& strIndex ) const = 0;
        virtual void printDependencyVar( std::ostream& os, const std::string& strIndex ) const = 0;
    };
    
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
    
        virtual int getLocalDomainSize() const { return 1; }
        virtual int getTotalDomainSize() const;
        virtual void printType( std::ostream& os ) const = 0;
        
        virtual void printAllocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const = 0;
        virtual void printDeallocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const = 0;
        virtual void printStart( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const = 0;
        virtual void printStop( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const = 0;
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
        const std::string& getIdentifier() const { return getDimension()->getIdentifier(); }
        virtual void print( std::ostream& os, std::string& strIndent ) const;
        virtual void printType( std::ostream& os ) const;
        
    protected:
        virtual int getDataSize() const;
        
        virtual void printAllocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;
        virtual void printDeallocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;
        virtual void printStart( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;
        virtual void printStop( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;
    public:
        bool isEGType() const
        {
            return !getActionTypes().empty();
        }
        
        const std::vector< ::eg::abstract::Action* >& getActionTypes() const
        {
            return getDimension()->getActionTypes();
        }
        
        const ::eg::abstract::Dimension* getDimension() const { return dynamic_cast< const ::eg::abstract::Dimension* >( m_pElement ); }
        
    private:
        Dimension_Generated* m_pTimestamp = nullptr;
    };
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class Action;
    class Dimension_Generated : public Dimension
    {
        friend class ::eg::InterfaceSession;
        friend class ::eg::ObjectFactoryImpl;
    public:
        static const ObjectType Type = eConcreteDimensionGenerated;
        
        enum DimensionType
        {
            eDimensionTimestamp,
            eActionRunning,
            eActionPaused,
            eActionCoroutine,
            eActionEventIter,
            eActionObject,
            eActionReference,
            eActionAllocatorData,
            eActionAllocatorHead,
            eActionStopTimestamp
        };
        
    protected:
        Dimension_Generated( const IndexedObject& indexedObject )
            :   Dimension( indexedObject )
        {
        }
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
        const std::string& getIdentifier() const;
        virtual void print( std::ostream& os, std::string& strIndent ) const;
        virtual void printType( std::ostream& os ) const;
        
        virtual int getDataSize() const;
        
        virtual void printAllocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;
        virtual void printDeallocation( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;
        virtual void printStart( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;
        virtual void printStop( std::ostream& os, const IPrintDimensions& printer, const std::string& strIndex ) const;

    public:
        DimensionType getDimensionType() const { return m_type; }
        const Dimension_User* getUserDimension() const { return m_pUserDimension; }
        Action* getAction() const { return m_pAction; }
        std::size_t getDependencyDomain() const { return dependencyDomain; }
        Dimension_Generated* getDependency() const { return m_pDependency; }
    private:
        DimensionType m_type;
        Dimension_User* m_pUserDimension = nullptr;
        Action* m_pAction = nullptr;
        Dimension_Generated* m_pDependency = nullptr;
        std::size_t dependencyDomain = 1U;
    };
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class Action : public Element
    {
        friend class ::eg::InterfaceSession;
        friend class Inheritance_Node;
        friend class ::eg::ObjectFactoryImpl;
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
        using IteratorMap = std::map< const Action*, const Dimension_Generated* >;
        
        const ::eg::abstract::Action* getAction() const { return dynamic_cast< const ::eg::abstract::Action* >( m_pElement ); }
        const Inheritance_Node* getInheritance() const { return m_inheritance; }
        const std::string& getName() const { return m_strName; }
        
        const Dimension_Generated* getRunningTimestamp () const { return m_pRunningTimestamp ; }
        const Dimension_Generated* getPauseTimestamp   () const { return m_pPauseTimestamp   ; }
        const Dimension_Generated* getCoroutine        () const { return m_pCoroutine        ; }
        const Dimension_Generated* getEventIterator    () const { return m_pEventIterator    ; }
        const Dimension_Generated* getMappedObject     () const { return m_pMappedObject     ; }
        const Dimension_Generated* getReference        () const { return m_pReference        ; }
        const Dimension_Generated* getAllocatorData    () const { return m_pAllocatorData    ; }
        const Dimension_Generated* getStopTimestamp    () const { return m_pStopTimestamp    ; }
    
        const Dimension_Generated* getIterator( const Action* pAction ) const 
        {
            IteratorMap::const_iterator iFind = m_allocators.find( pAction );
            VERIFY_RTE( iFind != m_allocators.end() );
            return iFind->second;
        }
    
        virtual int getDataSize() const;
        virtual int getLocalDomainSize() const;
        virtual int getTotalDomainSize() const;
        
        const std::string& getIdentifier() const { return getAction()->getIdentifier(); }
        void print( std::ostream& os, std::string& strIndent ) const;
        virtual void printType( std::ostream& os ) const;
        
    private:
        Inheritance_Node* m_inheritance;
        std::string m_strName;
        mutable int m_totalDomainSize = 0;
        
        Dimension_Generated* m_pRunningTimestamp   = nullptr;
        Dimension_Generated* m_pPauseTimestamp     = nullptr;
        Dimension_Generated* m_pCoroutine          = nullptr;
        Dimension_Generated* m_pEventIterator      = nullptr;
        Dimension_Generated* m_pMappedObject       = nullptr;
        Dimension_Generated* m_pReference          = nullptr;
        Dimension_Generated* m_pAllocatorData      = nullptr;
        Dimension_Generated* m_pStopTimestamp      = nullptr;
        
        IteratorMap m_allocators;
        Action* m_pDependencyProvider = nullptr;
    };


} //namespace concrete
} //namespace eg

#endif //CONCRETE_TREE_19_04_2019