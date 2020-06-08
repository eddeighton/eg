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



#include "common.hpp"
#include "scheduler.hpp"
#include "clock.hpp"

#include <list>
#include <queue>

namespace
{
    class BasicScheduler
    {
        struct ActiveAction
        {
            eg::reference ref;
			eg::Scheduler::StopperFunctionPtr pStopper;
            eg::Scheduler::ActionOperator op;
			
			bool bAllEvents;
			std::vector< Event > events;
			
        };
        
        using ActiveActionList = std::list< ActiveAction >;
        
		
		using TimeoutAction = std::pair< std::chrono::steady_clock::time_point, ActiveAction >;
		struct ComparePriorities
		{
			inline bool operator()( const TimeoutAction& left, const TimeoutAction& right ) const
			{
				return left.first < right.first;
			}
		};
		using TimeoutQueue = std::priority_queue< TimeoutAction, std::vector< TimeoutAction >, ComparePriorities >;
		
	
    private:
        ActiveActionList m_ready, m_waiting, m_sleeping, m_sleepingUntilEvent;
		TimeoutQueue m_timeouts;
		
		
    public:
        void start( const eg::reference& ref, eg::Scheduler::StopperFunctionPtr pStopper, eg::Scheduler::ActionOperator action )
        {
            m_ready.push_back( ActiveAction{ ref, pStopper, action } );
        }
        
        void call( const eg::reference& ref, eg::Scheduler::StopperFunctionPtr pStopper, eg::Scheduler::ActionOperator action )
        {
            m_ready.push_back( ActiveAction{ ref, pStopper, action } );
        }
        
        void stop( const eg::reference& ref )
        {
        }
        
        void pause( const eg::reference& ref )
        {
        }
        
        void unpause( const eg::reference& ref )
        {
        }
    

        bool active()
        {
            return !m_ready.empty() || !m_waiting.empty() || !m_sleeping.empty() || !m_timeouts.empty();
        }

        //run a cycle
        void cycle()
        {
            using namespace eg;
			
			std::list< eg::reference > stops;
			
			//check the timeouts
			while( !m_timeouts.empty() )
			{
				TimeoutAction head = m_timeouts.top();
				if( head.first < std::chrono::steady_clock::now() )
				{
					m_timeouts.pop();
					m_ready.push_back( head.second );
				}
				else
				{
					break;
				}
			}
            
			//swap the sleep with ready
            if( m_ready.empty() && m_waiting.empty() )
            {
                m_ready.swap( m_sleeping );
            }
            
			//run until ready and waiting empty
			bool bMadeProgress = false;
            while( !( m_ready.empty() && m_waiting.empty() ) )
            {
                if( m_ready.empty() )
                {
					if( !bMadeProgress )
					{
						LOG( "Failed to make progress waiting for event" );
						throw std::runtime_error( "Failed to make progress waiting for event" );
					}
					bMadeProgress = false;
                    m_ready.swap( m_waiting );
                }
				
				ActiveAction action = m_ready.front();
				m_ready.pop_front();
				
				//does the action have an event criteria?
				bool bResumed = false;
				if( !action.events.empty() )
				{
					for( const eg::reference& ref : stops )
					{
						for( std::vector< Event >::iterator 
								i = action.events.begin(); i != action.events.end();  )
						{
							if( i->data == ref )
							{
								i = action.events.erase( i );
								if( !action.bAllEvents )
									bResumed = true;
							}
							else
							{
								++i;
							}
						}
					}
				}
				
				if( !( action.events.empty() || bResumed ) )
				{
					//put back on the end of the list
					m_waiting.push_back( action );
				}
				else
				{
					bMadeProgress = true;
					
					//run the action
					ReturnReason returnReason =
						action.op( ResumeReason() );
					
					switch( returnReason.reason )
					{
						case eReason_Wait:
							m_waiting.push_back( action );
							break;
						case eReason_Wait_All:
							action.events.swap( returnReason.events );
							action.bAllEvents = true;
							m_waiting.push_back( action );
							break;
						case eReason_Wait_Any:
							action.events.swap( returnReason.events );
							action.bAllEvents = false;
							m_waiting.push_back( action );
							break;
						case eReason_Sleep:
							m_sleeping.push_back( action );
							break;
						case eReason_Sleep_All:
							action.events.swap( returnReason.events );
							action.bAllEvents = true;
							m_sleepingUntilEvent.push_back( action );
							break;
						case eReason_Sleep_Any:
							action.events.swap( returnReason.events );
							action.bAllEvents = false;
							m_sleepingUntilEvent.push_back( action );
							break;
						case eReason_Timeout:
							m_timeouts.push( std::make_pair( returnReason.timeout.value(), action ) );
							break;
						case eReason_Terminated:
							action.pStopper( action.ref.instance );
							stops.push_back( action.ref );
							
							//whenever an event happens - test to see if anything is waiting for it
							{
								for( std::list< ActiveAction >::iterator 
									j = m_sleepingUntilEvent.begin();
										j != m_sleepingUntilEvent.end(); )
								{
									bool bResumed = false;
									ActiveAction& sleeperAction = *j;
									for( std::vector< Event >::iterator 
											i = sleeperAction.events.begin(); 
												i != sleeperAction.events.end();  )
									{
										if( i->data == action.ref )
										{
											i = sleeperAction.events.erase( i );
											if( !sleeperAction.bAllEvents )
												bResumed = true;
										}
										else
										{
											++i;
										}
									}
									if( sleeperAction.events.empty() || bResumed )
									{
										m_ready.push_front( sleeperAction ); //handle next
										j = m_sleepingUntilEvent.erase( j );
									}
									else
									{
										++j;
									}
								}
							}
							break;
						default:
							throw std::runtime_error( "Unknown return reason" );
					}
				}
            }
        }
        
    };


    BasicScheduler theScheduler;
}

namespace eg
{

    void Scheduler::start_ref( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action )
    {
        theScheduler.start( ref, pStopper, action );
    }
    
    void Scheduler::call_ref( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action )
    {
        theScheduler.call( ref, pStopper, action );
    }
    
    void Scheduler::stop_ref( const reference& ref )
    {
        theScheduler.stop( ref );
    }
    
    void Scheduler::pause_ref( const reference& ref )
    {
        theScheduler.pause( ref );
    }
    
    void Scheduler::unpause_ref( const reference& ref )
    {
        theScheduler.unpause( ref );
    }
    
    //are there any active actions
    bool Scheduler::active()
    {
        return theScheduler.active();
    }

    //run a cycle
    void Scheduler::cycle()
    {
        theScheduler.cycle();
    }
    
}
        