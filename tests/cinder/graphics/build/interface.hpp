#ifndef EG_INTERFACE_GUARD_INTERFACE
#define EG_INTERFACE_GUARD_INTERFACE
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////



#define EG_FIBER_STACK_SIZE ( 16384 )

struct [[clang::eg_type( -24 )]]Line;
struct [[clang::eg_type( -30 )]]Move;
struct [[clang::eg_type( -27 )]]colour;
struct [[clang::eg_type( -31 )]]dir;
struct [[clang::eg_type( -26 )]]end;
struct [[clang::eg_type( -23 )]]screen;
struct [[clang::eg_type( -25 )]]start;
struct [[clang::eg_type( -28 )]]width;

//EG Interface
template< typename __eg1 >struct [[clang::eg_type(21)]]__eg_root
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
  template< typename __eg2 >struct [[clang::eg_type(23)]]__eg_screen;
  template< typename __eg2 >struct [[clang::eg_type(24)]]__eg_Line
  {
    __eg_Line()
    {
      data.instance = 0;
      data.type = 0;
      data.timestamp = eg::INVALID_TIMESTAMP;
    }
    __eg_Line( const eg::reference& reference )
    {
      data = reference;
    }
    template< typename TFrom >
    __eg_Line( const TFrom& from );
    template< typename TFrom >
    __eg_Line& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename eg::result_type< __eg_Line< __eg2 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
    using Iterator = __eg_ReferenceIterator< __eg_Line >;
    using EGRangeType = __eg_Range< Iterator >;
    eg::reference data;
    static const eg::Instance SIZE = 2048;
    template< typename __eg3 >struct [[clang::eg_type(25)]]__eg_start;
    template< typename __eg3 >struct [[clang::eg_type(26)]]__eg_end;
    template< typename __eg3 >struct [[clang::eg_type(27)]]__eg_colour;
    template< typename __eg3 >struct [[clang::eg_type(28)]]__eg_width;
    template< typename __eg3 >struct [[clang::eg_type(30)]]__eg_Move
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
      template< typename __eg4 >struct [[clang::eg_type(31)]]__eg_dir;
    };
    using Move = __eg_Move< __eg2 >;
  };
  using Line = __eg_Line< __eg1 >;
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;
template struct __eg_root< void >::__eg_Line< void >;
template struct __eg_root< void >::__eg_Line< void >::__eg_Move< void >;


//Explicit Trait Template Instantiations
template<>
template<>
struct __eg_root< void >::__eg_screen< void >
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
struct __eg_root< void >::__eg_Line< void >::__eg_start< void >
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
struct __eg_root< void >::__eg_Line< void >::__eg_end< void >
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
struct __eg_root< void >::__eg_Line< void >::__eg_colour< void >
{
  using Type  = glm::vec3;
  using Read  = eg::DimensionTraits< glm::vec3 >::Read;
  using Write = eg::DimensionTraits< glm::vec3 >::Write;
  using Get   = eg::DimensionTraits< glm::vec3 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< glm::vec3 >::Size;
};
template<>
template<>
template<>
struct __eg_root< void >::__eg_Line< void >::__eg_width< void >
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
struct __eg_root< void >::__eg_Line< void >::__eg_Move< void >::__eg_dir< void >
{
  using Type  = glm::vec2;
  using Read  = eg::DimensionTraits< glm::vec2 >::Read;
  using Write = eg::DimensionTraits< glm::vec2 >::Write;
  using Get   = eg::DimensionTraits< glm::vec2 >::Get;
  static const eg::Instance Size = eg::DimensionTraits< glm::vec2 >::Size;
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
