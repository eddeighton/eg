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


#ifndef ALLOCATOR_23_JUNE_2020
#define ALLOCATOR_23_JUNE_2020


#include "eg_compiler/objects.hpp"

namespace eg
{
    class InterfaceSession;
    
    namespace concrete
    {
        class Action;
        class Dimension_Generated;

        class Allocator : public IndexedObject
        {
            friend class ::eg::InterfaceSession;
            friend class ::eg::ObjectFactoryImpl;
        protected:
            Allocator( const IndexedObject& indexedObject )
                :   IndexedObject( indexedObject )
            {
            }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            
            virtual void constructDimensions( InterfaceSession& session ) = 0;
            
        public:
            const Action* getAllocated() const { return m_pContext_Allocated; }
            const Action* getAllocating() const { return m_pContext_Allocating; }
            
        protected:
            Action* m_pContext_Allocated = nullptr;
            Action* m_pContext_Allocating = nullptr;
        };
    
        class NothingAllocator : public Allocator
        {
            friend class ::eg::InterfaceSession;
            friend class ::eg::ObjectFactoryImpl;
            friend Allocator* chooseAllocator( InterfaceSession&, Action*, Action* );
        public:
            static const ObjectType Type = eConcreteAllocator_Nothing;
        protected:
            NothingAllocator( const IndexedObject& indexedObject )
                :   Allocator( indexedObject )
            {
            }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            
            virtual void constructDimensions( InterfaceSession& session );
        };
        
        class SingletonAllocator : public Allocator
        {
            friend class ::eg::InterfaceSession;
            friend class ::eg::ObjectFactoryImpl;
            friend Allocator* chooseAllocator( InterfaceSession&, Action*, Action* );
        public:
            static const ObjectType Type = eConcreteAllocator_Singleton;
        protected:
            SingletonAllocator( const IndexedObject& indexedObject )
                :   Allocator( indexedObject )
            {
            }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            
            virtual void constructDimensions( InterfaceSession& session );
        };
        
        class RangeAllocator : public Allocator
        {
            friend class ::eg::InterfaceSession;
            friend class ::eg::ObjectFactoryImpl;
            friend Allocator* chooseAllocator( InterfaceSession&, Action*, Action* );
        public:
            static const ObjectType Type = eConcreteAllocator_Range;
        protected:
            RangeAllocator( const IndexedObject& indexedObject )
                :   Allocator( indexedObject )
            {
            }
            
            virtual void load( Loader& loader );
            virtual void store( Storer& storer ) const;
            
        public:
            const Dimension_Generated* getAllocatorData() const { return m_pAllocatorData ; }
            std::string getAllocatorType() const;
        
        protected:
            virtual void constructDimensions( InterfaceSession& session );
            
            Dimension_Generated* m_pAllocatorData = nullptr;
        };
        
        Allocator* chooseAllocator( InterfaceSession& session, Action* pParent, Action* pChild );
        
    
    } //namespace concrete
} //namespace eg

#endif //ALLOCATOR_23_JUNE_2020