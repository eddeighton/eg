#include "structures.hpp"


//input::Action function forward declarations

__eg_root< void > root_starter( eg::Instance _gid );
void root_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Camera< void > root_Camera_starter( eg::Instance _gid );
void root_Camera_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Cube< void > root_Cube_starter( eg::Instance _gid );
void root_Cube_stopper( eg::Instance _gid );

__eg_root< void >::__eg_CreateCube< void > root_CreateCube_starter( eg::Instance _gid );
void root_CreateCube_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > root_Camera_Orbit_starter( eg::Instance _gid );
void root_Camera_Orbit_stopper( eg::Instance _gid );

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
             root_Camera_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 9; childIndex != ( _gid + 1 ) * 9; ++childIndex )
         {
             root_Cube_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_CreateCube_stopper( childIndex );
         }
         g_root[ _gid ].g_root_state = ::eg::action_stopped;
         g_root[ _gid ].g_root_cycle = clock::cycle();
         if( g_root[ _gid ].g_root_fiber.joinable() )
             g_root[ _gid ].g_root_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root[ _gid ].g_root_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Camera< void > root_Camera_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootCamera_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Camera[ _gid * 1 + nextCellIndex ].g_root_Camera_alloc_data;
         if( g_root_Camera[ nextInstance ].g_root_Camera_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootCamera_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Camera< void >& reference = g_root_Camera[ nextInstance ].g_root_Camera_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Camera[ nextInstance ].g_root_Camera_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Camera< void > nullInstance;
    return nullInstance;
}

void root_Camera_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Camera[ _gid ].g_root_Camera_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootCamera_alloc_iter.load() );
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
              if( g_root[ _parent_id ].g_rootCamera_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Camera[ freeCellIndex ].g_root_Camera_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootCamera_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Camera_Orbit_stopper( childIndex );
         }
         g_root_Camera[ _gid ].g_root_Camera_state = ::eg::action_stopped;
         g_root_Camera[ _gid ].g_root_Camera_cycle = clock::cycle();
         if( g_root_Camera[ _gid ].g_root_Camera_fiber.joinable() )
             g_root_Camera[ _gid ].g_root_Camera_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Camera[ _gid ].g_root_Camera_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Cube< void > root_Cube_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootCube_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         eg::Instance nextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 8 )
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
         eg::Instance nextInstance = g_root_Cube[ _gid * 9 + nextCellIndex ].g_root_Cube_alloc_data;
         if( g_root_Cube[ nextInstance ].g_root_Cube_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootCube_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Cube< void >& reference = g_root_Cube[ nextInstance ].g_root_Cube_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Cube[ nextInstance ].g_root_Cube_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Cube< void > nullInstance;
    return nullInstance;
}

void root_Cube_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 9;
     if( g_root_Cube[ _gid ].g_root_Cube_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootCube_alloc_iter.load() );
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
              if( static_cast< eg::Instance >( iter.tail ) == 9 - 1U )
              {
                  iter.tail = 0U;
              }
              else
              {
                  ++iter.tail;
              }
              //attempt to set the atomic
              if( g_root[ _parent_id ].g_rootCube_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Cube[ freeCellIndex ].g_root_Cube_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootCube_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Cube[ _gid ].g_root_Cube_state = ::eg::action_stopped;
         g_root_Cube[ _gid ].g_root_Cube_cycle = clock::cycle();
         if( g_root_Cube[ _gid ].g_root_Cube_fiber.joinable() )
             g_root_Cube[ _gid ].g_root_Cube_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Cube[ _gid ].g_root_Cube_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_CreateCube< void > root_CreateCube_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootCreateCube_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_CreateCube[ _gid * 1 + nextCellIndex ].g_root_CreateCube_alloc_data;
         if( g_root_CreateCube[ nextInstance ].g_root_CreateCube_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootCreateCube_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_CreateCube< void >& reference = g_root_CreateCube[ nextInstance ].g_root_CreateCube_reference;
                 reference.data.timestamp = startCycle;
                 g_root_CreateCube[ nextInstance ].g_root_CreateCube_state = ::eg::action_running;
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
    __eg_root< void >::__eg_CreateCube< void > nullInstance;
    return nullInstance;
}

void root_CreateCube_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_CreateCube[ _gid ].g_root_CreateCube_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootCreateCube_alloc_iter.load() );
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
              if( g_root[ _parent_id ].g_rootCreateCube_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_CreateCube[ freeCellIndex ].g_root_CreateCube_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootCreateCube_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_CreateCube[ _gid ].g_root_CreateCube_state = ::eg::action_stopped;
         g_root_CreateCube[ _gid ].g_root_CreateCube_cycle = clock::cycle();
         if( g_root_CreateCube[ _gid ].g_root_CreateCube_fiber.joinable() )
             g_root_CreateCube[ _gid ].g_root_CreateCube_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_CreateCube[ _gid ].g_root_CreateCube_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > root_Camera_Orbit_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Camera[ _gid ].g_root_CameraOrbit_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Camera_Orbit[ _gid * 1 + nextCellIndex ].g_root_Camera_Orbit_alloc_data;
         if( g_root_Camera_Orbit[ nextInstance ].g_root_Camera_Orbit_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Camera[ _gid ].g_root_CameraOrbit_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >& reference = g_root_Camera_Orbit[ nextInstance ].g_root_Camera_Orbit_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Camera_Orbit[ nextInstance ].g_root_Camera_Orbit_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > nullInstance;
    return nullInstance;
}

void root_Camera_Orbit_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Camera_Orbit[ _gid ].g_root_Camera_Orbit_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Camera[ _parent_id ].g_root_CameraOrbit_alloc_iter.load() );
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
              if( g_root_Camera[ _parent_id ].g_root_CameraOrbit_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Camera_Orbit[ freeCellIndex ].g_root_Camera_Orbit_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Camera[ _parent_id ].g_root_CameraOrbit_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Camera_Orbit[ _gid ].g_root_Camera_Orbit_state = ::eg::action_stopped;
         g_root_Camera_Orbit[ _gid ].g_root_Camera_Orbit_cycle = clock::cycle();
         if( g_root_Camera_Orbit[ _gid ].g_root_Camera_Orbit_fiber.joinable() )
             g_root_Camera_Orbit[ _gid ].g_root_Camera_Orbit_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Camera_Orbit[ _gid ].g_root_Camera_Orbit_reference, sizeof( eg::reference ) );
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
    __eg_root< void >::__eg_Camera< void >,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_Camera< void >, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Camera< void > operator()( __eg_root< void > context, Args... args )
    {
        __eg_root< void >::__eg_Camera< void > ref = root_Camera_starter( context.data.instance );
        if( ref )
        {
            g_root_Camera[ ref.data.instance ].g_root_Camera_fiber = boost::fibers::fiber
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
                    root_Camera_stopper( ref.data.instance );
                }
            );
            g_root_Camera[ ref.data.instance ].g_root_Camera_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >,
    __eg_root< void >::__eg_Camera< void >,
    __eg_type_path< Orbit, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > operator()( __eg_root< void >::__eg_Camera< void > context, Args... args )
    {
        __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > ref = root_Camera_Orbit_starter( context.data.instance );
        if( ref )
        {
            g_root_Camera_Orbit[ ref.data.instance ].g_root_Camera_Orbit_fiber = boost::fibers::fiber
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
                    root_Camera_Orbit_stopper( ref.data.instance );
                }
            );
            g_root_Camera_Orbit[ ref.data.instance ].g_root_Camera_Orbit_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >,
    __eg_type_path< rate >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > context, __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_rate< void >::Write value )
    {
        g_root_Camera_Orbit[ context.data.instance ].m_rate = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >,
    __eg_type_path< radius >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > context, __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_radius< void >::Write value )
    {
        g_root_Camera_Orbit[ context.data.instance ].m_radius = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >,
    __eg_type_path< x >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void > context, __eg_root< void >::__eg_x< void >::Write value )
    {
        g_root[ context.data.instance ].m_x = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >,
    __eg_type_path< y >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void > context, __eg_root< void >::__eg_y< void >::Write value )
    {
        g_root[ context.data.instance ].m_y = value;
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
        iter = eg::Iterator( g_root[ context.data.instance ].g_rootCube_alloc_iter.load() );
        if( iter.full || ( iter.head != iter.tail ) ) return false;
        iter = eg::Iterator( g_root[ context.data.instance ].g_rootCreateCube_alloc_iter.load() );
        if( iter.full || ( iter.head != iter.tail ) ) return false;
        iter = eg::Iterator( g_root[ context.data.instance ].g_rootCamera_alloc_iter.load() );
        if( iter.full || ( iter.head != iter.tail ) ) return false;
        return true;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Camera< void >::__eg_perspective< void >::Read,
    __eg_root< void >,
    __eg_type_path< perspective >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Camera< void >::__eg_perspective< void >::Read operator()( __eg_root< void > context )
    {
        return g_root_Camera[ context.data.instance ].m_perspective;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Camera< void >,
    __eg_type_path< eye >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Camera< void > context, __eg_root< void >::__eg_Camera< void >::__eg_eye< void >::Write value )
    {
        g_root_Camera[ context.data.instance ].m_eye = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Camera< void >::__eg_perspective< void >::Get,
    __eg_root< void >::__eg_Camera< void >,
    __eg_type_path< perspective, Get >,
    Get
>
{
    __eg_root< void >::__eg_Camera< void >::__eg_perspective< void >::Get operator()( __eg_root< void >::__eg_Camera< void > context )
    {
        return g_root_Camera[ context.data.instance ].m_perspective;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Camera< void >::__eg_eye< void >::Read,
    __eg_root< void >::__eg_Camera< void >,
    __eg_type_path< eye >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Camera< void >::__eg_eye< void >::Read operator()( __eg_root< void >::__eg_Camera< void > context )
    {
        return g_root_Camera[ context.data.instance ].m_eye;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_rate< void >::Read,
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >,
    __eg_type_path< rate >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_rate< void >::Read operator()( __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > context )
    {
        return g_root_Camera_Orbit[ context.data.instance ].m_rate;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_radius< void >::Read,
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >,
    __eg_type_path< radius >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::__eg_radius< void >::Read operator()( __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > context )
    {
        return g_root_Camera_Orbit[ context.data.instance ].m_radius;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >,
    __eg_type_path< eye >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > context, __eg_root< void >::__eg_Camera< void >::__eg_eye< void >::Write value )
    {
        g_root_Camera[ context.data.instance ].m_eye = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Cube< void >::__eg_position< void >::Read,
    __eg_root< void >::__eg_Cube< void >,
    __eg_type_path< position >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Cube< void >::__eg_position< void >::Read operator()( __eg_root< void >::__eg_Cube< void > context )
    {
        return g_root_Cube[ context.data.instance ].m_position;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Cube< void >::__eg_size< void >::Read,
    __eg_root< void >::__eg_Cube< void >,
    __eg_type_path< size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Cube< void >::__eg_size< void >::Read operator()( __eg_root< void >::__eg_Cube< void > context )
    {
        return g_root_Cube[ context.data.instance ].m_size;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Cube< void >,
    __eg_root< void >::__eg_CreateCube< void >,
    __eg_type_path< __eg_root< void >::__eg_Cube< void >, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Cube< void > operator()( __eg_root< void >::__eg_CreateCube< void > context, Args... args )
    {
        __eg_root< void >::__eg_Cube< void > ref = root_Cube_starter( context.data.instance );
        if( ref )
        {
            g_root_Cube[ ref.data.instance ].g_root_Cube_fiber = boost::fibers::fiber
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
                    root_Cube_stopper( ref.data.instance );
                }
            );
            g_root_Cube[ ref.data.instance ].g_root_Cube_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Cube< void >,
    __eg_type_path< size >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Cube< void > context, __eg_root< void >::__eg_Cube< void >::__eg_size< void >::Write value )
    {
        g_root_Cube[ context.data.instance ].m_size = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_x< void >::Read,
    __eg_root< void >::__eg_CreateCube< void >,
    __eg_type_path< x >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_x< void >::Read operator()( __eg_root< void >::__eg_CreateCube< void > context )
    {
        return g_root[ context.data.instance ].m_x;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_y< void >::Read,
    __eg_root< void >::__eg_CreateCube< void >,
    __eg_type_path< y >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_y< void >::Read operator()( __eg_root< void >::__eg_CreateCube< void > context )
    {
        return g_root[ context.data.instance ].m_y;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Cube< void >,
    __eg_type_path< position >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Cube< void > context, __eg_root< void >::__eg_Cube< void >::__eg_position< void >::Write value )
    {
        g_root_Cube[ context.data.instance ].m_position = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_CreateCube< void >,
    __eg_type_path< x >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_CreateCube< void > context, __eg_root< void >::__eg_x< void >::Write value )
    {
        g_root[ context.data.instance ].m_x = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_CreateCube< void >,
    __eg_type_path< y >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_CreateCube< void > context, __eg_root< void >::__eg_y< void >::Write value )
    {
        g_root[ context.data.instance ].m_y = value;
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
     case 56: //root
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
      case 56: //root
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


//root::Camera
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Camera< void >, __eg_root< void >::__eg_Camera< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Camera< __eg2 >::__eg_Camera( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Camera< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 62: //root::Camera
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
typename __eg_root< __eg1 >::template __eg_Camera< __eg2 >& __eg_root< __eg1 >::__eg_Camera< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Camera< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 62: //root::Camera
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Camera< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Camera[ instance ].g_root_Camera_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Camera< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Camera[ instance ].g_root_Camera_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Camera< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Camera[ instance ].g_root_Camera_state;
}


//root::Camera::Orbit
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >, __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Camera< __eg2 >::__eg_Orbit< __eg3 >::__eg_Orbit( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 82: //root::Camera::Orbit
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
}
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
typename __eg_root< __eg1 >::template __eg_Camera< __eg2 >::template __eg_Orbit< __eg3 >& __eg_root< __eg1 >::__eg_Camera< __eg2 >::__eg_Orbit< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 82: //root::Camera::Orbit
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_state;
}


//root::Cube
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Cube< void >, __eg_root< void >::__eg_Cube< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Cube< __eg2 >::__eg_Cube( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Cube< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 67: //root::Cube
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
typename __eg_root< __eg1 >::template __eg_Cube< __eg2 >& __eg_root< __eg1 >::__eg_Cube< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Cube< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 67: //root::Cube
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Cube< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Cube[ instance ].g_root_Cube_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Cube< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Cube[ instance ].g_root_Cube_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Cube< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Cube[ instance ].g_root_Cube_state;
}


//root::CreateCube
template<>
struct eg::is_convertible< __eg_root< void >::__eg_CreateCube< void >, __eg_root< void >::__eg_CreateCube< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_CreateCube< __eg2 >::__eg_CreateCube( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_CreateCube< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 72: //root::CreateCube
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
typename __eg_root< __eg1 >::template __eg_CreateCube< __eg2 >& __eg_root< __eg1 >::__eg_CreateCube< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_CreateCube< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 72: //root::CreateCube
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_CreateCube< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_CreateCube[ instance ].g_root_CreateCube_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_CreateCube< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_CreateCube[ instance ].g_root_CreateCube_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_CreateCube< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_CreateCube[ instance ].g_root_CreateCube_state;
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

//root::Camera
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Camera< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Camera< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Camera< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Camera< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Camera::Orbit
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Camera< __eg2 >::template __eg_Orbit< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Camera< __eg2 >::__eg_Orbit< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Camera< __eg2 >::__eg_Orbit< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Camera< __eg2 >::__eg_Orbit< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Cube
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Cube< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Cube< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Cube< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Cube< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::CreateCube
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_CreateCube< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_CreateCube< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_CreateCube< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_CreateCube< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}


