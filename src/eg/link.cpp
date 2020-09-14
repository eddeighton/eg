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
	
    //calculateSets called in the interface_session linkAnalysis as the first step
	LinkAnalysis::ContextSetPtrSet LinkAnalysis::calculateSets( const std::vector< interface::Context* >& contexts )
	{
		ContextSetPtrSet sets; //set of disjoint sets of contexts
		
        //for ALL contexts that are NOT links establish their membership in the ContextSetPtrSet
        
		for( interface::Context* pContext : contexts )
		{
            if( !dynamic_cast< interface::Link* >( pContext ) )
			{
				addAction( sets, pContext );
			}
		}
        
        return sets;
	}
	
	LinkAnalysis::ContextSetPtr LinkAnalysis::find( const ContextSetPtrSet& sets, interface::Context* pContext )
	{
		ContextSetPtr pContextSet;
		for( ContextSetPtr pSet : sets )
		{
			if( pSet->count( pContext ) )
			{
				pContextSet = pSet;
				break;
			}
		}
		return pContextSet;
	}
	
    //Construct the set of disjoint sets of Context sets
    //Each disjoint set combines contexts where:
    //  Contexts are in a parent, child relation i.e. one context has the other as a base context
    //The algorithm produces the transitive closure such that for any context that derives from a common 
    //context it will occur within the SAME set as any other type that derives from that common context.
	void LinkAnalysis::addAction( ContextSetPtrSet& sets, interface::Context* pContext )
	{
		VERIFY_RTE( !dynamic_cast< interface::Link* >( pContext ) );
		
		ContextSetPtr pContextSet = find( sets, pContext );
		
		for( interface::Context* pBase : pContext->getBaseContexts() )
		{
			if( ContextSetPtr pSet = find( sets, pBase ) )
			{
				if( pContextSet )
				{
					//merge
					pContextSet->insert( pSet->begin(), pSet->end() );
					sets.erase( pSet );
				}
				else
				{
					pContextSet = pSet;
                    pContextSet->insert( pContext );
				}
			}
		}
		
		if( !pContextSet )
		{
			pContextSet = std::make_shared< ContextSet >();
            
            pContextSet->insert( pContext );
            pContextSet->insert( 
                pContext->getBaseContexts().begin(), 
                pContext->getBaseContexts().end() );
                
			sets.insert( pContextSet );
		}
	}
    
    const interface::Context* LinkGroup::getLinkTarget( const interface::Link* pLink )
    {
        VERIFY_RTE_MSG( pLink->getBaseContexts().size() == 1U, 
            "Link does not have singular link target type: " << pLink->getFriendlyName() );
        return pLink->getBaseContexts().front();
    }
    interface::Context* LinkGroup::getLinkTarget( interface::Link* pLink )
    {
        VERIFY_RTE_MSG( pLink->getBaseContexts().size() == 1U, 
            "Link does not have singular link target type: " << pLink->getFriendlyName() );
        return pLink->getBaseContexts().front();
    }
	
    //calculateSets called in the interface_session linkAnalysis as the second step
	void LinkAnalysis::calculateGroups( const ContextSetPtrSet& sets,
            const std::vector< interface::Context* >& contexts, 
			const DerivationAnalysis& derivationAnalysis, 
			AppendingSession& session )
	{
		LinkGroupMap groupMap;
		
        //enumerate ALL links
		for( interface::Context* pContext : contexts )
		{
			if( interface::Link* pLink = dynamic_cast< interface::Link* >( pContext ) )
			{
				interface::Context* pBase = LinkGroup::getLinkTarget( pLink );
				VERIFY_RTE( pBase );
				
				ContextSetPtr pSet = find( sets, pBase );
				VERIFY_RTE_MSG( pSet, "Failed to locate context set for context: " << pBase->getFriendlyName() );
				
                //Create an map fromentry containing the:
                //    Link Group Name
                //    The context set for the link target - which consitutes the set of all possible interface types that might be
                //        valid types which the link can be set to ( note note the subset of valid concrete types )
                //
                //    to the link itself
				groupMap.insert( std::make_pair( std::make_pair( pContext->getIdentifier(), pSet ), pLink ) );
			}
		}
        
        //so the result is that a link group is created for every unique std::make_pair( pContext->getIdentifier(), pSet ) combination.
        //the context set will be the same when a link is to any context within that set.
		
		//collate results
		LinkGroupMap::iterator 
			i = groupMap.begin(),
			iEnd = groupMap.end();
			
		for( ; i!=iEnd; )
		{
            
			LinkGroup* pGroup = session.construct< LinkGroup >();
			
			pGroup->m_name = i->first.first;
			VERIFY_RTE( !pGroup->m_name.empty() );
			ContextSetPtr pSet = i->first.second;
            
            //interface targets
            {
				for( interface::Context* pContext : *pSet )
                {
                    pGroup->m_interfaceTargets.push_back( pContext );
                }
            }
			
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
            
            //get the concrete links
			{
				std::vector< const concrete::Element* > concreteLinks;
				for( interface::Context* pLink : pGroup->m_links )
				{
					VERIFY_RTE( dynamic_cast< interface::Link* >( pLink ) );
					derivationAnalysis.getInstances( pLink, concreteLinks, true );
				}
								
				concreteLinks =
					uniquify_without_reorder( concreteLinks );
					
				for( const concrete::Element* pElement : concreteLinks )
				{
					const concrete::Action* pContext = dynamic_cast< const concrete::Action* >( pElement );
					VERIFY_RTE( pContext );
					pGroup->m_concreteLinks.push_back( const_cast< concrete::Action* >( pContext ) );
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
    const LinkGroup* LinkAnalysis::getLinkGroup( const std::string& strName ) const
    {
        for( const LinkGroup* pLinkGroup : m_groups )
        {
            if( pLinkGroup->getLinkName() == strName )
                return pLinkGroup;
        }
        return nullptr;
    }
		
	void LinkGroup::load( Loader& loader )
	{
		loader.load( m_name );
		loader.loadObjectVector( m_links );
		loader.loadObjectVector( m_interfaceTargets );
		loader.loadObjectVector( m_concreteTargets );
		loader.loadObjectVector( m_concreteLinks );
        loader.loadObjectMap( m_dimensionMap );
	}
	
	void LinkGroup::store( Storer& storer ) const
	{
		storer.store( m_name );
		storer.storeObjectVector( m_links );
		storer.storeObjectVector( m_interfaceTargets );
		storer.storeObjectVector( m_concreteTargets );
		storer.storeObjectVector( m_concreteLinks );
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