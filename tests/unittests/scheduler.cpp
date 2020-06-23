
#include <gtest/gtest.h>
#include <iostream>

#include "eg/coroutine.hpp"
#include "eg/scheduler.hpp"

struct TestAction
{
    eg::ActionCoroutine operator()()
    {
        
        co_return eg::done();
    }
    
    eg::reference data;
};

void stopper( eg::Instance i )
{
    
}

TEST( Scheduler, Basic )
{
    TestAction test;
    
    //eg::Scheduler::call( test, &stopper );
    
    
    //eg::Scheduler::cycle();
    
    ASSERT_TRUE( !eg::Scheduler::active() );
}