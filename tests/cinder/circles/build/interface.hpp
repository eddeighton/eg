#ifndef EG_INTERFACE_GUARD_B4DF49CE_72B1_4B4A_A155_CDB38375DC7E__2019_Jun_12_18_01_30
#define EG_INTERFACE_GUARD_B4DF49CE_72B1_4B4A_A155_CDB38375DC7E__2019_Jun_12_18_01_30
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct [[clang::eg_type( -73 )]]Attract;
struct [[clang::eg_type( -65 )]]Circle;
struct [[clang::eg_type( -57 )]]ColorChanger;
struct [[clang::eg_type( -55 )]]Default;
struct [[clang::eg_type( -60 )]]Morph;
struct [[clang::eg_type( -75 )]]Randomise;
struct [[clang::eg_type( -50 )]]Shape;
struct [[clang::eg_type( -67 )]]Spiral;
struct [[clang::eg_type( -63 )]]StrokedCircle;
struct [[clang::eg_type( -53 )]]color;
struct [[clang::eg_type( -68 )]]curve;
struct [[clang::eg_type( -58 )]]f;
struct [[clang::eg_type( -51 )]]position;
struct [[clang::eg_type( -52 )]]positionActual;
struct [[clang::eg_type( -69 )]]radius;
struct [[clang::eg_type( -77 )]]range;
struct [[clang::eg_type( -70 )]]relative;
struct [[clang::eg_type( -54 )]]size;
struct [[clang::eg_type( -61 )]]speed;

//EG Interface
template< typename __eg1 >struct [[clang::eg_type(48)]]__eg_root
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
  template< typename __eg2 >struct [[clang::eg_type(50)]]__eg_Shape
  {
    __eg_Shape()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Shape( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Shape( const TFrom& from );
    template< typename TFrom >
    __eg_Shape& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Shape< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Shape >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    template< typename __eg3 >struct [[clang::eg_type(51)]]__eg_position;
    template< typename __eg3 >struct [[clang::eg_type(52)]]__eg_positionActual;
    template< typename __eg3 >struct [[clang::eg_type(53)]]__eg_color;
    template< typename __eg3 >struct [[clang::eg_type(54)]]__eg_size;
    template< typename __eg3 >struct [[clang::eg_type(55)]]__eg_Default
    {
      __eg_Default()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Default( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Default( const TFrom& from );
      template< typename TFrom >
      __eg_Default& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Default< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Default >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
    };
    using Default = __eg_Default< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(57)]]__eg_ColorChanger
    {
      __eg_ColorChanger()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_ColorChanger( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_ColorChanger( const TFrom& from );
      template< typename TFrom >
      __eg_ColorChanger& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_ColorChanger< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_ColorChanger >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
      template< typename __eg4 >struct [[clang::eg_type(58)]]__eg_f;
    };
    using ColorChanger = __eg_ColorChanger< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(60)]]__eg_Morph
    {
      __eg_Morph()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Morph( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Morph( const TFrom& from );
      template< typename TFrom >
      __eg_Morph& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Morph< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Morph >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
      template< typename __eg4 >struct [[clang::eg_type(61)]]__eg_speed;
    };
    using Morph = __eg_Morph< __eg2 >;
  };
  using Shape = __eg_Shape< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(63)]]__eg_StrokedCircle
  {
    __eg_StrokedCircle()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_StrokedCircle( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_StrokedCircle( const TFrom& from );
    template< typename TFrom >
    __eg_StrokedCircle& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_StrokedCircle< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_StrokedCircle >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    static const eg::Instance SIZE = 256;
    template< typename __eg2 >struct [[clang::eg_type(23)]]__eg_Base_0;
  };
  using StrokedCircle = __eg_StrokedCircle< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(65)]]__eg_Circle
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
    static const eg::Instance SIZE = 256;
    template< typename __eg2 >struct [[clang::eg_type(27)]]__eg_Base_0;
  };
  using Circle = __eg_Circle< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(67)]]__eg_Spiral
  {
    __eg_Spiral()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Spiral( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Spiral( const TFrom& from );
    template< typename TFrom >
    __eg_Spiral& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Spiral< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Spiral >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    template< typename __eg3 >struct [[clang::eg_type(68)]]__eg_curve;
    template< typename __eg3 >struct [[clang::eg_type(69)]]__eg_radius;
    template< typename __eg3 >struct [[clang::eg_type(70)]]__eg_relative;
    template< typename __eg3 >struct [[clang::eg_type(71)]]__eg_speed;
  };
  using Spiral = __eg_Spiral< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(73)]]__eg_Attract
  {
    __eg_Attract()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Attract( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Attract( const TFrom& from );
    template< typename TFrom >
    __eg_Attract& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Attract< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Attract >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
  };
  using Attract = __eg_Attract< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(75)]]__eg_Randomise
  {
    __eg_Randomise()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Randomise( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Randomise( const TFrom& from );
    template< typename TFrom >
    __eg_Randomise& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Randomise< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Randomise >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    template< typename __eg3 >struct [[clang::eg_type(77)]]__eg_range;
  };
  using Randomise = __eg_Randomise< __eg1 >;
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;
template struct __eg_root< void >::__eg_Shape< void >;
template struct __eg_root< void >::__eg_Shape< void >::__eg_Default< void >;
template struct __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >;
template struct __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >;
template struct __eg_root< void >::__eg_StrokedCircle< void >;
template struct __eg_root< void >::__eg_Circle< void >;
template struct __eg_root< void >::__eg_Spiral< void >;
template struct __eg_root< void >::__eg_Attract< void >;
template struct __eg_root< void >::__eg_Randomise< void >;


//Explicit Trait Template Instantiations
template<>
template<>
template<>
struct __eg_root< void >::__eg_Shape< void >::__eg_position< void >
{
  using Type  = glm::vec2;
  using Read  = eg::DimensionTraits< glm::vec2 >::Read;
  using Write = eg::DimensionTraits< glm::vec2 >::Write;
  using Get   = eg::DimensionTraits< glm::vec2 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< glm::vec2 >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >
{
  using Type  = glm::vec2;
  using Read  = eg::DimensionTraits< glm::vec2 >::Read;
  using Write = eg::DimensionTraits< glm::vec2 >::Write;
  using Get   = eg::DimensionTraits< glm::vec2 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< glm::vec2 >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Shape< void >::__eg_color< void >
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
struct __eg_root< void >::__eg_Shape< void >::__eg_size< void >
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
struct __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::__eg_f< void >
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
struct __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >::__eg_speed< void >
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
struct __eg_root< void >::__eg_StrokedCircle< void >::__eg_Base_0< void >
{
  using Type  = Shape;
  using Dependency = eg::ObjectTraits< Shape >::Dependency;
  static const eg::Instance Size = eg::ObjectTraits< Shape >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Circle< void >::__eg_Base_0< void >
{
  using Type  = Shape;
  using Dependency = eg::ObjectTraits< Shape >::Dependency;
  static const eg::Instance Size = eg::ObjectTraits< Shape >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Spiral< void >::__eg_curve< void >
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
struct __eg_root< void >::__eg_Spiral< void >::__eg_radius< void >
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
struct __eg_root< void >::__eg_Spiral< void >::__eg_relative< void >
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
struct __eg_root< void >::__eg_Spiral< void >::__eg_speed< void >
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
struct __eg_root< void >::__eg_Randomise< void >::__eg_range< void >
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
