
#include <iostream>

#include "w:/root/eg/install/include/eg/common.hpp"
#include "w:/root/eg/install/include/eg/result_type.hpp"
#include "w:/root/eg/install/include/eg/coroutine.hpp"

struct [[clang::eg_type( -32 )]]Vehicle;
struct [[clang::eg_type( -29 )]]foo2;
struct [[clang::eg_type( -27 )]]foobar;
struct [[clang::eg_type( -33 )]]getPosition;
struct [[clang::eg_type( -35 )]]getSpeed;
struct [[clang::eg_type( -39 )]]getVehicle;
struct [[clang::eg_type( -37 )]]m_position;
struct [[clang::eg_type( -38 )]]m_speed;

template< typename __eg1 >struct [[clang::eg_type(26)]]__eg_root
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
  static __eg_root null() { return __eg_root(); }
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
  operator ::eg::ReferenceState() const;
  bool operator==( ::eg::ReferenceState cmp ) const;
  bool operator!=( ::eg::ReferenceState cmp ) const;
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
  
  eg::reference data;
  
  template< typename __eg2 >struct [[clang::eg_type(39)]]__eg_getVehicle;
};
using root = __eg_root< void >;


template< typename __eg1 >
template< typename __eg2 >
struct __eg_root< __eg1 >::__eg_getVehicle
{
  __eg_getVehicle()
  {
    data.instance = 0;
    data.type = 0;
    data.timestamp = eg::INVALID_TIMESTAMP;
  }
  __eg_getVehicle( const eg::reference& reference )
  {
    data = reference;
  }
  template< typename TFrom >
  __eg_getVehicle( const TFrom& from );
  template< typename TFrom >
  __eg_getVehicle& operator=( const TFrom& from );
  static __eg_getVehicle null() { return __eg_getVehicle(); }
  template< typename TypePath, typename Operation, typename... Args >
  typename eg::result_type< __eg_getVehicle< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
  operator ::eg::ReferenceState() const;
  bool operator==( ::eg::ReferenceState cmp ) const;
  bool operator!=( ::eg::ReferenceState cmp ) const;
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
  
  eg::reference data;
  template< typename __eg2 >struct [[clang::eg_type(23)]]__eg_Base_0;

};

template< typename __eg1 >
template< typename TypePath, typename Operation, typename... Args >
inline typename eg::result_type< __eg_root< __eg1 >, TypePath, Operation >::Type
__eg_root< __eg1 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >, TypePath, Operation >::Type, __eg_root< __eg1 >, CanonicalTypePathType, Operation >()( *this, args... );
}


template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
inline typename eg::result_type< typename __eg_root< __eg1 >::template __eg_getVehicle< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_getVehicle< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_getVehicle< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_getVehicle< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}




template<>
void __eg_root< void >::operator()() const
{
    std::cout << "root called" << std::endl;
}

template<>
template<>
void __eg_root< void >::__eg_getVehicle< void >::operator()() const
{
    std::cout << "__eg_getVehicle called" << std::endl;
}

int main()
{
    root r;
    
    r();
    
    root::__eg_getVehicle< void > g;
    g();
    
    return 0;
}

