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



#include "eg_compiler/link.hpp"

namespace eg
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
	
	LinkAnalysis::ActionSetPtrSet LinkAnalysis::calculateSets( const std::vector< interface::Context* >& contexts )
	{
		ActionSetPtrSet sets;
		
		for( interface::Context* pContext : contexts )
		{
            if( !dynamic_cast< interface::Link* >( pContext ) )
			{
				addAction( sets, pContext );
			}
		}
        
        return sets;
	}
	
	LinkAnalysis::ActionSetPtr LinkAnalysis::find( const ActionSetPtrSet& sets, interface::Context* pContext )
	{
		ActionSetPtr pActionSet;
		for( ActionSetPtr pSet : sets )
		{
			if( pSet->count( pContext ) )
			{
				pActionSet = pSet;
				break;
			}
		}
		return pActionSet;
	}
	
	void LinkAnalysis::addAction( ActionSetPtrSet& sets, interface::Context* pContext )
	{
		VERIFY_RTE( !dynamic_cast< interface::Link* >( pContext ) );
		
		ActionSetPtr pActionSet = find( sets, pContext );
		
		for( interface::Context* pBase : pContext->getBaseContexts() )
		{
			if( ActionSetPtr pSet = find( sets, pBase ) )
			{
				if( pActionSet )
				{
					//merge
					pActionSet->insert( pSet->begin(), pSet->end() );
					sets.erase( pSet );
				}
				else
				{
					pActionSet = pSet;
				}
			}
		}
		
		
		if( !pActionSet )
		{
			pActionSet = std::make_shared< ActionSet >();
			sets.insert( pActionSet );
		}
		
		pActionSet->insert( pContext );
		pActionSet->insert( 
			pContext->getBaseContexts().begin(), 
			pContext->getBaseContexts().end() );
	}
    
    const interface::Context* LinkGroup::getLinkTarget( const interface::Link* pLink )
    {
        VERIFY_RTE_MSG( pLink->getBaseContexts().size() == 1U, 
            "Link does not have singular link target type: " << pLink->getIdentifier() );
        return pLink->getBaseContexts().front();
    }
    interface::Context* LinkGroup::getLinkTarget( interface::Link* pLink )
    {
        VERIFY_RTE_MSG( pLink->getBaseContexts().size() == 1U, 
            "Link does not have singular link target type: " << pLink->getIdentifier() );
        return pLink->getBaseContexts().front();
    }
	
	void LinkAnalysis::calculateGroups( const ActionSetPtrSet& sets,
            const std::vector< interface::Context* >& contexts, 
			const DerivationAnalysis& derivationAnalysis, 
			AppendingSession& session )
	{
		LinkGroupMap groupMap;
		
		for( interface::Context* pContext : contexts )
		{
			if( interface::Link* pLink = dynamic_cast< interface::Link* >( pContext ) )
			{
				interface::Context* pBase = LinkGroup::getLinkTarget( pLink );
				
				ActionSetPtr pSet = find( sets, pBase );
				VERIFY_RTE( pSet );
				
				groupMap.insert( std::make_pair( std::make_pair( pContext->getIdentifier(), pSet ), pLink ) );
			}
		}
		
		//collate results
		LinkGroupMap::iterator 
			i = groupMap.begin(),
			iEnd = groupMap.end();
			
		for( ; i!=iEnd; )
		{
			LinkGroup* pGroup = session.construct< LinkGroup >();
			
			pGroup->m_name = i->first.first;
			VERIFY_RTE( !pGroup->m_name.empty() );
			ActionSetPtr pSet = i->first.second;
			
			//get the links
			{
				LinkGroupMap::iterator iNext =
					groupMap.upper_bound( i->first );
				for( ; i != iNext; ++i )
				{
					pGroup->m_links.push_back( i->second );
				}
			}
			
			//get the concrete targets
			{
				std::vector< const concrete::Element* > concreteTargets;
				for( interface::Context* pContext : *pSet )
				{
					VERIFY_RTE( !dynamic_cast< interface::Link* >( pContext ) );
					derivationAnalysis.getInstances( pContext, concreteTargets, true );
				}
								
				concreteTargets =
					uniquify_without_reorder( concreteTargets );
					
				for( const concrete::Element* pElement : concreteTargets )
				{
					const concrete::Action* pContext = dynamic_cast< const concrete::Action* >( pElement );
					VERIFY_RTE( pContext );
					pGroup->m_concreteTargets.push_back( const_cast< concrete::Action* >( pContext ) );
				}
			}
			
			m_groups.push_back( pGroup );
		}
	}
    
    const LinkGroup* LinkAnalysis::getLinkGroup( const interface::Link* pLink ) const
    {
        for( const LinkGroup* pLinkGroup : m_groups )
        {
            const std::vector< interface::Link* >& links = pLinkGroup->getLinks();
            if( std::find( links.begin(), links.end(), pLink ) != links.end() )
                return pLinkGroup;
        }
        return nullptr;
    }
		
	void LinkGroup::load( Loader& loader )
	{
		loader.load( m_name );
		loader.loadObjectVector( m_links );
		loader.loadObjectVector( m_concreteTargets );
        loader.loadObjectMap( m_dimensionMap );
	}
	
	void LinkGroup::store( Storer& storer ) const
	{
		storer.store( m_name );
		storer.storeObjectVector( m_links );
		storer.storeObjectVector( m_concreteTargets );
        storer.storeObjectMap( m_dimensionMap );
	}
		
    void LinkAnalysis::load( Loader& loader )
    {
		loader.loadObjectVector( m_groups );
    }
	
    void LinkAnalysis::store( Storer& storer ) const
    {
		storer.storeObjectVector( m_groups );
    }
        

} //namespace eg