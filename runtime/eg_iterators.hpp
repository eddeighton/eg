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

#include "eg_common.hpp"
#include "eg_include.hpp" //clock

#include <array>

namespace eg
{

    union Iterator
    {
        std::uint64_t data;
        struct
        {
            std::uint32_t head : 31;
            std::uint32_t full : 1;
            std::uint32_t tail : 31;
            std::uint32_t protection : 1;
        };
     
        Iterator()
            :   data( 0UL ) 
        {
        }
        Iterator( std::uint64_t value ) 
            :   data( value ) 
        { 
        }
    };
}

template< typename ReferenceType >
inline eg::TimeStamp getTimestamp( eg::TypeID type, eg::Instance instance );

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
            if( ( instance == sentinal ) || ( getTimestamp< ReferenceType >( type, instance ) <= clock::subcycle() ) )
                break;
        }
        return *this;
    }
    inline __eg_ReferenceIterator operator++(int) {__eg_ReferenceIterator tmp(*this); operator++(); return tmp;}
    inline bool operator==(const __eg_ReferenceIterator& rhs) const {return (instance==rhs.instance) && (type==rhs.type);}
    inline bool operator!=(const __eg_ReferenceIterator& rhs) const {return !(rhs==*this);}
    inline const value_type operator*()
    {
        return ReferenceType( eg::reference{ instance, type, getTimestamp< ReferenceType >( type, instance ) } );
    }
};

template< class ReferenceType, std::size_t SIZE >
class __eg_MultiIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
{
public:
    using IteratorType = __eg_ReferenceIterator< ReferenceType >;
    using IteratorArray = std::array< IteratorType, SIZE >;
    using value_type = ReferenceType;
private:
    IteratorArray iterators;
    std::size_t szIndex = 0U;
public:

    inline __eg_MultiIterator()
        :   iterators( { ReferenceType{ 0, 0, 0 }, ReferenceType{ 0, 0, 0 } } ),
            szIndex( 2U )
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
    
    inline const ReferenceType operator*()
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
    template< typename Result, typename __eg_Range >
    inline Result one( const __eg_Range& range )
    {
        for( typename __eg_Range::iterator_type 
            i = range.begin(), iEnd = range.end();
            i!=iEnd;  )
        {
            return *i;
        }
        return Result();
    }
    
               

} //namespace eg



#endif //EG_ITERATORS_02_05_2019