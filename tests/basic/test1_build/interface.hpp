#ifndef EG_INTERFACE_GUARD_D9B75849_E9C1_44CF_BAB1_06D9DEBB1D9A__2019_Apr_19_18_06_44
#define EG_INTERFACE_GUARD_D9B75849_E9C1_44CF_BAB1_06D9DEBB1D9A__2019_Apr_19_18_06_44
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


//Forward declarations

//EG Interface
template< typename __EGInterface1 >struct [[clang::eg_type(4)]]__interface_root
{
  __interface_root()
  {
    data.instance = 0;
    data.type = __interface_root::ID;
    data.timestamp = EG_INVALID_TIMESTAMP;
  }
  __interface_root( EGInstance instance, EGTimeStamp timestamp )
  {
    data.instance = instance;
    data.type = __interface_root::ID;
    data.timestamp = timestamp;
  }
  template< typename TFrom >
  __interface_root( const TFrom& from );
  template< typename TFrom >
  __interface_root& operator=( const TFrom& from );
  template< typename TypePath, typename Operation, typename... Args >
  typename result_type< __interface_root< __EGInterface1 >, TypePath, Operation >::Type invoke( Args... args );
  EGEvent get_next_event() const;
  operator const void*() const;
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
  EGCoroutine operator()();
  class Iterator : public std::iterator< std::forward_iterator_tag, __interface_root >
  {
  public:
    EGInstance instance;
    EGInstance sentinal;
    Iterator( EGInstance instance, EGInstance sentinal ) : instance( instance ), sentinal( sentinal ) {}
    Iterator( const Iterator& from ) : instance( from.instance ), sentinal( from.sentinal ) {}
    Iterator& operator++();
    Iterator operator++(int) {Iterator tmp(*this); operator++(); return tmp;}
    bool operator==(const Iterator& rhs) const {return instance==rhs.instance;}
    bool operator!=(const Iterator& rhs) const {return instance!=rhs.instance;}
    __interface_root& operator*();
  };
  using EGRangeType = EGRange< Iterator >;
  __eg_reference data;
  static const EGInstance ID = 4;
};
using root = __interface_root< void >;


//Explicit Template Instantiations
template struct __interface_root< void >;


//Explicit Trait Template Instantiations


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
