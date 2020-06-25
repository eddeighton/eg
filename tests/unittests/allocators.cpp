
#include <gtest/gtest.h>

#include <intrin.h>
#include <memory>
#include <memory_resource>
#include <algorithm>
#include <bitset>
#include <random>

#include "eg/allocator.hpp"

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

TEST( Allocators, BitScan )
{
    unsigned long mask = 0x1000;
    unsigned long index;
    unsigned char isNonzero;

    isNonzero = _BitScanForward( &index, mask );
    
    ASSERT_TRUE( isNonzero );
    ASSERT_EQ( index, 12 );
}


template< typename AllocatorType >
void testAllocator()
{
    std::random_device rd;
    std::mt19937 g( rd() );
    
    AllocatorType allocator;
    
    std::set< eg::Instance > allocated;
    
    for( int k = 0; k < 10; ++k )
    {
        for( int j = 0; j < 10; ++j )
        {
            //insert some
            {
                for( int i = 0; i < AllocatorType::Size / 2; ++i )
                {
                    eg::Instance instance = allocator.nextFree();
                    if( instance == AllocatorType::Size )
                        break;
                    allocator.allocate( instance );
                    ASSERT_TRUE( allocated.insert( instance ).second );
                }
            }
            
            //remove some
            {
                std::vector< int > allocs( allocated.begin(), allocated.end() );
                std::shuffle( allocs.begin(), allocs.end(), g );
                allocs.resize( allocs.size() / 2 );
                for( eg::Instance i : allocs )
                {
                    allocator.free( i );
                    ASSERT_EQ( 1, allocated.erase( i ) );
                }
            }
        }
    
        //remove all
        {
            std::vector< int > allocs( allocated.begin(), allocated.end() );
            std::shuffle( allocs.begin(), allocs.end(), g );
            for( eg::Instance i : allocs )
            {
                allocator.free( i );
                ASSERT_EQ( 1, allocated.erase( i ) );
            }
        }
        
        ASSERT_TRUE( allocated.empty() );
        ASSERT_TRUE( allocator.empty() );
    }
    
    //test can insert to all
    {
        for( int i = 0; i < AllocatorType::Size; ++i )
        {
            eg::Instance instance = allocator.nextFree();
            ASSERT_TRUE( instance != AllocatorType::Size );
            allocator.allocate( instance );
            ASSERT_TRUE( allocated.insert( instance ).second );
        }
        ASSERT_EQ( allocated.size(), AllocatorType::Size );
    }
}


template< typename AllocatorType >
void benchAllocator()
{
    AllocatorType allocator;
    for( int i = 0; i < AllocatorType::Size; ++i )
    {
        allocator.allocate( allocator.nextFree() );
    }
    for( int i = 0; i < AllocatorType::Size; ++i )
    {
        allocator.free( i );
    }
    ASSERT_TRUE( allocator.empty() );
}

TEST( Allocators, Bitmask8Allocator )
{
    using AllocatorType = eg::Bitmask32Allocator< 8 >;
    testAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask16Allocator )
{
    using AllocatorType = eg::Bitmask32Allocator< 16 >;
    testAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask25Allocator )
{
    using AllocatorType = eg::Bitmask32Allocator< 25 >;
    testAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask32Allocator )
{
    using AllocatorType = eg::Bitmask32Allocator< 32 >;
    testAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask48Allocator )
{
    using AllocatorType = eg::Bitmask64Allocator< 48 >;
    testAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask64Allocator )
{
    using AllocatorType = eg::Bitmask64Allocator< 64 >;
    testAllocator< AllocatorType >();
}
TEST( Allocators, Ring128Allocator )
{
    using RingAlloc = eg::RingAllocator< 128 >;
    testAllocator< RingAlloc >();
}
TEST( Allocators, Ring1024Allocator )
{
    using RingAlloc = eg::RingAllocator< 1024 >;
    testAllocator< RingAlloc >();
}
TEST( Allocators, Ring2048Allocator )
{
    using RingAlloc = eg::RingAllocator< 2048 >;
    testAllocator< RingAlloc >();
}

TEST( Allocators, Bitmask8AllocatorBench )
{
    using AllocatorType = eg::Bitmask32Allocator< 8 >;
    benchAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask16AllocatorBench )
{
    using AllocatorType = eg::Bitmask32Allocator< 16 >;
    benchAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask25AllocatorBench )
{
    using AllocatorType = eg::Bitmask32Allocator< 25 >;
    benchAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask32AllocatorBench )
{
    using AllocatorType = eg::Bitmask32Allocator< 32 >;
    benchAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask48AllocatorBench )
{
    using AllocatorType = eg::Bitmask64Allocator< 48 >;
    benchAllocator< AllocatorType >();
}
TEST( Allocators, Bitmask64AllocatorBench )
{
    using AllocatorType = eg::Bitmask64Allocator< 64 >;
    benchAllocator< AllocatorType >();
}
TEST( Allocators, Ring128AllocatorBench )
{
    using RingAlloc = eg::RingAllocator< 128 >;
    benchAllocator< RingAlloc >();
}
TEST( Allocators, Ring1024AllocatorBench )
{
    using RingAlloc = eg::RingAllocator< 1024 >;
    benchAllocator< RingAlloc >();
}
TEST( Allocators, Ring2048AllocatorBench )
{
    using RingAlloc = eg::RingAllocator< 2048 >;
    benchAllocator< RingAlloc >();
}
TEST( Allocators, Ring65000AllocatorBench )
{
    using RingAlloc = eg::RingAllocator< 65000 >;
    benchAllocator< RingAlloc >();
}