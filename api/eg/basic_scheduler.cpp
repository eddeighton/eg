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
#include "coroutine.hpp"
#include "scheduler.hpp"
#include "clock.hpp"

#include <list>
#include <map>
#include <unordered_map>
#include <optional>

namespace
{

    class BasicScheduler
    {
    private:
        struct ActiveAction;
        
        struct ReferenceHash
        {
            inline std::size_t operator()( const eg::reference& ref ) const
            {
                #define MAX_TYPES 1024
                return ref.instance * MAX_TYPES + ref.type;
            }
        };
            
        using ActiveActionMap = std::unordered_map< eg::reference, ActiveAction*, BasicScheduler::ReferenceHash >;
        using ActiveActionList = std::list< ActiveActionMap::iterator >;
        
		using Timeout = std::chrono::steady_clock::time_point;
        using TimeoutQueue = std::multimap< Timeout, ActiveActionMap::iterator >;
        
        struct ActiveAction
        {
            eg::reference ref;
			eg::Scheduler::StopperFunctionPtr pStopper;
            eg::Scheduler::ActionOperator op;
			
            eg::ActionCoroutine coroutine;
			//bool bAllEvents;
			//std::vector< Event > events;
            
            //action must only occur in one of the iterators at a time
            
            ActiveActionList::iterator iter_one, iter_two, iter_three;
            TimeoutQueue::iterator iter_timeout;
            ActiveActionList::iterator iter_pause;
            
            ActiveAction() = delete;
            ActiveAction( const ActiveAction& ) = delete;
            
            ActiveAction( const eg::reference& _ref, 
                eg::Scheduler::StopperFunctionPtr _pStopper, 
                eg::Scheduler::ActionOperator& _op,
                ActiveActionList::iterator _iter_one,
                ActiveActionList::iterator _iter_two,
                ActiveActionList::iterator _iter_three,
                ActiveActionList::iterator _iter_pause  )
                :
                    ref( _ref ),
                    pStopper( _pStopper ),
                    op( _op ),
                    
                    iter_one( _iter_one ),
                    iter_two( _iter_two ),
                    iter_three( _iter_three ),
                    iter_pause( _iter_pause )
            {
                
            }
        };
        
        
	
        ActiveActionMap m_actions;
        
        ActiveActionList m_listOne, m_listTwo, m_listThree;
		TimeoutQueue m_timeouts;
        ActiveActionList m_paused;
        
        enum SleepSwapState
        {
            eState_A_W_S, //active, wait, sleep
            eState_A_S_W,
            eState_W_A_S,
            eState_W_S_A,
            eState_S_A_W,
            eState_S_W_A
        };
        SleepSwapState m_sleepState = eState_A_W_S;
        
        ActiveActionList::iterator& getActiveIter( ActiveAction& action )
        {
           switch( m_sleepState )
           {
              case eState_A_W_S: return action.iter_one;
              case eState_A_S_W: return action.iter_one;
              case eState_W_A_S: return action.iter_two;
              case eState_W_S_A: return action.iter_three;
              case eState_S_A_W: return action.iter_two;
              case eState_S_W_A: return action.iter_three;
           }
        }
        ActiveActionList::iterator& getWaitIter( ActiveAction& action )
        {
           switch( m_sleepState )
           {
              case eState_A_W_S: return action.iter_two;
              case eState_A_S_W: return action.iter_three;
              case eState_W_A_S: return action.iter_one;
              case eState_W_S_A: return action.iter_one;
              case eState_S_A_W: return action.iter_three;
              case eState_S_W_A: return action.iter_two;
           }
        }
        ActiveActionList::iterator& getSleepIter( ActiveAction& action )
        {
           switch( m_sleepState )
           {
              case eState_A_W_S: return action.iter_three;
              case eState_A_S_W: return action.iter_two;
              case eState_W_A_S: return action.iter_three;
              case eState_W_S_A: return action.iter_two;
              case eState_S_A_W: return action.iter_one;
              case eState_S_W_A: return action.iter_one;
           }
        }
        
        void swapActiveSleep()
        {
           switch( m_sleepState )
           {
              case eState_A_W_S: m_sleepState = eState_S_W_A; break;
              case eState_A_S_W: m_sleepState = eState_S_A_W; break;
              case eState_W_A_S: m_sleepState = eState_W_S_A; break;
              case eState_W_S_A: m_sleepState = eState_W_A_S; break;
              case eState_S_A_W: m_sleepState = eState_A_S_W; break;
              case eState_S_W_A: m_sleepState = eState_A_W_S; break;
           }
        }
        
        void swapActiveWait()
        {
           switch( m_sleepState )
           {
              case eState_A_W_S: m_sleepState = eState_W_A_S; break;
              case eState_A_S_W: m_sleepState = eState_W_S_A; break;
              case eState_W_A_S: m_sleepState = eState_A_W_S; break;
              case eState_W_S_A: m_sleepState = eState_A_S_W; break;
              case eState_S_A_W: m_sleepState = eState_S_W_A; break;
              case eState_S_W_A: m_sleepState = eState_S_A_W; break;
           }
        }
        
        ActiveActionList& getActive()
        {
            switch( m_sleepState )
            {
              case eState_A_W_S: return m_listOne;
              case eState_A_S_W: return m_listOne;
              case eState_W_A_S: return m_listTwo;
              case eState_W_S_A: return m_listThree;
              case eState_S_A_W: return m_listTwo;
              case eState_S_W_A: return m_listThree;
            }
        }
        
        ActiveActionList& getWaiting()
        {
            switch( m_sleepState )
            {
              case eState_A_W_S: return m_listTwo;
              case eState_A_S_W: return m_listThree;
              case eState_W_A_S: return m_listOne;
              case eState_W_S_A: return m_listOne;
              case eState_S_A_W: return m_listThree;
              case eState_S_W_A: return m_listTwo;
            }
        }
        
        ActiveActionList& getSleeping()
        {
            switch( m_sleepState )
            {
              case eState_A_W_S: return m_listThree;
              case eState_A_S_W: return m_listTwo;
              case eState_W_A_S: return m_listThree;
              case eState_W_S_A: return m_listTwo;
              case eState_S_A_W: return m_listOne;
              case eState_S_W_A: return m_listOne;
            }
        }
        
        void active_insert( ActiveActionMap::iterator iterAction )
        {
            ActiveActionList& list = getActive();
            getActiveIter( *iterAction->second ) = list.insert( list.end(), iterAction );
        }
        void active_remove( ActiveActionMap::iterator iterAction )
        {
            ActiveActionList& list = getActive();
            ActiveActionList::iterator& i = getActiveIter( *iterAction->second );
            if( i != list.end() )
            {
                list.erase( i );
                i = list.end();
            }
        }
        
        void wait_insert( ActiveActionMap::iterator iterAction )
        {
            ActiveActionList& list = getWaiting();
            getWaitIter( *iterAction->second ) = list.insert( list.end(), iterAction );
        }
        void wait_remove( ActiveActionMap::iterator iterAction )
        {
            ActiveActionList& list = getWaiting();
            ActiveActionList::iterator& i = getWaitIter( *iterAction->second );
            if( i != list.end() )
            {
                list.erase( i );
                i = list.end();
            }
        }
        
        void sleep_insert( ActiveActionMap::iterator iterAction )
        {
            ActiveActionList& list = getSleeping();
            getSleepIter( *iterAction->second ) = list.insert( list.end(), iterAction );
        }
        void sleep_remove( ActiveActionMap::iterator iterAction )
        {
            ActiveActionList& list = getSleeping();
            ActiveActionList::iterator& i = getSleepIter( *iterAction->second );
            if( i != list.end() )
            {
                list.erase( i );
                i = list.end();
            }
        }
        
        
        void pause_insert( ActiveActionMap::iterator iterAction )
        {
            iterAction->second->iter_pause = m_paused.insert( m_paused.end(), iterAction );
        }
        void pause_remove( ActiveActionMap::iterator iterAction )
        {
            if( iterAction->second->iter_pause != m_paused.end() )
            {
                m_paused.erase( iterAction->second->iter_pause );
                iterAction->second->iter_pause = m_paused.end();
            }
        }
        
		//using Timeout = std::chrono::steady_clock::time_point;
        //using TimeoutQueue = std::multimap< Timeout, ActiveActionMap::iterator >;
        void timeout_insert( ActiveActionMap::iterator iterAction, const Timeout& timeout )
        {
            TimeoutQueue::iterator insertResult =
                m_timeouts.insert( std::make_pair( timeout, iterAction ) );
            iterAction->second->iter_timeout = insertResult;
        }
        void timeout_remove( ActiveActionMap::iterator iterAction )
        {
            if( iterAction->second->iter_timeout != m_timeouts.end() )
            {
                m_timeouts.erase( iterAction->second->iter_timeout );
                iterAction->second->iter_timeout = m_timeouts.end();
            }
        }
        
		
    public:
    
        void call( const eg::reference& ref, eg::Scheduler::StopperFunctionPtr pStopper, eg::Scheduler::ActionOperator actionOperator )
        {
            ActiveAction* pAction = new ActiveAction( ref, pStopper, actionOperator, 
                            getActive().end(), 
                            getWaiting().end(), 
                            getSleeping().end(),
                            m_paused.end() );
                            
             
            ActiveActionMap::_Pairib insertResult =
                m_actions.insert( std::make_pair( ref, pAction ) );
            if( insertResult.second )
            {
                active_insert( insertResult.first );
            }
        }
        
        void stop( const eg::reference& ref )
        {
            ActiveActionMap::iterator iFind = m_actions.find( ref );
            if( iFind != m_actions.end() )
            {
                ActiveAction* pAction = iFind->second;
                
                active_remove( iFind );
                wait_remove( iFind );
                sleep_remove( iFind );
                m_actions.erase( iFind );
                
                //invoke the stopper
                pAction->pStopper( pAction->ref.instance );
                
                delete pAction;
                
            }
            else
            {
                ERR( "Stopped inactive reference" );
            }
        }
        
        void pause( const eg::reference& ref )
        {
            ActiveActionMap::iterator iFind = m_actions.find( ref );
            if( iFind != m_actions.end() )
            {
                active_remove( iFind );
                wait_remove( iFind );
                sleep_remove( iFind );
                pause_insert( iFind );
            }
            else
            {
                ERR( "Stopped inactive reference" );
            }
        }
        
        void unpause( const eg::reference& ref )
        {
            ActiveActionMap::iterator iFind = m_actions.find( ref );
            if( iFind != m_actions.end() )
            {
                pause_remove( iFind );
                active_insert( iFind );
            }
            else
            {
                ERR( "Stopped inactive reference" );
            }
        }
    

        bool active()
        {
            return !m_actions.empty();
        }

        //run a cycle
        void cycle()
        {
            using namespace eg;
            
            //timeouts 
            while( !m_timeouts.empty() )
            {
                TimeoutQueue::iterator iterTimeout = m_timeouts.begin();
                if( iterTimeout->first <= std::chrono::steady_clock::now() )
                {
                    ActiveActionMap::iterator iter = iterTimeout->second;
                    timeout_remove( iter );
                    active_insert( iter );
                }
                else
                {
                    break;
                }
            }
            
            if( getActive().empty() && getWaiting().empty() )
            {
                swapActiveSleep();
            }
            
            //run until everything is sleeping
            while( !( getActive().empty() && getWaiting().empty() ) )
            {
                if( getActive().empty() )
                {
                    swapActiveWait();
                }
                
                ActiveActionMap::iterator iter = getActive().front();
                active_remove( iter );
                
                ActiveAction* pAction = iter->second;
                
                if( !pAction->coroutine.started() || pAction->coroutine.done() )
                {
                    pAction->coroutine = pAction->op( ResumeReason() );
                    pAction->coroutine.resume();
                }
                else
                {
                    pAction->coroutine.resume();
                }
                
                const eg::ReturnReason& reason = 
                    pAction->coroutine.getReason();
                switch( reason.reason )
                {
                    case eReason_Wait:
                        wait_insert( iter );
                        break;
                    case eReason_Wait_All:
                        break;
                    case eReason_Wait_Any:
                        break;
                    case eReason_Sleep:
                        sleep_insert( iter );
                        break;
                    case eReason_Sleep_All:
                        break;
                    case eReason_Sleep_Any:
                        break;
                    case eReason_Timeout:
                        timeout_insert( iter, reason.timeout.value() );
                        break;
                    case eReason_Terminated:
                        pAction->pStopper( pAction->ref.instance );
                        m_actions.erase( iter );
                        break;
                    default:
                        throw std::runtime_error( "Unknown return reason" );
                }
            }
            
        }
        
    };


    BasicScheduler theScheduler;
}

namespace eg
{
    void Scheduler::allocated_ref( const reference& ref, StopperFunctionPtr pStopper )
    {
    }
    
    void Scheduler::call_ref( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action )
    {
        theScheduler.call( ref, pStopper, action );
    }
    
    void Scheduler::signal_ref( const reference& ref, StopperFunctionPtr pStopper  )
    {
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
    
    void Scheduler::zeroRefCount( const reference& ref, eg::Instance* pRefCount )
    {
        
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
        