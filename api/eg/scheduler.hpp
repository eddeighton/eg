
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

#ifndef EG_SCHEDULER
#define EG_SCHEDULER

#include "common.hpp"
#include "clock.hpp"
#include "event.hpp"

#include <boost/fiber/all.hpp>
#include <boost/optional.hpp>

#include <chrono>
#include <queue>

using namespace std::chrono_literals;

eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance );
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance );
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance );
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance );

namespace eg
{
    
class termination_exception
{
};

class fiber_props : public boost::fibers::fiber_properties 
{
public:
    fiber_props( boost::fibers::context* pContext )
        :   fiber_properties( pContext ),
            m_cycle( 0U ),
            m_bIsTimekeeper( false ),
            m_bShouldContinue( true )
    {
    }
    
    TimeStamp getCycle() const
    {
        return m_cycle;
    }
    void setCycle( TimeStamp timestamp )
    {
        m_cycle = timestamp;
    }
    
    const reference& getReference() const
    {
        return m_reference;
    }
    void setReference( const reference& ref )
    { 
        if( m_reference != ref )
        {
            m_reference = ref;
        }
    }
    
    bool shouldContinue() const
    {
        return m_bShouldContinue;
    }
    void stop()
    {
        m_bShouldContinue = false;
    }
    
    bool isTimeKeeper() const { return m_bIsTimekeeper; }
    void setTimeKeeper() { m_bIsTimekeeper = true; }
    
    using ResumptionFunctor = std::function< bool( Event e ) >;
    
    template< typename ResumptionFunctorType >
    void setResumption( ResumptionFunctorType&& functor )
    {
        m_resumptionFunctor = functor;
    }
    ResumptionFunctor getResumption() const
    {
        return m_resumptionFunctor;
    }
    void resetResumption()
    {
        m_resumptionFunctor = ResumptionFunctor();
    }
    
    
    void setTimeout( const std::chrono::steady_clock::time_point& timeout )
    {
        m_timeout = timeout;
    }
    void resetTimeout()
    {
        m_timeout.reset();
    }
    boost::optional< std::chrono::steady_clock::time_point > getTimeout() const { return m_timeout; }
    
private:
    TimeStamp m_cycle;
    reference m_reference;
    bool m_bIsTimekeeper;
    bool m_bShouldContinue;
    ResumptionFunctor m_resumptionFunctor;
    boost::optional< std::chrono::steady_clock::time_point > m_timeout;
};


struct eg_algorithm : public boost::fibers::algo::algorithm_with_properties< fiber_props >
{
    using rqueue_t = boost::fibers::scheduler::ready_queue_type ;
    
    using ContextPriority = std::pair< std::chrono::steady_clock::time_point, boost::fibers::context* >;
    struct ComparePriorities
    {
        inline bool operator()( const ContextPriority& left, const ContextPriority& right ) const
        {
            return left.first < right.first;
        }
    };
    using TimeoutQueue = std::priority_queue< ContextPriority, std::vector< ContextPriority >, ComparePriorities >;
    
    rqueue_t m_queue_ready;
    rqueue_t m_queue_sleep;
    rqueue_t m_queue_resume;
    TimeoutQueue m_timeoutPriorityQueue;
    boost::fibers::context* m_pTimeKeeper = nullptr;
    
    std::mutex                  mtx_{};
    std::condition_variable     cnd_{};
    bool                        flag_{ false };
    event_iterator m_eventIterator;

    eg_algorithm() 
        :   m_queue_ready(), 
            m_queue_sleep(),
            m_queue_resume(),
            m_eventIterator( events::getIterator() )
    {
    }
    
    ~eg_algorithm()
    {
    }
    
    const fiber_props& getContextProps( const boost::fibers::context* pContext ) const noexcept
    {
        return const_cast< eg_algorithm* >( this )->properties( 
            const_cast< boost::fibers::context* >( pContext ) );
    }
    
    bool isActionFiber( const boost::fibers::context* pContext ) const noexcept
    {
        return getContextProps( pContext ).getReference().type != 0;
    }
    
    bool isTimeKeeperFiber( const boost::fibers::context* pContext ) const noexcept
    {
        return getContextProps( pContext ).isTimeKeeper();
    }

    virtual void awakened( boost::fibers::context* pContext, fiber_props& props ) noexcept
    {
        //blockedWaitingFibers.erase( pContext->get_id() );
        
        //if a timeout is set then put the fiber into the timeout queue
        if( boost::optional< std::chrono::steady_clock::time_point > timeout = props.getTimeout() )
        {
            m_timeoutPriorityQueue.push( std::make_pair( timeout.get(), pContext ) );
        }
        
        //if the fiber has a resumption criteria set then put it straight into the resume queue
        else if( fiber_props::ResumptionFunctor functor = props.getResumption() )
        {
            m_queue_resume.push_back( *pContext );
        }
        
        //else if the fiber IS an action fiber then interogate the action state to decide what to do
        else if( isActionFiber( pContext ) )
        {
            const reference& ref = props.getReference();
            switch( getState( ref.type, ref.instance ) )
            {
                case action_running:
                    {
                        //if the fiber cycle is greater than the current then
                        //the fiber has gone to sleep for this cycle
                        if( props.getCycle() <= clock::cycle() )
                        {
                            m_queue_ready.push_back( *pContext );
                        }
                        else
                        {
                            m_queue_sleep.push_back( *pContext );
                        }
                    }
                    break;
                case action_paused:
                    {
                        //m_queue_pause.push_back( *pContext );
                    }
                    break;
                case action_stopped:
                    {
                        m_queue_ready.push_back( *pContext );
                    }
                    break;
                default:
                    //unreachable
                    std::terminate();
            }
        }
        //if the fiber is the time keeper - i.e. the main host fiber
        //then we simple record the pointer for use in pick_next
        else if( isTimeKeeperFiber( pContext ) )
        {
            m_pTimeKeeper = pContext;
            
            //if all the queues are empty then we have completed the program and can stop
            if( m_queue_ready.empty() && m_queue_sleep.empty() && m_queue_resume.empty() && m_timeoutPriorityQueue.empty() )
            {
                props.stop();
            }
        }
        else 
        {
            if( props.getCycle() <= clock::cycle() )
            {
                m_queue_ready.push_back( *pContext );
            }
            else
            {
                m_queue_sleep.push_back( *pContext );
            }
        }
        
    }

    virtual boost::fibers::context* pick_next() noexcept
    {
        //check the timeouts
        while( !m_timeoutPriorityQueue.empty() )
        {
            ContextPriority head = m_timeoutPriorityQueue.top();
            if( head.first < std::chrono::steady_clock::now() )
            {
                m_timeoutPriorityQueue.pop();
                properties( head.second ).resetTimeout();
                m_queue_ready.push_back( *head.second );
            }
            else
            {
                break;
            }
        }
        
        //attempt to dispatch events first
        if( !m_queue_resume.empty() )
        {
            //scan to the end of the event log and attempt to resume any
            //fibers waiting for the event
            Event e;
            while( events::get( m_eventIterator, e ) )
            {
                for( rqueue_t::iterator 
                    i = m_queue_resume.begin(),
                    iEnd = m_queue_resume.end(); i!=iEnd; )
                {
                    boost::fibers::context* pContext( &*i );
                    fiber_props& props = properties( pContext );
                    if( props.getResumption()( e ) )
                    {
                        //to resume the fiber just move it to the ready queue
                        props.resetResumption();
                        i = m_queue_resume.erase( i );
                        m_queue_ready.push_back( *pContext );
                    }
                    else
                    {
                        ++i;
                    }
                }
            }
        }
        /*else if( !m_queue_ready.empty() )
        {
            //skip
            m_eventIterator = events::getIterator();
        }*/
        
        //return the head of the ready queue if not empty
        if( !m_queue_ready.empty() )
        {
            boost::fibers::context* pContext( &*m_queue_ready.begin() );
            m_queue_ready.pop_front();
            return pContext;
        }
        
        //otherwise the cycle is complete and we return the timekeeper IF we have it
        if( m_pTimeKeeper )
        {
            m_queue_ready.swap( m_queue_sleep );
                    
            boost::fibers::context* pContext = m_pTimeKeeper;
            m_pTimeKeeper = nullptr;
            return pContext;
        }
        
        return nullptr;
    }

    virtual bool has_ready_fibers() const noexcept
    {
        return !m_queue_ready.empty();
    }
    
    virtual void property_change( boost::fibers::context * ctx, fiber_props & props ) noexcept 
    {
        //from example code - not actually used...
        if ( !ctx->ready_is_linked() ) 
        { 
            return;
        }
        ctx->ready_unlink();
        awakened( ctx, props);
    }

    virtual void suspend_until( std::chrono::steady_clock::time_point const& timePoint ) noexcept
    {
        if ( (std::chrono::steady_clock::time_point::max)() == timePoint ) 
        {
            //if the program fails to terminate gracefully this may happen
            //usually there are fibers still alive while the fiber manager is shutting down
            std::unique_lock< std::mutex > lk( mtx_ );
            cnd_.wait( lk, [this](){ return flag_; } );
            flag_ = false;
            std::cout << "."; 
        } 
        else 
        {
            std::unique_lock< std::mutex > lk( mtx_ );
            cnd_.wait_until( lk, timePoint, [this](){ return flag_; } );
            flag_ = false;
        }
    }

    virtual void notify() noexcept
    {
        std::unique_lock< std::mutex > lk( mtx_ );
        flag_ = true;
        lk.unlock();
        cnd_.notify_all();
    }
};

inline void checkIfStopped()
{
    const reference& ref = boost::this_fiber::properties< eg::fiber_props >().getReference();
    if( ref.type != 0 && getState( ref.type, ref.instance ) == action_stopped )
    {
        //use exception to terminate the fiber - this will be caught by the lambda that started it.
        throw eg::termination_exception();
    }
}

inline void sleep()
{
    checkIfStopped();
    //set the cycle to the next cycle indicating that the fiber is now sleeping
    boost::this_fiber::properties< eg::fiber_props >().setCycle( clock::cycle() + 1 );
    boost::this_fiber::yield();
    checkIfStopped();
}

//allow user to specify arbitrary resumption function
template< typename ResumptionFunctorType >
inline auto sleep( ResumptionFunctorType functor ) -> decltype( functor( Event() ), bool() )
{
    checkIfStopped();
    boost::this_fiber::properties< eg::fiber_props >().setResumption( functor );
    boost::this_fiber::yield();
    checkIfStopped();
    return false;
}

inline void sleep( Event event )
{
    checkIfStopped();
    //sleeping on an event means the fiber can be reawoken in the current cycle
    boost::this_fiber::properties< eg::fiber_props >().setResumption
    (  
        [ event ]( Event e )
        {
            return ( e == event );
        }
    );
    boost::this_fiber::yield();
    checkIfStopped();
}

template< typename Clock, typename Duration >
inline void sleep( std::chrono::time_point< Clock, Duration > const& sleep_time )
{
    checkIfStopped();
    boost::this_fiber::properties< eg::fiber_props >().setTimeout( sleep_time );
    boost::this_fiber::yield();
    checkIfStopped();
}


template< typename Rep, typename Period >
inline void sleep( std::chrono::duration< Rep, Period > const& timeout_duration )
{
    checkIfStopped();
    boost::this_fiber::properties< eg::fiber_props >().setTimeout( std::chrono::steady_clock::now() + timeout_duration );
    boost::this_fiber::yield();
    checkIfStopped();
}

inline void wait()
{
    boost::this_fiber::yield();
}

}

#endif //EG_SCHEDULER