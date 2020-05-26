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

namespace
{
    class BasicScheduler
    {
        struct ActiveAction
        {
            eg::reference ref;
            eg::Scheduler::ActionOperator op;
        };
        
        using ActiveActionList = std::list< ActiveAction >;
        
    public:
        void start( const eg::reference& ref, eg::Scheduler::ActionOperator action )
        {
            m_ready.push_back( ActiveAction{ ref, action } );
        }
        
        void call( const eg::reference& ref, eg::Scheduler::ActionOperator action )
        {
            m_ready.push_back( ActiveAction{ ref, action } );
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
            return !m_ready.empty() || !m_waiting.empty() || !m_sleeping.empty();
        }

        //run a cycle
        void cycle()
        {
            using namespace eg;
            
            if( m_ready.empty() && m_waiting.empty() )
            {
                m_ready.swap( m_sleeping );
            }
            
            while( !m_ready.empty() || !m_waiting.empty() )
            {
                if( m_ready.empty() )
                {
                    m_ready.swap( m_waiting );
                }
                
                ActiveAction action = m_ready.front();
                m_ready.pop_front();
                
                const ReturnReason returnReason =
                    action.op( ResumeReason{ ResumeReason::eStart } );
                
                switch( returnReason.m_type )
                {
                    case ReturnReason::eComplete :
                        break;
                    case ReturnReason::eWait     :
                        m_waiting.push_back( action );
                        break;
                    case ReturnReason::eSleep    :
                        m_sleeping.push_back( action );
                        break;
                }
            }
        }
        
    private:
        ActiveActionList m_ready, m_waiting, m_sleeping;
    };


    BasicScheduler theScheduler;
}

namespace eg
{

    void Scheduler::start_ref( const reference& ref, ActionOperator action )
    {
        theScheduler.start( ref, action );
    }
    
    void Scheduler::call_ref( const reference& ref, ActionOperator action )
    {
        theScheduler.call( ref, action );
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
        