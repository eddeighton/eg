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
	
	LinkAnalysis::ActionSetPtrSet LinkAnalysis::calculateSets( const std::vector< interface::Action* >& actions )
	{
		ActionSetPtrSet sets;
		
		for( interface::Action* pAction : actions )
		{
			if( !pAction->isLink() )
			{
				addAction( sets, pAction );
			}
		}
        
        return sets;
	}
	
	LinkAnalysis::ActionSetPtr LinkAnalysis::find( const ActionSetPtrSet& sets, interface::Action* pAction )
	{
		ActionSetPtr pActionSet;
		for( ActionSetPtr pSet : sets )
		{
			if( pSet->count( pAction ) )
			{
				pActionSet = pSet;
				break;
			}
		}
		return pActionSet;
	}
	
	void LinkAnalysis::addAction( ActionSetPtrSet& sets, interface::Action* pAction )
	{
		VERIFY_RTE( !pAction->isLink() );
		
		ActionSetPtr pActionSet = find( sets, pAction );
		
		for( interface::Action* pBase : pAction->getBaseActions() )
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
		
		pActionSet->insert( pAction );
		pActionSet->insert( 
			pAction->getBaseActions().begin(), 
			pAction->getBaseActions().end() );
	}
    
    const interface::Action* LinkGroup::getLinkTarget( const interface::Action* pLink )
    {
        VERIFY_RTE( pLink->isLink() );
        
        VERIFY_RTE_MSG( pLink->getBaseActions().size() == 1U, 
            "Link does not have singular link target type: " << pLink->getIdentifier() );
        
        return pLink->getBaseActions().front();
    }
    interface::Action* LinkGroup::getLinkTarget( interface::Action* pLink )
    {
        VERIFY_RTE( pLink->isLink() );
        
        VERIFY_RTE_MSG( pLink->getBaseActions().size() == 1U, 
            "Link does not have singular link target type: " << pLink->getIdentifier() );
        
        return pLink->getBaseActions().front();
    }
	
	void LinkAnalysis::calculateGroups( const ActionSetPtrSet& sets,
            const std::vector< interface::Action* >& actions, 
			const DerivationAnalysis& derivationAnalysis, 
			AppendingSession& session )
	{
		LinkGroupMap groupMap;
		
		for( interface::Action* pAction : actions )
		{
			if( pAction->isLink() )
			{
				interface::Action* pBase = LinkGroup::getLinkTarget( pAction );
				
				ActionSetPtr pSet = find( sets, pBase );
				VERIFY_RTE( pSet );
				
				groupMap.insert( std::make_pair( std::make_pair( pAction->getIdentifier(), pSet ), pAction ) );
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
					VERIFY_RTE( i->second->isLink() );
					pGroup->m_links.push_back( i->second );
				}
			}
			
			//get the concrete targets
			{
				std::vector< const concrete::Element* > concreteTargets;
				for( interface::Action* pAction : *pSet )
				{
					VERIFY_RTE( !pAction->isLink() );
					derivationAnalysis.getInstances( pAction, concreteTargets, true );
				}
								
				concreteTargets =
					uniquify_without_reorder( concreteTargets );
					
				for( const concrete::Element* pElement : concreteTargets )
				{
					const concrete::Action* pAction = dynamic_cast< const concrete::Action* >( pElement );
					VERIFY_RTE( pAction );
					pGroup->m_concreteTargets.push_back( const_cast< concrete::Action* >( pAction ) );
				}
			}
			
			m_groups.push_back( pGroup );
		}
	}
    
    const LinkGroup* LinkAnalysis::getLinkGroup( const interface::Action* pLink ) const
    {
        for( const LinkGroup* pLinkGroup : m_groups )
        {
            const std::vector< interface::Action* >& links = pLinkGroup->getLinks();
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