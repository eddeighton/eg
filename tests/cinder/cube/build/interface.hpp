#ifndef EG_INTERFACE_GUARD_9A55810B_47D0_408F_A112_D8FD8F10B63B__2019_Jun_13_02_20_12
#define EG_INTERFACE_GUARD_9A55810B_47D0_408F_A112_D8FD8F10B63B__2019_Jun_13_02_20_12
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct [[clang::eg_type( -32 )]]Camera;
struct [[clang::eg_type( -49 )]]CreateCube;
struct [[clang::eg_type( -41 )]]Cube;
struct [[clang::eg_type( -36 )]]Orbit;
struct [[clang::eg_type( -34 )]]eye;
struct [[clang::eg_type( -33 )]]perspective;
struct [[clang::eg_type( -42 )]]position;
struct [[clang::eg_type( -38 )]]radius;
struct [[clang::eg_type( -37 )]]rate;
struct [[clang::eg_type( -43 )]]size;
struct [[clang::eg_type( -46 )]]x;
struct [[clang::eg_type( -47 )]]y;

//EG Interface
template< typename __eg1 >struct [[clang::eg_type(31)]]__eg_root
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
  template< typename __eg2 >struct [[clang::eg_type(32)]]__eg_Camera
  {
    __eg_Camera()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Camera( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Camera( const TFrom& from );
    template< typename TFrom >
    __eg_Camera& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Camera< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Camera >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    template< typename __eg3 >struct [[clang::eg_type(33)]]__eg_perspective;
    template< typename __eg3 >struct [[clang::eg_type(34)]]__eg_eye;
    template< typename __eg3 >struct [[clang::eg_type(36)]]__eg_Orbit
    {
      __eg_Orbit()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Orbit( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Orbit( const TFrom& from );
      template< typename TFrom >
      __eg_Orbit& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Orbit< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Orbit >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
      template< typename __eg4 >struct [[clang::eg_type(37)]]__eg_rate;
      template< typename __eg4 >struct [[clang::eg_type(38)]]__eg_radius;
    };
    using Orbit = __eg_Orbit< __eg2 >;
  };
  using Camera = __eg_Camera< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(41)]]__eg_Cube
  {
    __eg_Cube()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Cube( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Cube( const TFrom& from );
    template< typename TFrom >
    __eg_Cube& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Cube< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Cube >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    static const eg::Instance SIZE = 9;
    template< typename __eg3 >struct [[clang::eg_type(42)]]__eg_position;
    template< typename __eg3 >struct [[clang::eg_type(43)]]__eg_size;
  };
  using Cube = __eg_Cube< __eg1 >;
  template< typename __eg2 >struct [[clang::eg_type(46)]]__eg_x;
  template< typename __eg2 >struct [[clang::eg_type(47)]]__eg_y;
  template< typename __eg2 >struct [[clang::eg_type(49)]]__eg_CreateCube
  {
    __eg_CreateCube()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_CreateCube( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_CreateCube( const TFrom& from );
    template< typename TFrom >
    __eg_CreateCube& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_CreateCube< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_CreateCube >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
  };
  using CreateCube = __eg_CreateCube< __eg1 >;
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;
template struct __eg_root< void >::__eg_Camera< void >;
template struct __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >;
template struct __eg_root< void >::__eg_Cube< void >;
template struct __eg_root< void >::__eg_CreateCube< void >;


//Explicit Trait Template Instantiations
template<>
template<>
template<>
struct __eg_root< void >::__eg_Camera< void >::__eg_perspective< void >
{
  using Type  = cinder::CameraPersp;
  using Read  = eg::DimensionTraits< cinder::CameraPersp >::Read;
  using Write = eg::DimensionTraits< cinder::CameraPersp >::Write;
  using Get   = eg::DimensionTraits< cinder::CameraPersp >::Get;
  static const eg::Instance Size = eg::DimensionTraits< cinder::CameraPersp >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Camera< void >::__eg_eye< void >
{
  using Type  = cinder::vec3;
  using Read  = eg::DimensionTraits< cinder::vec3 >::Read;
  using Write = eg::DimensionTraits< cinder::vec3 >::Write;
  using Get   = eg::DimensionTraits< cinder::vec3 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< cinder::vec3 >::Size;
};
template<>
template<>
template<>
template<>
struct __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_rate< void >
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
struct __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_radius< void >
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
struct __eg_root< void >::__eg_Cube< void >::__eg_position< void >
{
  using Type  = cinder::vec3;
  using Read  = eg::DimensionTraits< cinder::vec3 >::Read;
  using Write = eg::DimensionTraits< cinder::vec3 >::Write;
  using Get   = eg::DimensionTraits< cinder::vec3 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< cinder::vec3 >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Cube< void >::__eg_size< void >
{
  using Type  = cinder::vec3;
  using Read  = eg::DimensionTraits< cinder::vec3 >::Read;
  using Write = eg::DimensionTraits< cinder::vec3 >::Write;
  using Get   = eg::DimensionTraits< cinder::vec3 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< cinder::vec3 >::Size;
};
template<>
template<>
struct __eg_root< void >::__eg_x< void >
{
  using Type  = int;
  using Read  = eg::DimensionTraits< int >::Read;
  using Write = eg::DimensionTraits< int >::Write;
  using Get   = eg::DimensionTraits< int >::Get;
  static const eg::Instance Size = eg::DimensionTraits< int >::Size;
};
template<>
template<>
struct __eg_root< void >::__eg_y< void >
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
