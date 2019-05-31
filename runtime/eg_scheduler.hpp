
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

#include "eg_common.hpp"
#include "eg_include.hpp"

#include <boost/fiber/all.hpp>

#include <chrono>

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
    
private:
    TimeStamp m_cycle;
    reference m_reference;
    bool m_bIsTimekeeper;
    bool m_bShouldContinue;
    ResumptionFunctor m_resumptionFunctor;
};


struct eg_algorithm : public boost::fibers::algo::algorithm_with_properties< fiber_props >
{
    typedef boost::fibers::scheduler::ready_queue_type rqueue_t;
    
    rqueue_t m_queue_ready;
    rqueue_t m_queue_sleep;
    rqueue_t m_queue_resume;
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
        
        if( fiber_props::ResumptionFunctor functor = props.getResumption() )
        {
            m_queue_resume.push_back( *pContext );
        }
        else if( isActionFiber( pContext ) )
        {
            const reference& ref = props.getReference();
            switch( getState( ref.type, ref.instance ) )
            {
                case action_running:
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
        else if( isTimeKeeperFiber( pContext ) )
        {
            m_pTimeKeeper = pContext;
            
            if( m_queue_ready.empty() && m_queue_sleep.empty() && m_queue_resume.empty() )
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
        if( !m_queue_resume.empty() )
        {
            eg::_event ev;
            while( events::get( m_eventIterator, ev ) )
            {
                if( 0U == strcmp( ev.type, "stop" ) )
                {
                    Event e( *reinterpret_cast< const reference* >( ev.value ) );
                
                    for( rqueue_t::iterator 
                        i = m_queue_resume.begin(),
                        iEnd = m_queue_resume.end(); i!=iEnd; )
                    {
                        boost::fibers::context* pContext( &*i );
                        fiber_props& props = properties( pContext );
                        if( props.getResumption()( e ) )
                        {
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
        }
        /*else if( !m_queue_ready.empty() )
        {
            //skip
            m_eventIterator = events::getIterator();
        }*/
        
        if( !m_queue_ready.empty() )
        {
            boost::fibers::context* pContext( &*m_queue_ready.begin() );
            m_queue_ready.pop_front();
            return pContext;
        }
        
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
        // Although our priority_props class defines multiple properties, only
        // one of them (priority) actually calls notify() when changed. The
        // point of a property_change() override is to reshuffle the ready
        // queue according to the updated priority value.
//<-
        //std::cout << "property_change(" << props.name << '(' << props.get_priority()
        //          << ")): ";
//->

        // 'ctx' might not be in our queue at all, if caller is changing the
        // priority of (say) the running fiber. If it's not there, no need to
        // move it: we'll handle it next time it hits awakened().
        if ( !ctx->ready_is_linked() ) 
        { /*<
            Your `property_change()` override must be able to
            handle the case in which the passed `ctx` is not in
            your ready queue. It might be running, or it might be
            blocked. >*/
//<-
            // hopefully user will distinguish this case by noticing that
            // the fiber with which we were called does not appear in the
            // ready queue at all
            //describe_ready_queue();
//->
            return;
        }

        // Found ctx: unlink it
        ctx->ready_unlink();

        // Here we know that ctx was in our ready queue, but we've unlinked
        // it. We happen to have a method that will (re-)add a context* to the
        // right place in the ready queue.
        awakened( ctx, props);
    }

    virtual void suspend_until( std::chrono::steady_clock::time_point const& timePoint ) noexcept
    {
        if ( (std::chrono::steady_clock::time_point::max)() == timePoint ) 
        {
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
        //std::cout << "checkIfStopped " << ref.type << " " << ref.instance << std::endl;
        throw eg::termination_exception();
    }
}

inline void sleep()
{
    checkIfStopped();
    boost::this_fiber::properties< eg::fiber_props >().setCycle( clock::cycle() + 1 );
    boost::this_fiber::yield();
    checkIfStopped();
}

inline void sleep( Event event )
{
    checkIfStopped();
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


inline void sleep( fiber_props::ResumptionFunctor functor )
{
    checkIfStopped();
    boost::this_fiber::properties< eg::fiber_props >().setResumption( functor );
    boost::this_fiber::yield();
    checkIfStopped();
}

/*
template< typename Clock, typename Duration >
inline void sleep( std::chrono::time_point< Clock, Duration > const& sleep_time )
{
    checkIfStopped();
    boost::this_fiber::properties< eg::fiber_props >().setCycle( clock::cycle() + 1 );
    boost::this_fiber::sleep_until( sleep_time );
    checkIfStopped();
}

template< typename Rep, typename Period >
inline void sleep( std::chrono::duration< Rep, Period > const& timeout_duration )
{
    checkIfStopped();
    boost::this_fiber::properties< eg::fiber_props >().setCycle( clock::cycle() + 1 );
    boost::this_fiber::sleep_for( timeout_duration );
    checkIfStopped();
}*/

inline void wait()
{
    boost::this_fiber::yield();
}

}

#endif //EG_SCHEDULER