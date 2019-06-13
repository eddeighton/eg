#include "structures.hpp"


//input::Action function forward declarations

__eg_root< void > root_starter( eg::Instance _gid );
void root_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Square< void > root_Square_starter( eg::Instance _gid );
void root_Square_stopper( eg::Instance _gid );

__eg_root< void >::__eg_imgui< void > root_imgui_starter( eg::Instance _gid );
void root_imgui_stopper( eg::Instance _gid );

//input::Action Function Implementations

__eg_root< void > root_starter( std::vector< std::function< void() > >& functions )
{
    const eg::Instance startCycle = clock::cycle();
    __eg_root< void >& reference = g_root[ 0 ].g_root_reference;
    reference.data.timestamp = startCycle;
    g_root[ 0 ].g_root_state = ::eg::action_running;
    events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
    g_root[ 0 ].g_root_fiber = boost::fibers::fiber
    (                                                                                       
        [ reference, &functions ]()                                                         
        {                                                                                   
            std::shared_ptr< boost::fibers::barrier > barrier(                              
                std::make_shared< boost::fibers::barrier >( functions.size() + 1U ) );      
                                                                                            
            for( auto& fn : functions )                                                     
            {                                                                               
                boost::fibers::fiber                                                        
                (                                                                           
                    std::bind(                                                              
                        []( std::function< void() >& fn,                                    
                            std::shared_ptr< boost::fibers::barrier >& barrier ) mutable    
                        {                                                                   
                            fn();                                                           
                            barrier->wait();                                                
                        },                                                                  
                        fn,                                                                 
                        barrier                                                             
                    )                                                                       
                ).detach();                                                                 
            }                                                                               
                                                                                            
            try                                                                             
            {                                                                               
                reference();                                                                
            }                                                                               
            catch( eg::termination_exception )                                              
            {                                                                               
            }                                                                               
            //wait for all fibers to complete                                               
            barrier->wait();                                                                
            //run the stopper                                                               
            root_stopper( reference.data.instance );                
        }                                                                                   
    );
    g_root[ 0 ].g_root_fiber.properties< eg::fiber_props >().setReference( reference.data );
    return reference;
}

void root_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root[ _gid ].g_root_state != ::eg::action_stopped )
     {
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Square_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 4; childIndex != ( _gid + 1 ) * 4; ++childIndex )
         {
             root_imgui_stopper( childIndex );
         }
         g_root[ _gid ].g_root_state = ::eg::action_stopped;
         g_root[ _gid ].g_root_cycle = clock::cycle();
         if( g_root[ _gid ].g_root_fiber.joinable() )
             g_root[ _gid ].g_root_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root[ _gid ].g_root_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Square< void > root_Square_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootSquare_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         eg::Instance nextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 0 )
         {
             iter.head = 0U;
         }
         else
         {
             ++iter.head;
         }
         if( static_cast< eg::Instance >( iter.head ) == static_cast< eg::Instance >( iter.tail ) )
         {
             iter.full = 1U;
         }
         eg::Instance nextInstance = g_root_Square[ _gid * 1 + nextCellIndex ].g_root_Square_alloc_data;
         if( g_root_Square[ nextInstance ].g_root_Square_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootSquare_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Square< void >& reference = g_root_Square[ nextInstance ].g_root_Square_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Square[ nextInstance ].g_root_Square_state = ::eg::action_running;
                 events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
                 return reference;
             }
         }
         else
         {
             break;
         }
    }
    //failure return null handle
    __eg_root< void >::__eg_Square< void > nullInstance;
    return nullInstance;
}

void root_Square_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Square[ _gid ].g_root_Square_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootSquare_alloc_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              eg::Instance freeCellIndex = static_cast< eg::Instance >( iter.tail );
              //if buffer is full then set the protection bit while freeing
              if( iter.full )
              {
                  iter.protection = 1U;
                  iter.full = 0U;
              }
              //claim the index to store free instance
              if( static_cast< eg::Instance >( iter.tail ) == 1 - 1U )
              {
                  iter.tail = 0U;
              }
              else
              {
                  ++iter.tail;
              }
              //attempt to set the atomic
              if( g_root[ _parent_id ].g_rootSquare_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Square[ freeCellIndex ].g_root_Square_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootSquare_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Square[ _gid ].g_root_Square_state = ::eg::action_stopped;
         g_root_Square[ _gid ].g_root_Square_cycle = clock::cycle();
         if( g_root_Square[ _gid ].g_root_Square_fiber.joinable() )
             g_root_Square[ _gid ].g_root_Square_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Square[ _gid ].g_root_Square_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_imgui< void > root_imgui_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootimgui_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         eg::Instance nextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 3 )
         {
             iter.head = 0U;
         }
         else
         {
             ++iter.head;
         }
         if( static_cast< eg::Instance >( iter.head ) == static_cast< eg::Instance >( iter.tail ) )
         {
             iter.full = 1U;
         }
         eg::Instance nextInstance = g_root_imgui[ _gid * 4 + nextCellIndex ].g_root_imgui_alloc_data;
         if( g_root_imgui[ nextInstance ].g_root_imgui_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootimgui_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_imgui< void >& reference = g_root_imgui[ nextInstance ].g_root_imgui_reference;
                 reference.data.timestamp = startCycle;
                 g_root_imgui[ nextInstance ].g_root_imgui_state = ::eg::action_running;
                 events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
                 return reference;
             }
         }
         else
         {
             break;
         }
    }
    //failure return null handle
    __eg_root< void >::__eg_imgui< void > nullInstance;
    return nullInstance;
}

void root_imgui_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 4;
     if( g_root_imgui[ _gid ].g_root_imgui_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootimgui_alloc_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              eg::Instance freeCellIndex = static_cast< eg::Instance >( iter.tail );
              //if buffer is full then set the protection bit while freeing
              if( iter.full )
              {
                  iter.protection = 1U;
                  iter.full = 0U;
              }
              //claim the index to store free instance
              if( static_cast< eg::Instance >( iter.tail ) == 4 - 1U )
              {
                  iter.tail = 0U;
              }
              else
              {
                  ++iter.tail;
              }
              //attempt to set the atomic
              if( g_root[ _parent_id ].g_rootimgui_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_imgui[ freeCellIndex ].g_root_imgui_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootimgui_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_imgui[ _gid ].g_root_imgui_state = ::eg::action_stopped;
         g_root_imgui[ _gid ].g_root_imgui_cycle = clock::cycle();
         if( g_root_imgui[ _gid ].g_root_imgui_fiber.joinable() )
             g_root_imgui[ _gid ].g_root_imgui_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_imgui[ _gid ].g_root_imgui_reference, sizeof( eg::reference ) );
     }
}




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
    void,
    __eg_root< void >,
    __eg_type_path< size >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void > context, __eg_root< void >::__eg_size< void >::Write value )
    {
        g_root[ context.data.instance ].m_size = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_imgui< void >,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_imgui< void >, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_imgui< void > operator()( __eg_root< void > context, Args... args )
    {
        __eg_root< void >::__eg_imgui< void > ref = root_imgui_starter( context.data.instance );
        if( ref )
        {
            g_root_imgui[ ref.data.instance ].g_root_imgui_fiber = boost::fibers::fiber
            (
                [ = ]()
                {
                    try
                    {
                        ref( args... );
                    }
                    catch( eg::termination_exception )
                    {
                    }
                    root_imgui_stopper( ref.data.instance );
                }
            );
            g_root_imgui[ ref.data.instance ].g_root_imgui_fiber.properties< eg::fiber_props >().setReference( ref.data );
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
        iter = eg::Iterator( g_root[ context.data.instance ].g_rootSquare_alloc_iter.load() );
        if( iter.full || ( iter.head != iter.tail ) ) return false;
        iter = eg::Iterator( g_root[ context.data.instance ].g_rootimgui_alloc_iter.load() );
        if( iter.full || ( iter.head != iter.tail ) ) return false;
        return true;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_size< void >::Read,
    __eg_root< void >::__eg_Square< void >,
    __eg_type_path< size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_size< void >::Read operator()( __eg_root< void >::__eg_Square< void > context )
    {
        return g_root[ context.data.instance ].m_size;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_size< void >::Get,
    __eg_root< void >::__eg_imgui< void >,
    __eg_type_path< size, Get >,
    Get
>
{
    __eg_root< void >::__eg_size< void >::Get operator()( __eg_root< void >::__eg_imgui< void > context )
    {
        return g_root[ ( context.data.instance ) / 4 ].m_size;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Square< void >,
    __eg_root< void >::__eg_imgui< void >,
    __eg_type_path< __eg_root< void >::__eg_Square< void >, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Square< void > operator()( __eg_root< void >::__eg_imgui< void > context, Args... args )
    {
        __eg_root< void >::__eg_Square< void > ref = root_Square_starter( ( context.data.instance ) / 4 );
        if( ref )
        {
            g_root_Square[ ref.data.instance ].g_root_Square_fiber = boost::fibers::fiber
            (
                [ = ]()
                {
                    try
                    {
                        ref( args... );
                    }
                    catch( eg::termination_exception )
                    {
                    }
                    root_Square_stopper( ref.data.instance );
                }
            );
            g_root_Square[ ref.data.instance ].g_root_Square_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_imgui< void >,
    __eg_root< void >::__eg_imgui< void >,
    __eg_type_path< __eg_root< void >::__eg_imgui< void >, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_imgui< void > operator()( __eg_root< void >::__eg_imgui< void > context, Args... args )
    {
        __eg_root< void >::__eg_imgui< void > ref = root_imgui_starter( ( context.data.instance ) / 4 );
        if( ref )
        {
            g_root_imgui[ ref.data.instance ].g_root_imgui_fiber = boost::fibers::fiber
            (
                [ = ]()
                {
                    try
                    {
                        ref( args... );
                    }
                    catch( eg::termination_exception )
                    {
                    }
                    root_imgui_stopper( ref.data.instance );
                }
            );
            g_root_imgui[ ref.data.instance ].g_root_imgui_fiber.properties< eg::fiber_props >().setReference( ref.data );
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
     case 28: //root
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
      case 28: //root
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


//root::Square
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Square< void >, __eg_root< void >::__eg_Square< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Square< __eg2 >::__eg_Square( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Square< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 34: //root::Square
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
typename __eg_root< __eg1 >::template __eg_Square< __eg2 >& __eg_root< __eg1 >::__eg_Square< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Square< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 34: //root::Square
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Square< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Square[ instance ].g_root_Square_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Square< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Square[ instance ].g_root_Square_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Square< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Square[ instance ].g_root_Square_state;
}


//root::imgui
template<>
struct eg::is_convertible< __eg_root< void >::__eg_imgui< void >, __eg_root< void >::__eg_imgui< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_imgui< __eg2 >::__eg_imgui( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_imgui< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 39: //root::imgui
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
typename __eg_root< __eg1 >::template __eg_imgui< __eg2 >& __eg_root< __eg1 >::__eg_imgui< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_imgui< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 39: //root::imgui
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_imgui< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_imgui[ instance ].g_root_imgui_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_imgui< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_imgui[ instance ].g_root_imgui_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_imgui< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_imgui[ instance ].g_root_imgui_state;
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

//root::Square
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Square< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Square< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Square< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Square< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::imgui
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_imgui< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_imgui< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_imgui< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_imgui< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}


