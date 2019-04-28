
#ifndef EG_ITERATORS
#define EG_ITERATORS

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
        Instance instance;
        Instance sentinal;
        inline ReferenceIterator( Instance instance, Instance sentinal ) : instance( instance ), sentinal( sentinal ) {}
        inline ReferenceIterator( const ReferenceIterator& from ) : instance( from.instance ), sentinal( from.sentinal ) {}
        ReferenceIterator& operator++();
        inline ReferenceIterator operator++(int) {ReferenceIterator tmp(*this); operator++(); return tmp;}
        inline bool operator==(const ReferenceIterator& rhs) const {return instance==rhs.instance;}
        inline bool operator!=(const ReferenceIterator& rhs) const {return instance!=rhs.instance;}
        const ReferenceType& operator*();
    };

    template< typename Iterator >
    struct Range
    {
        Iterator _begin, _end;
        Range( Iterator _begin, Iterator _end ) : _begin( _begin ), _end( _end ) {}
        Iterator begin() const { return _begin; }
        Iterator end() const { return _end; }
    }; 
               

} //namespace eg



#endif //EG_ITERATORS