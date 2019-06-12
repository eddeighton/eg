#ifndef EG_INTERFACE_GUARD_420E5933_E3D0_4520_8491_A10611092685__2019_Jun_12_13_03_24
#define EG_INTERFACE_GUARD_420E5933_E3D0_4520_8491_A10611092685__2019_Jun_12_13_03_24
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//EG Interface
template< typename __eg1 >struct [[clang::eg_type(3)]]__eg_root
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
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;


//Explicit Trait Template Instantiations


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
