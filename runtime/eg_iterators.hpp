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


#ifndef EG_ITERATORS
#define EG_ITERATORS

#include "eg_common.hpp"

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

    template< class ReferenceType >
    class ReferenceIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
    {
    public:
        using value_type = ReferenceType;
        Instance instance;
        Instance sentinal;
        TypeID type;
        inline ReferenceIterator( Instance instance, Instance sentinal, TypeID type ) : instance( instance ), sentinal( sentinal ), type( type ) {}
        inline ReferenceIterator( const ReferenceIterator& from ) : instance( from.instance ), sentinal( from.sentinal ), type( from.type ) {}
        ReferenceIterator& operator++();
        inline ReferenceIterator operator++(int) {ReferenceIterator tmp(*this); operator++(); return tmp;}
        inline bool operator==(const ReferenceIterator& rhs) const {return (instance==rhs.instance) && (type==rhs.type);}
        inline bool operator!=(const ReferenceIterator& rhs) const {return !(rhs==*this);}
        const value_type operator*();
    };
    
    template< class ReferenceType, int SIZE >
    class MultiIterator : public std::iterator< std::forward_iterator_tag, ReferenceType >
    {
        using IteratorType = ReferenceIterator< ReferenceType >;
        using IteratorArray = std::array< IteratorType, SIZE >;
        IteratorArray iterators;
        std::size_t szIndex = 0U;
    public:
        using value_type = ReferenceType;
    
        template< typename... Iterators >
        inline MultiIterator( Iterators... iterators ) 
            :   iterators( iterators... )
        {
            
        }
        
        inline MultiIterator& operator++()
        {
            while( szIndex != SIZE )
            {
                if( iterators[ szIndex ].instance == iterators[ szIndex ].sentinal )
                    ++szIndex;
                else
                {
                    ++iterators[ szIndex ];
                    if( iterators[ szIndex ].instance != iterators[ szIndex ].sentinal )
                        break;
                }
            }
            return *this;
        }
        
        inline MultiIterator operator++(int) 
        {
            MultiIterator tmp(*this); 
            operator++(); 
            return tmp;
        }
        
        inline bool operator==(const MultiIterator& rhs) const {return (szIndex==rhs.szIndex) && (iterators[ szIndex ]==rhs.iterators[ szIndex ]);}
        inline bool operator!=(const MultiIterator& rhs) const {return !(rhs==*this);}
        
        const ReferenceType operator*()
        {
            return *iterators[ szIndex ];
        }
    };

    template< typename Iterator >
    struct Range
    {
        using iterator_type = Iterator;
        Iterator _begin, _end;
        Range( Iterator _begin, Iterator _end ) : _begin( _begin ), _end( _end ) {}
        Iterator begin() const { return _begin; }
        Iterator end() const { return _end; }
    }; 
    
    template< typename Result, typename Range >
    inline Result one( const Range& range )
    {
        for( typename Range::iterator_type 
            i = range.begin(), iEnd = range.end();
            i!=iEnd;  )
        {
            return *i;
        }
        return Result();
    }
    
               

} //namespace eg



#endif //EG_ITERATORS