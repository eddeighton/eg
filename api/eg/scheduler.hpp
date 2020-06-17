
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
#include "event.hpp"

#include <chrono>
#include <optional>
#include <vector>
#include <functional>
#include <string>

using namespace std::chrono_literals;
using namespace std::string_literals;

eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance );
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance );
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance );

namespace eg
{
    enum Resumption
    {
        eResumption_Start,
        eResumption_Wait,
        eResumption_Sleep,
        eResumption_Wait_Event,
        eResumption_Sleep_Event,
        eResumption_Sleep_Timeout
    };
    
    struct ResumeReason
    {
        Resumption resumption;
        
        ResumeReason()
            :   resumption( eResumption_Start )
        {
            
        }
        
        ResumeReason( Resumption _resumption )
            :   resumption( _resumption )
        {
            
        }
        
    };
    
    enum Reason
    {
        eReason_Wait,
        eReason_Wait_All,
        eReason_Wait_Any,
        eReason_Sleep,
        eReason_Sleep_All,
        eReason_Sleep_Any,
        eReason_Timeout,
        eReason_Terminated
    };
    
    struct ReturnReason
    {
        Reason reason;
        std::vector< Event > events;
        std::optional< std::chrono::steady_clock::time_point > timeout;
        
        ReturnReason()
            :   reason( eReason_Terminated )
        {
            
        }
        
        ReturnReason( Reason _reason )
            :   reason( _reason )
        {
            
        }
        
        ReturnReason( Reason _reason, const Event& event )
            :   reason( _reason ),
                events( 1, event )
        {
        }
        
        ReturnReason( Reason _reason, std::initializer_list< Event > _events )
            :   reason( _reason ),
                events( _events )
        {
            
        }
        
        ReturnReason( const std::chrono::steady_clock::time_point& _timeout )
            :   reason( eReason_Timeout ),
                timeout( _timeout )
        {
            
        }
        
    };
    
    
    ////////////////////////////////////////////////////////////////////////////
    //wait functions
    inline ReturnReason wait()
    {
        return ReturnReason( eReason_Wait );
    }
    
    inline ReturnReason wait( const Event& event )
    {
        return ReturnReason( eReason_Wait_All, event.data );
    }
    
    inline ReturnReason wait_all( std::initializer_list< Event > events )
    {
        return ReturnReason( eReason_Wait_All, events );
    }
    
    inline ReturnReason wait_any( std::initializer_list< Event > events )
    {
        return ReturnReason( eReason_Wait_Any, events );
    }
    
    ////////////////////////////////////////////////////////////////////////////
    //sleep functions
    inline ReturnReason sleep()
    {
        return ReturnReason( eReason_Sleep );
    }
    
    inline ReturnReason sleep( const Event& event )
    {
        return ReturnReason( eReason_Sleep_All, event );
    }
    
    inline ReturnReason sleep_all( std::initializer_list< Event > events )
    {
        return ReturnReason( eReason_Sleep_All, events );
    }
    
    inline ReturnReason sleep_any( std::initializer_list< Event > events )
    {
        return ReturnReason( eReason_Sleep_Any, events );
    }
    
    template< typename Clock, typename Duration >
    inline ReturnReason sleep( std::chrono::time_point< Clock, Duration > const& sleep_time )
    {
        return ReturnReason( sleep_time );
    }
    
    template< typename Rep, typename Period >
    inline ReturnReason sleep( std::chrono::duration< Rep, Period > const& timeout_duration )
    {
        return ReturnReason( std::chrono::steady_clock::now() + timeout_duration );
    }
    
    inline ReturnReason sleep( float fDuration )
    {
        auto floatDuration      = std::chrono::duration< float, std::ratio< 1 > >( fDuration );
        auto intMilliseconds    = std::chrono::duration_cast< std::chrono::milliseconds >( floatDuration );
        return sleep( intMilliseconds );
    }

    inline ReturnReason sleep( double dbDuration )
    {
        auto doubleDuration      = std::chrono::duration< double, std::ratio< 1 > >( dbDuration );
        auto intMilliseconds    = std::chrono::duration_cast< std::chrono::milliseconds >( doubleDuration );
        return sleep( intMilliseconds );
    }
        
    ////////////////////////////////////////////////////////////////////////////
    //the scheduler interface
    class Scheduler
    {
    public:
		typedef void (*StopperFunctionPtr)( eg::Instance );
        using ActionOperator = std::function< ReturnReason( ResumeReason ) >;
        
        //static void start_ref( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action );
        static void allocated_ref( const reference& ref, StopperFunctionPtr pStopper );
        static void call_ref( const reference& ref, StopperFunctionPtr pStopper, ActionOperator action );
        static void signal_ref( const reference& ref, StopperFunctionPtr pStopper );
        static void stop_ref( const reference& ref );
        static void pause_ref( const reference& ref );
        static void unpause_ref( const reference& ref );
        
        /*template< typename T, typename... Args >
        static void start( const T& staticRef, StopperFunctionPtr pStopper, Args... args )
        {
            using namespace std::placeholders;
            start_ref( staticRef.data, pStopper, std::bind( &T::operator(), staticRef, _1, args... ) );
        }*/
        template< typename T, typename... Args >
        static void call( const T& staticRef, StopperFunctionPtr pStopper, Args... args )
        {
            using namespace std::placeholders;
            call_ref( staticRef.data, pStopper, std::bind( &T::operator(), staticRef, _1, args... ) );
        }
        /*template< typename T >
        static void stop( const T& staticRef )
        {
            stop_ref( staticRef.data );
        }
        template< typename T >
        static void pause( const T& staticRef )
        {
            pause_ref( staticRef.data );
        }
        template< typename T >
        static void unpause( const T& staticRef )
        {
            using namespace std::placeholders;
            unpause_ref( staticRef.data );
        }*/
        
        //object lifetime
        static void zeroRefCount( const reference& ref, eg::Instance* pRefCount );
        
        //are there any active actions
        static bool active();
    
        //run a cycle
        static void cycle();
    };
    
    

}

#endif //EG_SCHEDULER