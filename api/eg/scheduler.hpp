
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

//#include <chrono>
#include <functional>

//using namespace std::chrono_literals;

eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance );
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance );
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance );

namespace eg
{
    
    class ResumeReason
    {
    public:
        enum Type
        {
            eStart,
            eWait,
            eSleep
        };
        
        Type m_type;
    };
    
    class ReturnReason
    {
    public:
        enum Type
        {
            eComplete,
            eWait,
            eSleep
        };
        
        Type m_type;
    };
    
    class Scheduler
    {
    public:
        using ActionOperator = std::function< ReturnReason( ResumeReason ) >;
        
        static void start_ref( const reference& ref, ActionOperator action );
        static void call_ref( const reference& ref, ActionOperator action );
        static void stop_ref( const reference& ref );
        static void pause_ref( const reference& ref );
        static void unpause_ref( const reference& ref );
        
        template< typename T, typename... Args >
        static void start( const T& staticRef, Args... args )
        {
            using namespace std::placeholders;
            start_ref( staticRef.data, std::bind( &T::operator(), staticRef, args..., _1 ) );
        }
        template< typename T, typename... Args >
        static void call( const T& staticRef, Args... args )
        {
            using namespace std::placeholders;
            call_ref( staticRef.data, std::bind( &T::operator(), staticRef, args..., _1 ) );
        }
        template< typename T >
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
        }
        
        //are there any active actions
        static bool active();
    
        //run a cycle
        static void cycle();
    };
    
    

}

#endif //EG_SCHEDULER