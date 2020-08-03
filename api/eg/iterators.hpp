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


#ifndef EG_ITERATORS_02_05_2019
#define EG_ITERATORS_02_05_2019

#include "common.hpp"
#include "clock.hpp"

#include <array>

namespace eg
{

    union RingBufferAllocator
    {
        std::uint64_t data;
        struct
        {
            std::uint32_t head : 31;
            std::uint32_t full : 1;
            std::uint32_t tail : 31;
            std::uint32_t protection : 1;
        };
     
        RingBufferAllocator()
            :   data( 0UL ) 
        {
        }
        RingBufferAllocator( std::uint64_t value ) 
            :   data( value ) 
        { 
        }
    };
}

template< typename ReferenceType >
inline eg::TimeStamp getTimestamp( eg::TypeID type, eg::Instance instance );

template< typename ReferenceType >
inline eg::ActionState getState( eg::TypeID type, eg::Instance instance );

template< typename ReferenceType >
inline eg::TimeStamp getStopCycle( eg::TypeID type, eg::Instance instance );

template< typename ReferenceType >
inline bool isActionActive( eg::TypeID type, eg::Instance instance )
{
    switch( getState< ReferenceType >( type, instance ) )
    {
        case eg::action_running : return true;
        case eg::action_paused  : return true;
        case eg::action_stopped :
            if( getStopCycle< ReferenceType >( type, instance ) == clock::cycle( type ) )
                return true;
            else
                return false;
        case eg::TOTAL_ACTION_STATES  : return false;
    }
}

template< class ReferenceType >
class __eg_ReferenceRawIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
{
public:
    using value_type = ReferenceType;
    eg::Instance instance;
    eg::Instance sentinal;
    eg::TypeID type;
    inline __eg_ReferenceRawIterator( eg::Instance instance, eg::Instance sentinal, eg::TypeID type ) : instance( instance ), sentinal( sentinal ), type( type ) {}
    inline __eg_ReferenceRawIterator( const __eg_ReferenceRawIterator& from ) : instance( from.instance ), sentinal( from.sentinal ), type( from.type ) {}
    inline __eg_ReferenceRawIterator& operator++()
    {
        ++instance;
        return *this;
    }
    inline __eg_ReferenceRawIterator operator++(int) {__eg_ReferenceRawIterator tmp(*this); operator++(); return tmp;}
    inline bool operator==(const __eg_ReferenceRawIterator& rhs) const {return (instance==rhs.instance) && (type==rhs.type);}
    inline bool operator!=(const __eg_ReferenceRawIterator& rhs) const {return !(rhs==*this);}
    inline const value_type operator*() const
    {
        return eg::reference{ instance, type, getTimestamp< ReferenceType >( type, instance ) };
    }
};

template< class ReferenceType >
class __eg_ReferenceIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
{
public:
    using value_type = ReferenceType;
    eg::Instance instance;
    eg::Instance sentinal;
    eg::TypeID type;
    inline __eg_ReferenceIterator( eg::Instance instance, eg::Instance sentinal, eg::TypeID type ) : instance( instance ), sentinal( sentinal ), type( type ) {}
    inline __eg_ReferenceIterator( const __eg_ReferenceIterator& from ) : instance( from.instance ), sentinal( from.sentinal ), type( from.type ) {}
    inline __eg_ReferenceIterator& operator++()
    {
        while( true )
        {
            ++instance;
            if( ( instance == sentinal ) || ( isActionActive< ReferenceType >( type, instance ) ) )
                break;
        }
        return *this;
    }
    inline __eg_ReferenceIterator operator++(int) {__eg_ReferenceIterator tmp(*this); operator++(); return tmp;}
    inline bool operator==(const __eg_ReferenceIterator& rhs) const {return (instance==rhs.instance) && (type==rhs.type);}
    inline bool operator!=(const __eg_ReferenceIterator& rhs) const {return !(rhs==*this);}
    inline const value_type operator*() const
    {
        return eg::reference{ instance, type, getTimestamp< ReferenceType >( type, instance ) };
    }
};

template< class IteratorType, eg::Instance SIZE >
class __eg_MultiIterator : public std::iterator< std::forward_iterator_tag, typename IteratorType::value_type >
{
public:
    using IteratorArray = std::array< IteratorType, SIZE >;
    using value_type = typename IteratorType::value_type;
private:
    IteratorArray iterators;
    eg::Instance szIndex = 0U;
public:

    inline __eg_MultiIterator()
        :   iterators( { ReferenceType{ 0, 0, 0 }, ReferenceType{ 0, 0, 0 } } ),
            szIndex( SIZE )
    {
    }
    
    inline __eg_MultiIterator( const __eg_MultiIterator& cpy )
        :   iterators( cpy.iterators ),
            szIndex( cpy.szIndex )
    {
        
    }
    
    inline __eg_MultiIterator( const IteratorArray& iters ) 
        :   iterators( iters ),
            szIndex( 0U )
    {
        while( szIndex != SIZE )
        {
            if( iterators[ szIndex ].instance == iterators[ szIndex ].sentinal )
                ++szIndex;
            else
                break;
        }
    }
    
    inline __eg_MultiIterator& operator++()
    {
        if( iterators[ szIndex ].instance != iterators[ szIndex ].sentinal )
            ++iterators[ szIndex ];
        if( iterators[ szIndex ].instance == iterators[ szIndex ].sentinal )
        {
            while( ( szIndex != SIZE ) && ( iterators[ szIndex ].instance == iterators[ szIndex ].sentinal ) )
                ++szIndex;
        }
        return *this;
    }
    
    inline __eg_MultiIterator operator++(int) 
    {
        __eg_MultiIterator tmp(*this); 
        operator++(); 
        return tmp;
    }
    
    inline bool operator==(const __eg_MultiIterator& rhs) const 
    {
        return (szIndex==rhs.szIndex) && 
            ( ( szIndex >= SIZE) || ( iterators[ szIndex ] == rhs.iterators[ szIndex ] ) );
    }
    inline bool operator!=(const __eg_MultiIterator& rhs) const {return !(rhs==*this);}
    
    inline const value_type operator*() const
    {
        return *iterators[ szIndex ];
    }
};

template< typename Iterator >
struct __eg_Range
{
    using iterator_type = Iterator;
    Iterator _begin, _end;
    __eg_Range( Iterator _begin, Iterator _end ) : _begin( _begin ), _end( _end ) {}
    Iterator begin() const { return _begin; }
    Iterator end() const { return _end; }
}; 
    
namespace eg
{
    template< typename Result, typename RangeType >
    inline Result one( const RangeType& range )
    {
        for( typename RangeType::iterator_type 
            i = range.begin(), iEnd = range.end();
            i!=iEnd;  )
        {
            return *i;
        }
        return Result();
    }
    
    template< typename RangeType >
    inline std::size_t count( const RangeType& range )
    {
        return std::distance( range.begin(), range.end() );
    }
               

} //namespace eg



#endif //EG_ITERATORS_02_05_2019