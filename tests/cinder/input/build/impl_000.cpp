#include "structures.hpp"


//input::Action function forward declarations

__eg_root< void > root_starter( eg::Instance _gid );
void root_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Circle< void > root_Circle_starter( eg::Instance _gid );
void root_Circle_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > root_Circle_Shrink_starter( eg::Instance _gid );
void root_Circle_Shrink_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Circle< void >::__eg_Grow< void > root_Circle_Grow_starter( eg::Instance _gid );
void root_Circle_Grow_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Circle< void >::__eg_Move< void > root_Circle_Move_starter( eg::Instance _gid );
void root_Circle_Move_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > root_Circle_ColourChanger_starter( eg::Instance _gid );
void root_Circle_ColourChanger_stopper( eg::Instance _gid );

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
         for( eg::Instance childIndex = _gid * 4096; childIndex != ( _gid + 1 ) * 4096; ++childIndex )
         {
             root_Circle_stopper( childIndex );
         }
         g_root[ _gid ].g_root_state = ::eg::action_stopped;
         g_root[ _gid ].g_root_cycle = clock::cycle();
         if( g_root[ _gid ].g_root_fiber.joinable() )
             g_root[ _gid ].g_root_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root[ _gid ].g_root_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Circle< void > root_Circle_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootCircle_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         eg::Instance nextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 4095 )
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
         eg::Instance nextInstance = g_root_Circle[ _gid * 4096 + nextCellIndex ].g_root_Circle_alloc_data;
         if( g_root_Circle[ nextInstance ].g_root_Circle_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootCircle_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Circle< void >& reference = g_root_Circle[ nextInstance ].g_root_Circle_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle[ nextInstance ].g_root_Circle_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Circle< void > nullInstance;
    return nullInstance;
}

void root_Circle_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 4096;
     if( g_root_Circle[ _gid ].g_root_Circle_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootCircle_alloc_iter.load() );
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
              if( static_cast< eg::Instance >( iter.tail ) == 4096 - 1U )
              {
                  iter.tail = 0U;
              }
              else
              {
                  ++iter.tail;
              }
              //attempt to set the atomic
              if( g_root[ _parent_id ].g_rootCircle_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle[ freeCellIndex ].g_root_Circle_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootCircle_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Circle_Shrink_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Circle_Grow_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Circle_Move_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Circle_ColourChanger_stopper( childIndex );
         }
         g_root_Circle[ _gid ].g_root_Circle_state = ::eg::action_stopped;
         g_root_Circle[ _gid ].g_root_Circle_cycle = clock::cycle();
         if( g_root_Circle[ _gid ].g_root_Circle_fiber.joinable() )
             g_root_Circle[ _gid ].g_root_Circle_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle[ _gid ].g_root_Circle_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > root_Circle_Shrink_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Circle[ _gid ].g_root_CircleShrink_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Circle_Shrink[ _gid * 1 + nextCellIndex ].g_root_Circle_Shrink_alloc_data;
         if( g_root_Circle_Shrink[ nextInstance ].g_root_Circle_Shrink_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleShrink_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >& reference = g_root_Circle_Shrink[ nextInstance ].g_root_Circle_Shrink_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle_Shrink[ nextInstance ].g_root_Circle_Shrink_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > nullInstance;
    return nullInstance;
}

void root_Circle_Shrink_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Circle_Shrink[ _gid ].g_root_Circle_Shrink_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Circle[ _parent_id ].g_root_CircleShrink_alloc_iter.load() );
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
              if( g_root_Circle[ _parent_id ].g_root_CircleShrink_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle_Shrink[ freeCellIndex ].g_root_Circle_Shrink_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Circle[ _parent_id ].g_root_CircleShrink_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Circle_Shrink[ _gid ].g_root_Circle_Shrink_state = ::eg::action_stopped;
         g_root_Circle_Shrink[ _gid ].g_root_Circle_Shrink_cycle = clock::cycle();
         if( g_root_Circle_Shrink[ _gid ].g_root_Circle_Shrink_fiber.joinable() )
             g_root_Circle_Shrink[ _gid ].g_root_Circle_Shrink_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle_Shrink[ _gid ].g_root_Circle_Shrink_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Circle< void >::__eg_Grow< void > root_Circle_Grow_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Circle[ _gid ].g_root_CircleGrow_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Circle_Grow[ _gid * 1 + nextCellIndex ].g_root_Circle_Grow_alloc_data;
         if( g_root_Circle_Grow[ nextInstance ].g_root_Circle_Grow_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleGrow_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >& reference = g_root_Circle_Grow[ nextInstance ].g_root_Circle_Grow_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle_Grow[ nextInstance ].g_root_Circle_Grow_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > nullInstance;
    return nullInstance;
}

void root_Circle_Grow_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Circle_Grow[ _gid ].g_root_Circle_Grow_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Circle[ _parent_id ].g_root_CircleGrow_alloc_iter.load() );
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
              if( g_root_Circle[ _parent_id ].g_root_CircleGrow_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle_Grow[ freeCellIndex ].g_root_Circle_Grow_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Circle[ _parent_id ].g_root_CircleGrow_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Circle_Grow[ _gid ].g_root_Circle_Grow_state = ::eg::action_stopped;
         g_root_Circle_Grow[ _gid ].g_root_Circle_Grow_cycle = clock::cycle();
         if( g_root_Circle_Grow[ _gid ].g_root_Circle_Grow_fiber.joinable() )
             g_root_Circle_Grow[ _gid ].g_root_Circle_Grow_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle_Grow[ _gid ].g_root_Circle_Grow_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Circle< void >::__eg_Move< void > root_Circle_Move_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Circle[ _gid ].g_root_CircleMove_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Circle_Move[ _gid * 1 + nextCellIndex ].g_root_Circle_Move_alloc_data;
         if( g_root_Circle_Move[ nextInstance ].g_root_Circle_Move_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleMove_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Circle< void >::__eg_Move< void >& reference = g_root_Circle_Move[ nextInstance ].g_root_Circle_Move_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle_Move[ nextInstance ].g_root_Circle_Move_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void > nullInstance;
    return nullInstance;
}

void root_Circle_Move_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Circle_Move[ _gid ].g_root_Circle_Move_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Circle[ _parent_id ].g_root_CircleMove_alloc_iter.load() );
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
              if( g_root_Circle[ _parent_id ].g_root_CircleMove_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle_Move[ freeCellIndex ].g_root_Circle_Move_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Circle[ _parent_id ].g_root_CircleMove_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Circle_Move[ _gid ].g_root_Circle_Move_state = ::eg::action_stopped;
         g_root_Circle_Move[ _gid ].g_root_Circle_Move_cycle = clock::cycle();
         if( g_root_Circle_Move[ _gid ].g_root_Circle_Move_fiber.joinable() )
             g_root_Circle_Move[ _gid ].g_root_Circle_Move_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle_Move[ _gid ].g_root_Circle_Move_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > root_Circle_ColourChanger_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Circle[ _gid ].g_root_CircleColourChanger_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Circle_ColourChanger[ _gid * 1 + nextCellIndex ].g_root_Circle_ColourChanger_alloc_data;
         if( g_root_Circle_ColourChanger[ nextInstance ].g_root_Circle_ColourChanger_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleColourChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >& reference = g_root_Circle_ColourChanger[ nextInstance ].g_root_Circle_ColourChanger_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle_ColourChanger[ nextInstance ].g_root_Circle_ColourChanger_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > nullInstance;
    return nullInstance;
}

void root_Circle_ColourChanger_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Circle_ColourChanger[ _gid ].g_root_Circle_ColourChanger_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Circle[ _parent_id ].g_root_CircleColourChanger_alloc_iter.load() );
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
              if( g_root_Circle[ _parent_id ].g_root_CircleColourChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle_ColourChanger[ freeCellIndex ].g_root_Circle_ColourChanger_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Circle[ _parent_id ].g_root_CircleColourChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Circle_ColourChanger[ _gid ].g_root_Circle_ColourChanger_state = ::eg::action_stopped;
         g_root_Circle_ColourChanger[ _gid ].g_root_Circle_ColourChanger_cycle = clock::cycle();
         if( g_root_Circle_ColourChanger[ _gid ].g_root_Circle_ColourChanger_fiber.joinable() )
             g_root_Circle_ColourChanger[ _gid ].g_root_Circle_ColourChanger_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle_ColourChanger[ _gid ].g_root_Circle_ColourChanger_reference, sizeof( eg::reference ) );
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
    __eg_type_path< lastPos >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void > context, __eg_root< void >::__eg_lastPos< void >::Write value )
    {
        g_root[ context.data.instance ].m_lastPos = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_Circle< void >, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Circle< void > operator()( __eg_root< void > context, Args... args )
    {
        __eg_root< void >::__eg_Circle< void > ref = root_Circle_starter( context.data.instance );
        if( ref )
        {
            g_root_Circle[ ref.data.instance ].g_root_Circle_fiber = boost::fibers::fiber
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
                    root_Circle_stopper( ref.data.instance );
                }
            );
            g_root_Circle[ ref.data.instance ].g_root_Circle_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_lastPos< void >::Read,
    __eg_root< void >,
    __eg_type_path< lastPos >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_lastPos< void >::Read operator()( __eg_root< void > context )
    {
        return g_root[ context.data.instance ].m_lastPos;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< position >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_position< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_position = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< color >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_color< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_color = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >,
    __eg_type_path< Stop >,
    Stop
>
{
    void operator()( __eg_root< void > context )
    {
        return root_stopper( context.data.instance );
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::EGRangeType,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_Circle< void >, Range >,
    Range
>
{
    __eg_root< void >::__eg_Circle< void >::EGRangeType operator()( __eg_root< void > context )
    {
        const eg::Instance iBegin = context.data.instance * 4096;
        const eg::Instance iEnd = ( context.data.instance + 1 ) * 4096;
        __eg_root< void >::__eg_Circle< void >::Iterator begin( iBegin - 1, iEnd, 81 ); //root::Circle
        ++begin;
        __eg_root< void >::__eg_Circle< void >::Iterator end( iEnd, iEnd, 81 );
        return __eg_root< void >::__eg_Circle< void >::EGRangeType( begin, end );
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< ColourChanger, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > operator()( __eg_root< void >::__eg_Circle< void > context, Args... args )
    {
        __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > ref = root_Circle_ColourChanger_starter( context.data.instance );
        if( ref )
        {
            g_root_Circle_ColourChanger[ ref.data.instance ].g_root_Circle_ColourChanger_fiber = boost::fibers::fiber
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
                    root_Circle_ColourChanger_stopper( ref.data.instance );
                }
            );
            g_root_Circle_ColourChanger[ ref.data.instance ].g_root_Circle_ColourChanger_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< ColourChanger, offset >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >::__eg_offset< void >::Write value )
    {
        g_root_Circle_ColourChanger[ context.data.instance ].m_offset = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< Shrink, Stop >,
    Stop
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return root_Circle_Shrink_stopper( context.data.instance );
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< Grow, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > operator()( __eg_root< void >::__eg_Circle< void > context, Args... args )
    {
        __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > ref = root_Circle_Grow_starter( context.data.instance );
        if( ref )
        {
            g_root_Circle_Grow[ ref.data.instance ].g_root_Circle_Grow_fiber = boost::fibers::fiber
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
                    root_Circle_Grow_stopper( ref.data.instance );
                }
            );
            g_root_Circle_Grow[ ref.data.instance ].g_root_Circle_Grow_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< Grow, Stop >,
    Stop
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return root_Circle_Grow_stopper( context.data.instance );
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< Shrink, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > operator()( __eg_root< void >::__eg_Circle< void > context, Args... args )
    {
        __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > ref = root_Circle_Shrink_starter( context.data.instance );
        if( ref )
        {
            g_root_Circle_Shrink[ ref.data.instance ].g_root_Circle_Shrink_fiber = boost::fibers::fiber
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
                    root_Circle_Shrink_stopper( ref.data.instance );
                }
            );
            g_root_Circle_Shrink[ ref.data.instance ].g_root_Circle_Shrink_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< Move, Start >,
    Start
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void > operator()( __eg_root< void >::__eg_Circle< void > context, Args... args )
    {
        __eg_root< void >::__eg_Circle< void >::__eg_Move< void > ref = root_Circle_Move_starter( context.data.instance );
        if( ref )
        {
            g_root_Circle_Move[ ref.data.instance ].g_root_Circle_Move_fiber = boost::fibers::fiber
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
                    root_Circle_Move_stopper( ref.data.instance );
                }
            );
            g_root_Circle_Move[ ref.data.instance ].g_root_Circle_Move_fiber.properties< eg::fiber_props >().setReference( ref.data );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >,
    __eg_type_path< direction >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void >::__eg_Move< void > context, __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_direction< void >::Write value )
    {
        g_root_Circle_Move[ context.data.instance ].m_direction = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< Move, speed >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_speed< void >::Write value )
    {
        g_root_Circle_Move[ context.data.instance ].m_speed = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< max_size >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_max_size = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< max_lifetime >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_max_lifetime = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< max_size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_max_size;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< size >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_size< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_size = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< starttime >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_starttime< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_starttime = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< max_lifetime >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_max_lifetime;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< lifetime >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void > context, __eg_root< void >::__eg_Circle< void >::__eg_lifetime< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_lifetime = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_lifetime< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< lifetime >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_lifetime< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_lifetime;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_color< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< color >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_color< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_color;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_position< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< position >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_position< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_position;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_size< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_size< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_size;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >,
    __eg_type_path< start >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > context, __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >::__eg_start< void >::Write value )
    {
        g_root_Circle_Shrink[ context.data.instance ].m_start = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > context )
    {
        return g_root_Circle_Shrink[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >,
    __eg_type_path< max_lifetime >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_max_lifetime;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >,
    __eg_type_path< max_size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_max_size;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >,
    __eg_type_path< size >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > context, __eg_root< void >::__eg_Circle< void >::__eg_size< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_size = value;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >,
    __eg_type_path< start >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > context, __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >::__eg_start< void >::Write value )
    {
        g_root_Circle_Grow[ context.data.instance ].m_start = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >::__eg_start< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >,
    __eg_type_path< start >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >::__eg_start< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > context )
    {
        return g_root_Circle_Grow[ context.data.instance ].m_start;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >,
    __eg_type_path< max_lifetime >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_max_lifetime< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_max_lifetime;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >,
    __eg_type_path< max_size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_max_size< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_max_size;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >,
    __eg_type_path< size >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > context, __eg_root< void >::__eg_Circle< void >::__eg_size< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_size = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_position< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >,
    __eg_type_path< position >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_position< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Move< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_position;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_direction< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >,
    __eg_type_path< direction >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_direction< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Move< void > context )
    {
        return g_root_Circle_Move[ context.data.instance ].m_direction;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_speed< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >,
    __eg_type_path< speed >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >::__eg_speed< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_Move< void > context )
    {
        return g_root_Circle_Move[ context.data.instance ].m_speed;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >::__eg_Move< void >,
    __eg_type_path< position >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void >::__eg_Move< void > context, __eg_root< void >::__eg_Circle< void >::__eg_position< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_position = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >::__eg_offset< void >::Read,
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >,
    __eg_type_path< offset >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >::__eg_offset< void >::Read operator()( __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > context )
    {
        return g_root_Circle_ColourChanger[ context.data.instance ].m_offset;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >,
    __eg_type_path< color >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > context, __eg_root< void >::__eg_Circle< void >::__eg_color< void >::Write value )
    {
        g_root_Circle[ context.data.instance ].m_color = value;
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
     case 75: //root
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
      case 75: //root
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


//root::Circle
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Circle< void >, __eg_root< void >::__eg_Circle< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Circle( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 81: //root::Circle
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
typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >& __eg_root< __eg1 >::__eg_Circle< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 81: //root::Circle
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Circle< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle[ instance ].g_root_Circle_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Circle< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle[ instance ].g_root_Circle_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Circle< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle[ instance ].g_root_Circle_state;
}


//root::Circle::Shrink
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >, __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Shrink< __eg3 >::__eg_Shrink( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 89: //root::Circle::Shrink
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
typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_Shrink< __eg3 >& __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Shrink< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 89: //root::Circle::Shrink
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_state;
}


//root::Circle::Grow
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >, __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Grow< __eg3 >::__eg_Grow( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 94: //root::Circle::Grow
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
typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_Grow< __eg3 >& __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Grow< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 94: //root::Circle::Grow
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Grow[ instance ].g_root_Circle_Grow_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Grow[ instance ].g_root_Circle_Grow_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Grow[ instance ].g_root_Circle_Grow_state;
}


//root::Circle::Move
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Circle< void >::__eg_Move< void >, __eg_root< void >::__eg_Circle< void >::__eg_Move< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Move< __eg3 >::__eg_Move( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_Move< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 99: //root::Circle::Move
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
typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_Move< __eg3 >& __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Move< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_Move< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 99: //root::Circle::Move
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Circle< void >::__eg_Move< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Move[ instance ].g_root_Circle_Move_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Circle< void >::__eg_Move< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Move[ instance ].g_root_Circle_Move_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Circle< void >::__eg_Move< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_Move[ instance ].g_root_Circle_Move_state;
}


//root::Circle::ColourChanger
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >, __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_ColourChanger< __eg3 >::__eg_ColourChanger( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 104: //root::Circle::ColourChanger
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
typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_ColourChanger< __eg3 >& __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_ColourChanger< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 104: //root::Circle::ColourChanger
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_state;
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

//root::Circle
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Circle< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Circle< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Circle< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Circle::Shrink
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_Shrink< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Shrink< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Shrink< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Shrink< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Circle::Grow
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_Grow< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Grow< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Grow< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Grow< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Circle::Move
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_Move< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Move< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Move< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_Move< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Circle::ColourChanger
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Circle< __eg2 >::template __eg_ColourChanger< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_ColourChanger< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_ColourChanger< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Circle< __eg2 >::__eg_ColourChanger< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}


