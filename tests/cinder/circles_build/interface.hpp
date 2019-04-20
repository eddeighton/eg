#ifndef EG_INTERFACE_GUARD_64E5412A_3E85_44F7_9705_E3F6E8A88FB7__2019_Apr_20_00_14_01
#define EG_INTERFACE_GUARD_64E5412A_3E85_44F7_9705_E3F6E8A88FB7__2019_Apr_20_00_14_01
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

struct [[clang::eg_type( -19 )]]Circle;
struct [[clang::eg_type( -24 )]]ColorChanger;
struct [[clang::eg_type( -27 )]]Spiral;
struct [[clang::eg_type( -21 )]]color;
struct [[clang::eg_type( -25 )]]f;
struct [[clang::eg_type( -20 )]]position;
struct [[clang::eg_type( -22 )]]size;

//EG Interface
template< typename __EGInterface1 >struct [[clang::eg_type(18)]]__interface_root
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
  static const EGInstance ID = 18;
  template< typename __EGInterface2 >struct [[clang::eg_type(19)]]__interface_Circle
  {
    __interface_Circle()
    {
      data.instance = 0;
      data.type = __interface_Circle::ID;
      data.timestamp = EG_INVALID_TIMESTAMP;
    }
    __interface_Circle( EGInstance instance, EGTimeStamp timestamp )
    {
      data.instance = instance;
      data.type = __interface_Circle::ID;
      data.timestamp = timestamp;
    }
    template< typename TFrom >
    __interface_Circle( const TFrom& from );
    template< typename TFrom >
    __interface_Circle& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename result_type< __interface_Circle< __EGInterface2 >, TypePath, Operation >::Type invoke( Args... args );
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
    class Iterator : public std::iterator< std::forward_iterator_tag, __interface_Circle >
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
      __interface_Circle& operator*();
    };
    using EGRangeType = EGRange< Iterator >;
    __eg_reference data;
    static const EGInstance SIZE = 2048;
    static const EGInstance ID = 19;
    template< typename __EGInterface3 >struct [[clang::eg_type(20)]]__interface_position;
    template< typename __EGInterface3 >struct [[clang::eg_type(21)]]__interface_color;
    template< typename __EGInterface3 >struct [[clang::eg_type(22)]]__interface_size;
    template< typename __EGInterface3 >struct [[clang::eg_type(24)]]__interface_ColorChanger
    {
      __interface_ColorChanger()
      {
        data.instance = 0;
        data.type = __interface_ColorChanger::ID;
        data.timestamp = EG_INVALID_TIMESTAMP;
      }
      __interface_ColorChanger( EGInstance instance, EGTimeStamp timestamp )
      {
        data.instance = instance;
        data.type = __interface_ColorChanger::ID;
        data.timestamp = timestamp;
      }
      template< typename TFrom >
      __interface_ColorChanger( const TFrom& from );
      template< typename TFrom >
      __interface_ColorChanger& operator=( const TFrom& from );
      template< typename TypePath, typename Operation, typename... Args >
      typename result_type< __interface_ColorChanger< __EGInterface3 >, TypePath, Operation >::Type invoke( Args... args );
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
      class Iterator : public std::iterator< std::forward_iterator_tag, __interface_ColorChanger >
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
        __interface_ColorChanger& operator*();
      };
      using EGRangeType = EGRange< Iterator >;
      __eg_reference data;
      static const EGInstance ID = 24;
      template< typename __EGInterface4 >struct [[clang::eg_type(25)]]__interface_f;
    };
    using ColorChanger = __interface_ColorChanger< __EGInterface2 >;
  };
  using Circle = __interface_Circle< __EGInterface1 >;
  template< typename __EGInterface2 >struct [[clang::eg_type(27)]]__interface_Spiral
  {
    __interface_Spiral()
    {
      data.instance = 0;
      data.type = __interface_Spiral::ID;
      data.timestamp = EG_INVALID_TIMESTAMP;
    }
    __interface_Spiral( EGInstance instance, EGTimeStamp timestamp )
    {
      data.instance = instance;
      data.type = __interface_Spiral::ID;
      data.timestamp = timestamp;
    }
    template< typename TFrom >
    __interface_Spiral( const TFrom& from );
    template< typename TFrom >
    __interface_Spiral& operator=( const TFrom& from );
    template< typename TypePath, typename Operation, typename... Args >
    typename result_type< __interface_Spiral< __EGInterface2 >, TypePath, Operation >::Type invoke( Args... args );
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
    class Iterator : public std::iterator< std::forward_iterator_tag, __interface_Spiral >
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
      __interface_Spiral& operator*();
    };
    using EGRangeType = EGRange< Iterator >;
    __eg_reference data;
    static const EGInstance ID = 27;
  };
  using Spiral = __interface_Spiral< __EGInterface1 >;
};
using root = __interface_root< void >;


//Explicit Template Instantiations
template struct __interface_root< void >;
template struct __interface_root< void >::__interface_Circle< void >;
template struct __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >;
template struct __interface_root< void >::__interface_Spiral< void >;


//Explicit Trait Template Instantiations
template<>
template<>
template<>
struct __interface_root< void >::__interface_Circle< void >::__interface_position< void >
{
  using Type  = vec2;
  using Read  = DimensionTraits< vec2 >::Read;
  using Write = DimensionTraits< vec2 >::Write;
  static const EGInstance Size = DimensionTraits< vec2 >::Size;
};
template<>
template<>
template<>
struct __interface_root< void >::__interface_Circle< void >::__interface_color< void >
{
  using Type  = Color;
  using Read  = DimensionTraits< Color >::Read;
  using Write = DimensionTraits< Color >::Write;
  static const EGInstance Size = DimensionTraits< Color >::Size;
};
template<>
template<>
template<>
struct __interface_root< void >::__interface_Circle< void >::__interface_size< void >
{
  using Type  = float;
  using Read  = DimensionTraits< float >::Read;
  using Write = DimensionTraits< float >::Write;
  static const EGInstance Size = DimensionTraits< float >::Size;
};
template<>
template<>
template<>
template<>
struct __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >::__interface_f< void >
{
  using Type  = float;
  using Read  = DimensionTraits< float >::Read;
  using Write = DimensionTraits< float >::Write;
  static const EGInstance Size = DimensionTraits< float >::Size;
};


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
#endif
