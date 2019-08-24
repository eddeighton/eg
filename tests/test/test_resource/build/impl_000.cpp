#include "structures.hpp"


//input::Action function forward declarations

__eg_root< void > root_starter( eg::Instance _gid );
void root_stopper( eg::Instance _gid );

__eg_root< void >::__eg_PrintRes< void > root_PrintRes_starter( eg::Instance _gid );
void root_PrintRes_stopper( eg::Instance _gid );



//invocation implementations
template< typename ResultType, typename ContextType, typename TypePathType, typename OperationType >
struct __invoke_impl{
    template< typename... Args >
    ResultType operator()( ContextType, Args... )
    {
        static_assert( 0 && typeid( ResultType ).name() && typeid( ContextType ).name() && typeid( TypePathType ).name(), "Critical error: Invocation system failed to match implementation" );
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_PrintRes< void >,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_PrintRes< void > >,
    __eg_ImpNoParams
>
{
    template< typename... Args >
    __eg_root< void >::__eg_PrintRes< void > operator()( __eg_root< void > context, Args... args )
    {
        __eg_root< void >::__eg_PrintRes< void > ref = root_PrintRes_starter( context.data.instance );
        if( ref )
        {
            ref( args... );
            root_PrintRes_stopper( ref.data.instance );
        }
        return ref;
    }
};

//generic variant invocation adaptor
template< typename... Ts >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< __eg_variant< Ts... >, TypePath, Operation >::Type
__eg_variant< Ts... >::invoke( Args... args )
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_variant< Ts... >, TypePath, Operation >::Type, __eg_variant< Ts... >, CanonicalTypePathType, Operation >()( *this, args... );
}


//root
template<>
struct eg::is_convertible< __eg_root< void >, __eg_root< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_root( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 19: //root
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename TFrom >
__eg_root< __eg1 >& __eg_root< __eg1 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 19: //root
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root[ instance ].g_root_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root[ instance ].g_root_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root[ instance ].g_root_state;
}


//root::PrintRes
template<>
struct eg::is_convertible< __eg_root< void >::__eg_PrintRes< void >, __eg_root< void >::__eg_PrintRes< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_PrintRes< __eg2 >::__eg_PrintRes( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_PrintRes< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 26: //root::PrintRes
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_PrintRes< __eg2 >& __eg_root< __eg1 >::__eg_PrintRes< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_PrintRes< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 26: //root::PrintRes
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_PrintRes< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_PrintRes[ instance ].g_root_PrintRes_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_PrintRes< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_PrintRes[ instance ].g_root_PrintRes_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_PrintRes< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_PrintRes[ instance ].g_root_PrintRes_state;
}


//root
template< typename __eg1 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< __eg_root< __eg1 >, TypePath, Operation >::Type
__eg_root< __eg1 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >, TypePath, Operation >::Type, __eg_root< __eg1 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::PrintRes
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_PrintRes< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_PrintRes< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_PrintRes< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_PrintRes< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}


