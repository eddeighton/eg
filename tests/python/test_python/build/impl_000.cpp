#include "structures.hpp"


//input::Action function forward declarations

__eg_root< void > root_starter( eg::Instance _gid );
void root_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Test< void > root_Test_starter( eg::Instance _gid );
void root_Test_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Python< void > root_Python_starter( eg::Instance _gid );
void root_Python_stopper( eg::Instance _gid );



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
    __eg_root< void >::__eg_Python< void >,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_Python< void >, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Python< void > operator()( __eg_root< void > context, Args... args )
    {
        __eg_root< void >::__eg_Python< void > ref = root_Python_starter( context.data.instance );
        if( ref )
        {
            g_root_Python[ ref.data.instance ].g_root_Python_fiber = boost::fibers::fiber
            (
                std::allocator_arg,
                boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                [ = ]()
                {
                    try
                    {
                        ref( args... );
                    }
                    catch( std::exception& e )
                    {
                        ERR( e.what() );
                    }
                    catch( eg::termination_exception )
                    {
                    }
                    catch( ... )
                    {
                        ERR( "Unknown exception occured in root::Python" );
                    }
                    root_Python_stopper( ref.data.instance );
                }
            );
            g_root_Python[ ref.data.instance ].g_root_Python_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    bool,
    __eg_root< void >,
    __eg_type_path< Done >,
    Done
>
{
    bool operator()( __eg_root< void > context )
    {
        eg::Iterator iter;
        iter = eg::Iterator( g_root[ context.data.instance ].g_rootTest_ring_iter.load() );
        if( iter.full || ( iter.head != iter.tail ) ) return false;
        iter = eg::Iterator( g_root[ context.data.instance ].g_rootPython_ring_iter.load() );
        if( iter.full || ( iter.head != iter.tail ) ) return false;
        return true;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Python< void >,
    __eg_type_path< bContinue >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Python< void > context, __eg_root< void >::__eg_Python< void >::__eg_bContinue< void >::Write value )
    {
        g_root_Python[ context.data.instance ].m_bContinue = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Python< void >::__eg_bContinue< void >::Read,
    __eg_root< void >::__eg_Python< void >,
    __eg_type_path< bContinue >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Python< void >::__eg_bContinue< void >::Read operator()( __eg_root< void >::__eg_Python< void > context )
    {
        return g_root_Python[ context.data.instance ].m_bContinue;
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
     case 25: //root
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
      case 25: //root
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


//root::Test
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Test< void >, __eg_root< void >::__eg_Test< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Test< __eg2 >::__eg_Test( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Test< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 32: //root::Test
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
typename __eg_root< __eg1 >::template __eg_Test< __eg2 >& __eg_root< __eg1 >::__eg_Test< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Test< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 32: //root::Test
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Test< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Test[ instance ].g_root_Test_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Test< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Test[ instance ].g_root_Test_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Test< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Test[ instance ].g_root_Test_state;
}


//root::Python
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Python< void >, __eg_root< void >::__eg_Python< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Python< __eg2 >::__eg_Python( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Python< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 38: //root::Python
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
typename __eg_root< __eg1 >::template __eg_Python< __eg2 >& __eg_root< __eg1 >::__eg_Python< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Python< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 38: //root::Python
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Python< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Python[ instance ].g_root_Python_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Python< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Python[ instance ].g_root_Python_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Python< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Python[ instance ].g_root_Python_state;
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

//root::Test
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Test< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Test< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Test< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Test< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Python
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Python< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Python< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Python< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Python< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}


