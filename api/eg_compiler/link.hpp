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


#ifndef LINKANALYSIS_09_JUNE_2020
#define LINKANALYSIS_09_JUNE_2020

#include "eg.hpp"
#include "objects.hpp"
#include "concrete.hpp"
#include "derivation.hpp"
#include "sessions/session.hpp"

#include "eg/common.hpp"

#include "common/file.hpp"

#include <map>
#include <set>
#include <vector>
#include <tuple>

namespace eg
{
	class LinkGroup : public IndexedObject
	{
        friend class ObjectFactoryImpl;
		friend class LinkAnalysis;
        friend class InterfaceSession;
    public:
        static const ObjectType Type = eLinkGroup;
    protected:
        LinkGroup( const IndexedObject& object )
            :   IndexedObject( object )
        {
        }
		
    public:
		using Vector = std::vector< LinkGroup* >;
        using LinkRefMap = std::map< 
            const concrete::Action*, 
            const concrete::Dimension_Generated*, 
            CompareIndexedObjects >;
		
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
		
		const std::string& getLinkName() const { return m_name; }
		const std::vector< interface::Link* >& getLinks() const { return m_links; }
		const std::vector< interface::Context* >& getInterfaceTargets() const { return m_interfaceTargets; }
		const std::vector< concrete::Action* >& getTargets() const { return m_concreteTargets; }
        const std::vector< concrete::Action* >& getConcreteLinks() const { return m_concreteLinks; }
        const LinkRefMap& getDimensionMap() const { return m_dimensionMap; }
        
        static const interface::Context* getLinkTarget( const interface::Link* pLink );
        static interface::Context* getLinkTarget( interface::Link* pLink );
		
	private:
		std::string m_name;
		std::vector< interface::Link* > m_links;
		std::vector< interface::Context* > m_interfaceTargets;
		std::vector< concrete::Action* > m_concreteTargets;
		std::vector< concrete::Action* > m_concreteLinks;
        LinkRefMap m_dimensionMap;
	};
	
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class LinkAnalysis : public IndexedObject
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eLinkAnalysis;
    protected:
        LinkAnalysis( const IndexedObject& object )
            :   IndexedObject( object )
        {
        }
        
    public:
        template< typename T >
        struct CompareNodePath
        {
            inline bool operator()( const T* pLeft, const T* pRight ) const
            {
                return pLeft->getIndex() < pRight->getIndex();
            }
        };
        
		using ContextSet = std::set< interface::Context*, CompareNodePath< interface::Context > >;
		using ContextSetPtr = std::shared_ptr< ContextSet >;
		using ContextSetPtrSet = std::set< ContextSetPtr >;
		
		ContextSetPtrSet calculateSets( const std::vector< interface::Context* >& actions );
		ContextSetPtr find( const ContextSetPtrSet& sets, interface::Context* pAction );
		void addAction( ContextSetPtrSet& sets, interface::Context* pAction );
        
		using LinkSet = std::pair< std::string, ContextSetPtr >;
		using LinkGroupMap = std::multimap< LinkSet, interface::Link* >;
		
		void calculateGroups( const ContextSetPtrSet& sets,
            const std::vector< interface::Context* >& actions, 
			const DerivationAnalysis& derivationAnalysis, 
			AppendingSession& session );
		
		const LinkGroup::Vector& getLinkGroups() const { return m_groups; }
        
        const LinkGroup* getLinkGroup( const interface::Link* pLink ) const; 
        const LinkGroup* getLinkGroup( const std::string& strName ) const;
		
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
		
	private:
		LinkGroup::Vector m_groups;
    };
    
}

#endif //LINKANALYSIS_09_JUNE_2020