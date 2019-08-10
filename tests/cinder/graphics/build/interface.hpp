#ifndef EG_INTERFACE_GUARD_INTERFACE
#define EG_INTERFACE_GUARD_INTERFACE
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////



#define EG_FIBER_STACK_SIZE ( 16384 )

struct [[clang::eg_type( -48 )]]Distance;
struct [[clang::eg_type( -52 )]]Down;
struct [[clang::eg_type( -54 )]]Left;
struct [[clang::eg_type( -40 )]]Line;
struct [[clang::eg_type( -45 )]]Move;
struct [[clang::eg_type( -56 )]]Right;
struct [[clang::eg_type( -58 )]]Sequence;
struct [[clang::eg_type( -59 )]]Sequencer;
struct [[clang::eg_type( -49 )]]Speed;
struct [[clang::eg_type( -50 )]]Up;
struct [[clang::eg_type( -43 )]]colour;
struct [[clang::eg_type( -46 )]]dir;
struct [[clang::eg_type( -42 )]]end;
struct [[clang::eg_type( -39 )]]screen;
struct [[clang::eg_type( -41 )]]start;
struct [[clang::eg_type( -44 )]]width;

//EG Interface
template< typename __eg1 >struct [[clang::eg_type(37)]]__eg_root
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
  template< typename __eg2 >struct [[clang::eg_type(39)]]__eg_screen;
  template< typename __eg2 >struct [[clang::eg_type(40)]]__eg_Line
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
    template< typename __eg3 >struct [[clang::eg_type(41)]]__eg_start;
    template< typename __eg3 >struct [[clang::eg_type(42)]]__eg_end;
    template< typename __eg3 >struct [[clang::eg_type(43)]]__eg_colour;
    template< typename __eg3 >struct [[clang::eg_type(44)]]__eg_width;
    template< typename __eg3 >struct [[clang::eg_type(45)]]__eg_Move
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
      template< typename __eg4 >struct [[clang::eg_type(46)]]__eg_dir;
    };
    using Move = __eg_Move< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(48)]]__eg_Distance;
    template< typename __eg3 >struct [[clang::eg_type(49)]]__eg_Speed;
    template< typename __eg3 >struct [[clang::eg_type(50)]]__eg_Up
    {
      __eg_Up()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Up( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Up( const TFrom& from );
      template< typename TFrom >
      __eg_Up& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Up< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Up >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
    };
    using Up = __eg_Up< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(52)]]__eg_Down
    {
      __eg_Down()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Down( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Down( const TFrom& from );
      template< typename TFrom >
      __eg_Down& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Down< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Down >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
    };
    using Down = __eg_Down< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(54)]]__eg_Left
    {
      __eg_Left()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Left( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Left( const TFrom& from );
      template< typename TFrom >
      __eg_Left& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Left< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Left >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
    };
    using Left = __eg_Left< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(56)]]__eg_Right
    {
      __eg_Right()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Right( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Right( const TFrom& from );
      template< typename TFrom >
      __eg_Right& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Right< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Right >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
    };
    using Right = __eg_Right< __eg2 >;
    template< typename __eg3 >struct [[clang::eg_type(58)]]__eg_Sequence;
    template< typename __eg3 >struct [[clang::eg_type(59)]]__eg_Sequencer
    {
      __eg_Sequencer()
      {
        data.instance = 0;
        data.type = 0;
        data.timestamp = eg::INVALID_TIMESTAMP;
      }
      __eg_Sequencer( const eg::reference& reference )
      {
        data = reference;
      }
      template< typename TFrom >
      __eg_Sequencer( const TFrom& from );
      template< typename TFrom >
      __eg_Sequencer& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename eg::result_type< __eg_Sequencer< __eg3 >, TypePath, Operation >::Type invoke( Args... args ) const;
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
      using Iterator = __eg_ReferenceIterator< __eg_Sequencer >;
      using EGRangeType = __eg_Range< Iterator >;
      eg::reference data;
    };
    using Sequencer = __eg_Sequencer< __eg2 >;
  };
  using Line = __eg_Line< __eg1 >;
};
using root = __eg_root< void >;


//Explicit Template Instantiations
template struct __eg_root< void >;
template struct __eg_root< void >::__eg_Line< void >;
template struct __eg_root< void >::__eg_Line< void >::__eg_Move< void >;
template struct __eg_root< void >::__eg_Line< void >::__eg_Up< void >;
template struct __eg_root< void >::__eg_Line< void >::__eg_Down< void >;
template struct __eg_root< void >::__eg_Line< void >::__eg_Left< void >;
template struct __eg_root< void >::__eg_Line< void >::__eg_Right< void >;
template struct __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >;


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
template<>
template<>
template<>
struct __eg_root< void >::__eg_Line< void >::__eg_Distance< void >
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
struct __eg_root< void >::__eg_Line< void >::__eg_Speed< void >
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
struct __eg_root< void >::__eg_Line< void >::__eg_Sequence< void >
{
  using Type  = std::vector< int >;
  using Read  = eg::DimensionTraits< std::vector< int > >::Read;
  using Write = eg::DimensionTraits< std::vector< int > >::Write;
  using Get   = eg::DimensionTraits< std::vector< int > >::Get;
  static const eg::Instance Size = eg::DimensionTraits< std::vector< int > >::Size;
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
