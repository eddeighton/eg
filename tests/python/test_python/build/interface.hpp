#ifndef EG_INTERFACE_GUARD_INTERFACE
#define EG_INTERFACE_GUARD_INTERFACE
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////



#define EG_FIBER_STACK_SIZE ( 16384 )

struct [[clang::eg_type( -9 )]]Test;
struct [[clang::eg_type( -8 )]]bContinue;

//EG Interface
template< typename __eg1 >struct [[clang::eg_type(7)]]__eg_root
{
  __eg_root()
  {
    data.instance = 0;
    data.type = 0;
    data.timestamp = eg::INVALID_TIMESTAMP;
  }
  __eg_root( const eg::reference& reference )
  {
    data = reference;
  }
  template< typename TFrom >
  __eg_root( const TFrom& from );
  template< typename TFrom >
  __eg_root& operator=( const TFrom& from );
  template< typename TypePath, typename Operation, typename... Args >
  typename eg::result_type< __eg_root< __eg1 >, TypePath, Operation >::Type invoke( Args... args ) const;
  operator const void*() const
  {
        if( data.timestamp != eg::INVALID_TIMESTAMP )
        {
            return reinterpret_cast< const void* >( &data );
        }
        else
        {
            return nullptr;
        }
  }
  template< typename TComp >
  bool operator==( const TComp& cmp ) const
  {
      return data == cmp.data;
  }
  template< typename TComp >
  bool operator!=( const TComp& cmp ) const
  {
      return !(data == cmp.data);
  }
  template< typename TComp >
  bool operator<( const TComp& cmp ) const
  {
      return data < cmp.data;
  }
  void operator()() const;
  using Iterator = __eg_ReferenceIterator< __eg_root >;
  using EGRangeType = __eg_Range< Iterator >;
  eg::reference data;
  template< typename __eg2 >struct [[clang::eg_type(8)]]__eg_bContinue;
  template< typename __eg2 >struct [[clang::eg_type(9)]]__eg_Test
  {
    __eg_Test()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Test( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Test( const TFrom& from );
    template< typename TFrom >
    __eg_Test& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Test< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
    operator const void*() const
    {
          if( data.timestamp != eg::INVALID_TIMESTAMP )
          {
              return reinterpret_cast< const void* >( &data );
          }
          else
          {
              return nullptr;
          }
    }
    template< typename TComp >
    bool operator==( const TComp& cmp ) const
    {
        return data == cmp.data;
    }
    template< typename TComp >
    bool operator!=( const TComp& cmp ) const
    {
        return !(data == cmp.data);
    }
    template< typename TComp >
    bool operator<( const TComp& cmp ) const
    {
        return data < cmp.data;
    }
    void operator()() const;
    using Iterator = __eg_ReferenceIterator< __eg_Test >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
  };
  using Test = __eg_Test< __eg1 >;
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;
template struct __eg_root< void >::__eg_Test< void >;


//Explicit Trait Template Instantiations
template<>
template<>
struct __eg_root< void >::__eg_bContinue< void >
{
  using Type  = int;
  using Read  = eg::DimensionTraits< int >::Read;
  using Write = eg::DimensionTraits< int >::Write;
  using Get   = eg::DimensionTraits< int >::Get;
  static const eg::Instance Size = eg::DimensionTraits< int >::Size;
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
