#ifndef EG_INTERFACE_GUARD_91D3CC09_B57C_43C3_B14C_B31B923DFFA4__2019_Jun_13_02_27_23
#define EG_INTERFACE_GUARD_91D3CC09_B57C_43C3_B14C_B31B923DFFA4__2019_Jun_13_02_27_23
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct [[clang::eg_type( -17 )]]Square;
struct [[clang::eg_type( -19 )]]imgui;
struct [[clang::eg_type( -15 )]]size;

//EG Interface
template< typename __eg1 >struct [[clang::eg_type(14)]]__eg_root
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
  template< typename __eg2 >struct [[clang::eg_type(15)]]__eg_size;
  template< typename __eg2 >struct [[clang::eg_type(17)]]__eg_Square
  {
    __eg_Square()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Square( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Square( const TFrom& from );
    template< typename TFrom >
    __eg_Square& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Square< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Square >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
  };
  using Square = __eg_Square< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(19)]]__eg_imgui
  {
    __eg_imgui()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_imgui( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_imgui( const TFrom& from );
    template< typename TFrom >
    __eg_imgui& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_imgui< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    void operator()(std::string strName) const;
    using Iterator = __eg_ReferenceIterator< __eg_imgui >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    static const eg::Instance SIZE = 4;
  };
  using imgui = __eg_imgui< __eg1 >;
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;
template struct __eg_root< void >::__eg_Square< void >;
template struct __eg_root< void >::__eg_imgui< void >;


//Explicit Trait Template Instantiations
template<>
template<>
struct __eg_root< void >::__eg_size< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
