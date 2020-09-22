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

#ifndef EG_ALLOCATORS_GUARD_20_JULY_2020
#define EG_ALLOCATORS_GUARD_20_JULY_2020

#include <array>
#include <bitset>
#include <cstddef>
#include <intrin.h>

#include "common.hpp"
#include "clock.hpp"

namespace eg
{
    
template< typename T >
struct DimensionTraits;
    
template< std::size_t _Size >
class Bitmask32Allocator
{
    friend struct eg::DimensionTraits< eg::Bitmask32Allocator< _Size > >;
public:
    static const std::size_t Size = _Size;
    
    inline Bitmask32Allocator()
    {
        static_assert( Size <= ( sizeof( std::uint32_t ) * 8 ) );        
        m_bitset.set();
    }
    
    inline Instance nextFree() const
    {
        unsigned long index;
        if( _BitScanForward( &index, m_bitset.to_ulong() ) )
        {
            return index;
        }
        else
        {
            return Size;
        }
    }

    inline void allocate( Instance instance )
    {
        m_bitset.reset( instance );
    }
    
    inline void free( Instance instance )
    {
        m_bitset.set( instance );
    }
    
    inline bool empty() const
    {
        return m_bitset.all();
    }
    inline bool full() const
    {
        return m_bitset.none();
    }
private:
    std::bitset< Size > m_bitset;
};

template< std::size_t _Size >
class Bitmask64Allocator
{
    friend struct eg::DimensionTraits< eg::Bitmask64Allocator< _Size > >;
public:
    static const std::size_t Size = _Size;
    
    inline Bitmask64Allocator()
    {
        static_assert( Size >   sizeof( std::uint32_t ) * 8 );
        static_assert( Size <=  sizeof( std::uint64_t ) * 8 );
        m_bitset.set();
    }
    
    inline Instance nextFree() const
    {
        const std::uint64_t mask = m_bitset.to_ullong();
        
        unsigned long index;
        if( _BitScanForward( &index, mask ) )
        {
            return index;
        }
        else
        {
            if( _BitScanForward( &index, mask >> 32 ) )
            {
                return index + 32U;
            }
            else
            {
                return Size;
            }
        }
    }

    inline void allocate( Instance instance )
    {
        m_bitset.reset( instance );
    }
    
    inline void free( Instance instance )
    {
        m_bitset.set( instance );
    }
    
    inline bool empty() const
    {
        return m_bitset.all();
    }
    inline bool full() const
    {
        return m_bitset.none();
    }
private:
    std::bitset< Size > m_bitset;
};
    

template< std::size_t _Size >
class RingAllocator
{
    friend struct eg::DimensionTraits< eg::RingAllocator< _Size > >;
public:
    static const std::size_t Size = _Size;
    
    inline RingAllocator()
    {
        static_assert( Size > sizeof( std::int64_t ) * 8 );
        m_head = 0U;
        m_tail = 0U;
        m_full = 0U;
        for( std::size_t sz = 0; sz != Size; ++sz )
        {
            m_ring[ sz ] = sz;
        }
    }
    
    inline Instance nextFree() const
    {
        if( !m_full )
        {
            std::size_t szHead = m_head;
            if( szHead == Size - 1 )
                szHead = 0U;
            else
                ++szHead;
            
            return m_ring[ szHead ];
        }
        return Size;
    }

    inline void allocate( Instance instance )
    {
        if( m_head == Size - 1 )
            m_head = 0U;
        else
            ++m_head;
        if( m_head == m_tail )
            m_full = true;
    }
    
    inline void free( Instance instance )
    {
        if( m_full )
            m_full = false;
        
        if( m_tail == Size - 1 )
            m_tail = 0U;
        else
            ++m_tail;
        
        m_ring[ m_tail ] = instance;
    }
    
    inline bool empty() const
    {
        return !m_full && ( m_head == m_tail );
    }
    inline bool full() const
    {
        return m_full;
    }
private:
    std::size_t m_head, m_tail;
    bool m_full;
    std::array< Instance, Size > m_ring;
    
};

}

#endif //EG_ALLOCATORS_GUARD_20_JULY_2020
