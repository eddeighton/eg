
#include <gtest/gtest.h>

#include <memory>
#include <memory_resource>

struct Stuff
{
    Stuff( int _x, int _y )
        : x( _x ), y( _y )
    {
    }
    
    operator int();
        
    int x,y;
};

Stuff::operator int()
{
    return 4;
}

TEST( Allocators, Basic )
{
    
    std::pmr::polymorphic_allocator< Stuff > alloc( std::pmr::new_delete_resource() );
    
    
    //Stuff* pStuff = alloc.new_object< Stuff >( 123, 456 );
    
    Stuff* pStuff = alloc.allocate( 1U );
    alloc.construct( pStuff, 123, 456 );
    
    ASSERT_EQ( pStuff->x, 123 );
    ASSERT_EQ( pStuff->y, 456 );
    
    const int i = *pStuff;
    
    ASSERT_EQ( i, 4 );
}
