
#include <gtest/gtest.h>
#include <iostream>

#include "eg/coroutine.hpp"

eg::ActionCoroutine test( int iParam )
{
    int iResult = iParam * 2;
    
    std::cout << "b\n";
    
    co_yield eg::ReturnReason();
    
    std::cout << "d\n";
    
    co_return eg::ReturnReason();
}



TEST( Coroutines, Basic )
{
    
    eg::ActionCoroutine c = test( 123 );
    
    std::cout << "a\n";
    
    c.resume();
    
    std::cout << "c\n";
    
    c.resume();
    
    std::cout << "e\n";
    
    
}