#ifndef EG_INTERFACE_GUARD_2B43C6AB_38B4_46F1_A528_4452867B6DEC__2019_Jun_13_02_09_22
#define EG_INTERFACE_GUARD_2B43C6AB_38B4_46F1_A528_4452867B6DEC__2019_Jun_13_02_09_22
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct [[clang::eg_type( -46 )]]Circle;
struct [[clang::eg_type( -65 )]]ColourChanger;
struct [[clang::eg_type( -58 )]]Grow;
struct [[clang::eg_type( -61 )]]Move;
struct [[clang::eg_type( -55 )]]Shrink;
struct [[clang::eg_type( -48 )]]color;
struct [[clang::eg_type( -62 )]]direction;
struct [[clang::eg_type( -69 )]]lastPos;
struct [[clang::eg_type( -51 )]]lifetime;
struct [[clang::eg_type( -53 )]]max_lifetime;
struct [[clang::eg_type( -52 )]]max_size;
struct [[clang::eg_type( -66 )]]offset;
struct [[clang::eg_type( -47 )]]position;
struct [[clang::eg_type( -49 )]]size;
struct [[clang::eg_type( -63 )]]speed;
struct [[clang::eg_type( -56 )]]start;
struct [[clang::eg_type( -50 )]]starttime;

//EG Interface
template< typename __eg1 >struct [[clang::eg_type(43)]]__eg_root
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
  template< typename __eg2 >struct [[clang::eg_type(46)]]__eg_Circle
  {
    __eg_Circle()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Circle( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Circle( const TFrom& from );
    template< typename TFrom >
    __eg_Circle& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Circle< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Circle >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    static const eg::Instance SIZE = 4096;
    template< typename __eg3 >struct [[clang::eg_type(47)]]__eg_position;
    template< typename __eg3 >struct [[clang::eg_type(48)]]__eg_color;
    template< typename __eg3 >struct [[clang::eg_type(49)]]__eg_size;
    template< typename __eg3 >struct [[clang::eg_type(50)]]__eg_starttime;
    template< typename __eg3 >struct [[clang::eg_type(51)]]__eg_lifetime;
    template< typename __eg3 >struct [[clang::eg_type(52)]]__eg_max_size;
    template< typename __eg3 >struct [[clang::eg_type(53)]]__eg_max_lifetime;
    template< typename __eg3 >struct [[clang::eg_type(55)]]__eg_Shrink
    {
      __eg_Shrink()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Shrink( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Shrink( const TFrom& from );
      template< typename TFrom >
      __eg_Shrink& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Shrink< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Shrink >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
      template< typename __eg4 >struct [[clang::eg_type(56)]]__eg_start;
    };
    using Shrink = __eg_Shrink< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(58)]]__eg_Grow
    {
      __eg_Grow()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Grow( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Grow( const TFrom& from );
      template< typename TFrom >
      __eg_Grow& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Grow< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Grow >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
      template< typename __eg4 >struct [[clang::eg_type(59)]]__eg_start;
    };
    using Grow = __eg_Grow< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(61)]]__eg_Move
    {
      __eg_Move()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Move( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Move( const TFrom& from );
      template< typename TFrom >
      __eg_Move& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Move< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Move >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
      template< typename __eg4 >struct [[clang::eg_type(62)]]__eg_direction;
      template< typename __eg4 >struct [[clang::eg_type(63)]]__eg_speed;
    };
    using Move = __eg_Move< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(65)]]__eg_ColourChanger
    {
      __eg_ColourChanger()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_ColourChanger( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_ColourChanger( const TFrom& from );
      template< typename TFrom >
      __eg_ColourChanger& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_ColourChanger< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_ColourChanger >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
      template< typename __eg4 >struct [[clang::eg_type(66)]]__eg_offset;
    };
    using ColourChanger = __eg_ColourChanger< __eg2 >;
  };
  using Circle = __eg_Circle< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(69)]]__eg_lastPos;
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;
template struct __eg_root< void >::__eg_Circle< void >;
template struct __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >;
template struct __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >;
template struct __eg_root< void >::__eg_Circle< void >::__eg_Move< void >;
template struct __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >;


//Explicit Trait Template Instantiations
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_position< void >
{
  using Type  = cinder::vec2;
  using Read  = eg::DimensionTraits< cinder::vec2 >::Read;
  using Write = eg::DimensionTraits< cinder::vec2 >::Write;
  using Get   = eg::DimensionTraits< cinder::vec2 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< cinder::vec2 >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_color< void >
{
  using Type  = cinder::Color;
  using Read  = eg::DimensionTraits< cinder::Color >::Read;
  using Write = eg::DimensionTraits< cinder::Color >::Write;
  using Get   = eg::DimensionTraits< cinder::Color >::Get;
  static const eg::Instance Size = eg::DimensionTraits< cinder::Color >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_size< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_starttime< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_lifetime< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >::__eg_start< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >::__eg_start< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_direction< void >
{
  using Type  = cinder::vec2;
  using Read  = eg::DimensionTraits< cinder::vec2 >::Read;
  using Write = eg::DimensionTraits< cinder::vec2 >::Write;
  using Get   = eg::DimensionTraits< cinder::vec2 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< cinder::vec2 >::Size;
};
template<>
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_speed< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >::__eg_offset< void >
{
  using Type  = float;
  using Read  = eg::DimensionTraits< float >::Read;
  using Write = eg::DimensionTraits< float >::Write;
  using Get   = eg::DimensionTraits< float >::Get;
  static const eg::Instance Size = eg::DimensionTraits< float >::Size;
};
template<>
template<>
struct __eg_root< void >::__eg_lastPos< void >
{
  using Type  = std::optional< cinder::vec2 >;
  using Read  = eg::DimensionTraits< std::optional< cinder::vec2 > >::Read;
  using Write = eg::DimensionTraits< std::optional< cinder::vec2 > >::Write;
  using Get   = eg::DimensionTraits< std::optional< cinder::vec2 > >::Get;
  static const eg::Instance Size = eg::DimensionTraits< std::optional< cinder::vec2 > >::Size;
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
