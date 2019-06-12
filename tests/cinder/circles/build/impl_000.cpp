#include "structures.hpp"


//input::Action function forward declarations

__eg_root< void > root_starter( eg::Instance _gid );
void root_stopper( eg::Instance _gid );

__eg_root< void >::__eg_StrokedCircle< void > root_StrokedCircle_starter( eg::Instance _gid );
void root_StrokedCircle_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Circle< void > root_Circle_starter( eg::Instance _gid );
void root_Circle_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Spiral< void > root_Spiral_starter( eg::Instance _gid );
void root_Spiral_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Attract< void > root_Attract_starter( eg::Instance _gid );
void root_Attract_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Randomise< void > root_Randomise_starter( eg::Instance _gid );
void root_Randomise_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Shape< void >::__eg_Default< void > root_StrokedCircle_Default_starter( eg::Instance _gid );
void root_StrokedCircle_Default_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > root_StrokedCircle_ColorChanger_starter( eg::Instance _gid );
void root_StrokedCircle_ColorChanger_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Shape< void >::__eg_Morph< void > root_StrokedCircle_Morph_starter( eg::Instance _gid );
void root_StrokedCircle_Morph_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Shape< void >::__eg_Default< void > root_Circle_Default_starter( eg::Instance _gid );
void root_Circle_Default_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > root_Circle_ColorChanger_starter( eg::Instance _gid );
void root_Circle_ColorChanger_stopper( eg::Instance _gid );

__eg_root< void >::__eg_Shape< void >::__eg_Morph< void > root_Circle_Morph_starter( eg::Instance _gid );
void root_Circle_Morph_stopper( eg::Instance _gid );

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
         for( eg::Instance childIndex = _gid * 256; childIndex != ( _gid + 1 ) * 256; ++childIndex )
         {
             root_StrokedCircle_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 256; childIndex != ( _gid + 1 ) * 256; ++childIndex )
         {
             root_Circle_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Spiral_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Attract_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Randomise_stopper( childIndex );
         }
         g_root[ _gid ].g_root_state = ::eg::action_stopped;
         g_root[ _gid ].g_root_cycle = clock::cycle();
         if( g_root[ _gid ].g_root_fiber.joinable() )
             g_root[ _gid ].g_root_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root[ _gid ].g_root_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_StrokedCircle< void > root_StrokedCircle_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootStrokedCircle_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         eg::Instance nextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 255 )
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
         eg::Instance nextInstance = g_root_StrokedCircle[ _gid * 256 + nextCellIndex ].g_root_StrokedCircle_alloc_data;
         if( g_root_StrokedCircle[ nextInstance ].g_root_StrokedCircle_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootStrokedCircle_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_StrokedCircle< void >& reference = g_root_StrokedCircle[ nextInstance ].g_root_StrokedCircle_reference;
                 reference.data.timestamp = startCycle;
                 g_root_StrokedCircle[ nextInstance ].g_root_StrokedCircle_state = ::eg::action_running;
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
    __eg_root< void >::__eg_StrokedCircle< void > nullInstance;
    return nullInstance;
}

void root_StrokedCircle_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 256;
     if( g_root_StrokedCircle[ _gid ].g_root_StrokedCircle_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootStrokedCircle_alloc_iter.load() );
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
              if( static_cast< eg::Instance >( iter.tail ) == 256 - 1U )
              {
                  iter.tail = 0U;
              }
              else
              {
                  ++iter.tail;
              }
              //attempt to set the atomic
              if( g_root[ _parent_id ].g_rootStrokedCircle_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_StrokedCircle[ freeCellIndex ].g_root_StrokedCircle_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootStrokedCircle_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_StrokedCircle_Default_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_StrokedCircle_ColorChanger_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_StrokedCircle_Morph_stopper( childIndex );
         }
         g_root_StrokedCircle[ _gid ].g_root_StrokedCircle_state = ::eg::action_stopped;
         g_root_StrokedCircle[ _gid ].g_root_StrokedCircle_cycle = clock::cycle();
         if( g_root_StrokedCircle[ _gid ].g_root_StrokedCircle_fiber.joinable() )
             g_root_StrokedCircle[ _gid ].g_root_StrokedCircle_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_StrokedCircle[ _gid ].g_root_StrokedCircle_reference, sizeof( eg::reference ) );
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
         if( nextCellIndex == 255 )
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
         eg::Instance nextInstance = g_root_Circle[ _gid * 256 + nextCellIndex ].g_root_Circle_alloc_data;
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
     eg::Instance _parent_id = _gid / 256;
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
              if( static_cast< eg::Instance >( iter.tail ) == 256 - 1U )
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
             root_Circle_Default_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Circle_ColorChanger_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Circle_Morph_stopper( childIndex );
         }
         g_root_Circle[ _gid ].g_root_Circle_state = ::eg::action_stopped;
         g_root_Circle[ _gid ].g_root_Circle_cycle = clock::cycle();
         if( g_root_Circle[ _gid ].g_root_Circle_fiber.joinable() )
             g_root_Circle[ _gid ].g_root_Circle_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle[ _gid ].g_root_Circle_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Spiral< void > root_Spiral_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootSpiral_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Spiral[ _gid * 1 + nextCellIndex ].g_root_Spiral_alloc_data;
         if( g_root_Spiral[ nextInstance ].g_root_Spiral_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootSpiral_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Spiral< void >& reference = g_root_Spiral[ nextInstance ].g_root_Spiral_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Spiral[ nextInstance ].g_root_Spiral_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Spiral< void > nullInstance;
    return nullInstance;
}

void root_Spiral_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Spiral[ _gid ].g_root_Spiral_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootSpiral_alloc_iter.load() );
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
              if( g_root[ _parent_id ].g_rootSpiral_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Spiral[ freeCellIndex ].g_root_Spiral_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootSpiral_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Spiral[ _gid ].g_root_Spiral_state = ::eg::action_stopped;
         g_root_Spiral[ _gid ].g_root_Spiral_cycle = clock::cycle();
         if( g_root_Spiral[ _gid ].g_root_Spiral_fiber.joinable() )
             g_root_Spiral[ _gid ].g_root_Spiral_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Spiral[ _gid ].g_root_Spiral_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Attract< void > root_Attract_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootAttract_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Attract[ _gid * 1 + nextCellIndex ].g_root_Attract_alloc_data;
         if( g_root_Attract[ nextInstance ].g_root_Attract_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootAttract_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Attract< void >& reference = g_root_Attract[ nextInstance ].g_root_Attract_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Attract[ nextInstance ].g_root_Attract_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Attract< void > nullInstance;
    return nullInstance;
}

void root_Attract_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Attract[ _gid ].g_root_Attract_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootAttract_alloc_iter.load() );
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
              if( g_root[ _parent_id ].g_rootAttract_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Attract[ freeCellIndex ].g_root_Attract_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootAttract_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Attract[ _gid ].g_root_Attract_state = ::eg::action_stopped;
         g_root_Attract[ _gid ].g_root_Attract_cycle = clock::cycle();
         if( g_root_Attract[ _gid ].g_root_Attract_fiber.joinable() )
             g_root_Attract[ _gid ].g_root_Attract_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Attract[ _gid ].g_root_Attract_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Randomise< void > root_Randomise_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _gid ].g_rootRandomise_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Randomise[ _gid * 1 + nextCellIndex ].g_root_Randomise_alloc_data;
         if( g_root_Randomise[ nextInstance ].g_root_Randomise_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootRandomise_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Randomise< void >& reference = g_root_Randomise[ nextInstance ].g_root_Randomise_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Randomise[ nextInstance ].g_root_Randomise_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Randomise< void > nullInstance;
    return nullInstance;
}

void root_Randomise_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Randomise[ _gid ].g_root_Randomise_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootRandomise_alloc_iter.load() );
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
              if( g_root[ _parent_id ].g_rootRandomise_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Randomise[ freeCellIndex ].g_root_Randomise_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootRandomise_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Randomise[ _gid ].g_root_Randomise_state = ::eg::action_stopped;
         g_root_Randomise[ _gid ].g_root_Randomise_cycle = clock::cycle();
         if( g_root_Randomise[ _gid ].g_root_Randomise_fiber.joinable() )
             g_root_Randomise[ _gid ].g_root_Randomise_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Randomise[ _gid ].g_root_Randomise_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Shape< void >::__eg_Default< void > root_StrokedCircle_Default_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_StrokedCircle[ _gid ].g_root_StrokedCircleDefault_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_StrokedCircle_Default[ _gid * 1 + nextCellIndex ].g_root_StrokedCircle_Default_alloc_data;
         if( g_root_StrokedCircle_Default[ nextInstance ].g_root_StrokedCircle_Default_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_StrokedCircle[ _gid ].g_root_StrokedCircleDefault_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Shape< void >::__eg_Default< void >& reference = g_root_StrokedCircle_Default[ nextInstance ].g_root_StrokedCircle_Default_reference;
                 reference.data.timestamp = startCycle;
                 g_root_StrokedCircle_Default[ nextInstance ].g_root_StrokedCircle_Default_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Shape< void >::__eg_Default< void > nullInstance;
    return nullInstance;
}

void root_StrokedCircle_Default_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_StrokedCircle_Default[ _gid ].g_root_StrokedCircle_Default_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleDefault_alloc_iter.load() );
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
              if( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleDefault_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_StrokedCircle_Default[ freeCellIndex ].g_root_StrokedCircle_Default_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleDefault_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_StrokedCircle_Default[ _gid ].g_root_StrokedCircle_Default_state = ::eg::action_stopped;
         g_root_StrokedCircle_Default[ _gid ].g_root_StrokedCircle_Default_cycle = clock::cycle();
         if( g_root_StrokedCircle_Default[ _gid ].g_root_StrokedCircle_Default_fiber.joinable() )
             g_root_StrokedCircle_Default[ _gid ].g_root_StrokedCircle_Default_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_StrokedCircle_Default[ _gid ].g_root_StrokedCircle_Default_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > root_StrokedCircle_ColorChanger_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_StrokedCircle[ _gid ].g_root_StrokedCircleColorChanger_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_StrokedCircle_ColorChanger[ _gid * 1 + nextCellIndex ].g_root_StrokedCircle_ColorChanger_alloc_data;
         if( g_root_StrokedCircle_ColorChanger[ nextInstance ].g_root_StrokedCircle_ColorChanger_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_StrokedCircle[ _gid ].g_root_StrokedCircleColorChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >& reference = g_root_StrokedCircle_ColorChanger[ nextInstance ].g_root_StrokedCircle_ColorChanger_reference;
                 reference.data.timestamp = startCycle;
                 g_root_StrokedCircle_ColorChanger[ nextInstance ].g_root_StrokedCircle_ColorChanger_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > nullInstance;
    return nullInstance;
}

void root_StrokedCircle_ColorChanger_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_StrokedCircle_ColorChanger[ _gid ].g_root_StrokedCircle_ColorChanger_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleColorChanger_alloc_iter.load() );
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
              if( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleColorChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_StrokedCircle_ColorChanger[ freeCellIndex ].g_root_StrokedCircle_ColorChanger_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleColorChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_StrokedCircle_ColorChanger[ _gid ].g_root_StrokedCircle_ColorChanger_state = ::eg::action_stopped;
         g_root_StrokedCircle_ColorChanger[ _gid ].g_root_StrokedCircle_ColorChanger_cycle = clock::cycle();
         if( g_root_StrokedCircle_ColorChanger[ _gid ].g_root_StrokedCircle_ColorChanger_fiber.joinable() )
             g_root_StrokedCircle_ColorChanger[ _gid ].g_root_StrokedCircle_ColorChanger_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_StrokedCircle_ColorChanger[ _gid ].g_root_StrokedCircle_ColorChanger_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Shape< void >::__eg_Morph< void > root_StrokedCircle_Morph_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_StrokedCircle[ _gid ].g_root_StrokedCircleMorph_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_StrokedCircle_Morph[ _gid * 1 + nextCellIndex ].g_root_StrokedCircle_Morph_alloc_data;
         if( g_root_StrokedCircle_Morph[ nextInstance ].g_root_StrokedCircle_Morph_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_StrokedCircle[ _gid ].g_root_StrokedCircleMorph_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >& reference = g_root_StrokedCircle_Morph[ nextInstance ].g_root_StrokedCircle_Morph_reference;
                 reference.data.timestamp = startCycle;
                 g_root_StrokedCircle_Morph[ nextInstance ].g_root_StrokedCircle_Morph_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > nullInstance;
    return nullInstance;
}

void root_StrokedCircle_Morph_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_StrokedCircle_Morph[ _gid ].g_root_StrokedCircle_Morph_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleMorph_alloc_iter.load() );
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
              if( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleMorph_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_StrokedCircle_Morph[ freeCellIndex ].g_root_StrokedCircle_Morph_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_StrokedCircle[ _parent_id ].g_root_StrokedCircleMorph_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_StrokedCircle_Morph[ _gid ].g_root_StrokedCircle_Morph_state = ::eg::action_stopped;
         g_root_StrokedCircle_Morph[ _gid ].g_root_StrokedCircle_Morph_cycle = clock::cycle();
         if( g_root_StrokedCircle_Morph[ _gid ].g_root_StrokedCircle_Morph_fiber.joinable() )
             g_root_StrokedCircle_Morph[ _gid ].g_root_StrokedCircle_Morph_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_StrokedCircle_Morph[ _gid ].g_root_StrokedCircle_Morph_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Shape< void >::__eg_Default< void > root_Circle_Default_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Circle[ _gid ].g_root_CircleDefault_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Circle_Default[ _gid * 1 + nextCellIndex ].g_root_Circle_Default_alloc_data;
         if( g_root_Circle_Default[ nextInstance ].g_root_Circle_Default_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleDefault_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Shape< void >::__eg_Default< void >& reference = g_root_Circle_Default[ nextInstance ].g_root_Circle_Default_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle_Default[ nextInstance ].g_root_Circle_Default_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Shape< void >::__eg_Default< void > nullInstance;
    return nullInstance;
}

void root_Circle_Default_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Circle_Default[ _gid ].g_root_Circle_Default_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Circle[ _parent_id ].g_root_CircleDefault_alloc_iter.load() );
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
              if( g_root_Circle[ _parent_id ].g_root_CircleDefault_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle_Default[ freeCellIndex ].g_root_Circle_Default_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Circle[ _parent_id ].g_root_CircleDefault_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Circle_Default[ _gid ].g_root_Circle_Default_state = ::eg::action_stopped;
         g_root_Circle_Default[ _gid ].g_root_Circle_Default_cycle = clock::cycle();
         if( g_root_Circle_Default[ _gid ].g_root_Circle_Default_fiber.joinable() )
             g_root_Circle_Default[ _gid ].g_root_Circle_Default_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle_Default[ _gid ].g_root_Circle_Default_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > root_Circle_ColorChanger_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Circle[ _gid ].g_root_CircleColorChanger_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Circle_ColorChanger[ _gid * 1 + nextCellIndex ].g_root_Circle_ColorChanger_alloc_data;
         if( g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleColorChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >& reference = g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > nullInstance;
    return nullInstance;
}

void root_Circle_ColorChanger_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Circle[ _parent_id ].g_root_CircleColorChanger_alloc_iter.load() );
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
              if( g_root_Circle[ _parent_id ].g_root_CircleColorChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle_ColorChanger[ freeCellIndex ].g_root_Circle_ColorChanger_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Circle[ _parent_id ].g_root_CircleColorChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_state = ::eg::action_stopped;
         g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_cycle = clock::cycle();
         if( g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_fiber.joinable() )
             g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Shape< void >::__eg_Morph< void > root_Circle_Morph_starter( eg::Instance _gid )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Circle[ _gid ].g_root_CircleMorph_alloc_iter.load() );
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
         eg::Instance nextInstance = g_root_Circle_Morph[ _gid * 1 + nextCellIndex ].g_root_Circle_Morph_alloc_data;
         if( g_root_Circle_Morph[ nextInstance ].g_root_Circle_Morph_cycle < clock::cycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleMorph_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const eg::Instance startCycle = clock::cycle();
                 __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >& reference = g_root_Circle_Morph[ nextInstance ].g_root_Circle_Morph_reference;
                 reference.data.timestamp = startCycle;
                 g_root_Circle_Morph[ nextInstance ].g_root_Circle_Morph_state = ::eg::action_running;
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
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > nullInstance;
    return nullInstance;
}

void root_Circle_Morph_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Circle_Morph[ _gid ].g_root_Circle_Morph_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Circle[ _parent_id ].g_root_CircleMorph_alloc_iter.load() );
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
              if( g_root_Circle[ _parent_id ].g_root_CircleMorph_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Circle_Morph[ freeCellIndex ].g_root_Circle_Morph_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Circle[ _parent_id ].g_root_CircleMorph_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Circle_Morph[ _gid ].g_root_Circle_Morph_state = ::eg::action_stopped;
         g_root_Circle_Morph[ _gid ].g_root_Circle_Morph_cycle = clock::cycle();
         if( g_root_Circle_Morph[ _gid ].g_root_Circle_Morph_fiber.joinable() )
             g_root_Circle_Morph[ _gid ].g_root_Circle_Morph_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Circle_Morph[ _gid ].g_root_Circle_Morph_reference, sizeof( eg::reference ) );
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
    __eg_root< void >::__eg_Randomise< void >,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_Randomise< void > >,
    __eg_ImpNoParams
>
{
    template< typename... Args >
    __eg_root< void >::__eg_Randomise< void > operator()( __eg_root< void > context, Args... args )
    {
        __eg_root< void >::__eg_Randomise< void > ref = root_Randomise_starter( context.data.instance );
        if( ref )
        {
            ref( args... );
            root_Randomise_stopper( ref.data.instance );
        }
        return ref;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >::__eg_Default< void >,
    __eg_type_path< color >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void >::__eg_Default< void > context, __eg_root< void >::__eg_Shape< void >::__eg_color< void >::Write value )
    {
        switch( context.data.type )
        {
            case 117: //root::Shape::Default
            {
                g_root_StrokedCircle[ context.data.instance ].m_color = value;
            }
            case 149: //root::Shape::Default
            {
                g_root_Circle[ context.data.instance ].m_color = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >::__eg_Default< void >,
    __eg_type_path< position >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void >::__eg_Default< void > context, __eg_root< void >::__eg_Shape< void >::__eg_position< void >::Write value )
    {
        switch( context.data.type )
        {
            case 117: //root::Shape::Default
            {
                g_root_StrokedCircle[ context.data.instance ].m_position = value;
            }
            case 149: //root::Shape::Default
            {
                g_root_Circle[ context.data.instance ].m_position = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >::__eg_Default< void >,
    __eg_type_path< size >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void >::__eg_Default< void > context, __eg_root< void >::__eg_Shape< void >::__eg_size< void >::Write value )
    {
        switch( context.data.type )
        {
            case 117: //root::Shape::Default
            {
                g_root_StrokedCircle[ context.data.instance ].m_size = value;
            }
            case 149: //root::Shape::Default
            {
                g_root_Circle[ context.data.instance ].m_size = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::__eg_f< void >::Read,
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >,
    __eg_type_path< f >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::__eg_f< void >::Read operator()( __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > context )
    {
        switch( context.data.type )
        {
            case 122: //root::Shape::ColorChanger
            {
                return g_root_StrokedCircle_ColorChanger[ context.data.instance ].m_f;
            }
            case 154: //root::Shape::ColorChanger
            {
                return g_root_Circle_ColorChanger[ context.data.instance ].m_f;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >,
    __eg_type_path< f >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > context, __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::__eg_f< void >::Write value )
    {
        switch( context.data.type )
        {
            case 122: //root::Shape::ColorChanger
            {
                g_root_StrokedCircle_ColorChanger[ context.data.instance ].m_f = value;
            }
            case 154: //root::Shape::ColorChanger
            {
                g_root_Circle_ColorChanger[ context.data.instance ].m_f = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >,
    __eg_type_path< color >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > context, __eg_root< void >::__eg_Shape< void >::__eg_color< void >::Write value )
    {
        switch( context.data.type )
        {
            case 122: //root::Shape::ColorChanger
            {
                g_root_StrokedCircle[ context.data.instance ].m_color = value;
            }
            case 154: //root::Shape::ColorChanger
            {
                g_root_Circle[ context.data.instance ].m_color = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >::Read,
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >,
    __eg_type_path< positionActual >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >::Read operator()( __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > context )
    {
        switch( context.data.type )
        {
            case 127: //root::Shape::Morph
            {
                return g_root_StrokedCircle[ context.data.instance ].m_positionActual;
            }
            case 159: //root::Shape::Morph
            {
                return g_root_Circle[ context.data.instance ].m_positionActual;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_position< void >::Read,
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >,
    __eg_type_path< position >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_position< void >::Read operator()( __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > context )
    {
        switch( context.data.type )
        {
            case 127: //root::Shape::Morph
            {
                return g_root_StrokedCircle[ context.data.instance ].m_position;
            }
            case 159: //root::Shape::Morph
            {
                return g_root_Circle[ context.data.instance ].m_position;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >::__eg_speed< void >::Read,
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >,
    __eg_type_path< speed >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >::__eg_speed< void >::Read operator()( __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > context )
    {
        switch( context.data.type )
        {
            case 127: //root::Shape::Morph
            {
                return g_root_StrokedCircle_Morph[ context.data.instance ].m_speed;
            }
            case 159: //root::Shape::Morph
            {
                return g_root_Circle_Morph[ context.data.instance ].m_speed;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >,
    __eg_type_path< positionActual >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > context, __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >::Write value )
    {
        switch( context.data.type )
        {
            case 127: //root::Shape::Morph
            {
                g_root_StrokedCircle[ context.data.instance ].m_positionActual = value;
            }
            case 159: //root::Shape::Morph
            {
                g_root_Circle[ context.data.instance ].m_positionActual = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_color< void >::Read,
    __eg_root< void >::__eg_StrokedCircle< void >,
    __eg_type_path< color >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_color< void >::Read operator()( __eg_root< void >::__eg_StrokedCircle< void > context )
    {
        return g_root_StrokedCircle[ context.data.instance ].m_color;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >::Read,
    __eg_root< void >::__eg_StrokedCircle< void >,
    __eg_type_path< positionActual >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >::Read operator()( __eg_root< void >::__eg_StrokedCircle< void > context )
    {
        return g_root_StrokedCircle[ context.data.instance ].m_positionActual;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_size< void >::Read,
    __eg_root< void >::__eg_StrokedCircle< void >,
    __eg_type_path< size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_size< void >::Read operator()( __eg_root< void >::__eg_StrokedCircle< void > context )
    {
        return g_root_StrokedCircle[ context.data.instance ].m_size;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_color< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< color >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_color< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_color;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< positionActual >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_positionActual< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_positionActual;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_size< void >::Read,
    __eg_root< void >::__eg_Circle< void >,
    __eg_type_path< size >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_size< void >::Read operator()( __eg_root< void >::__eg_Circle< void > context )
    {
        return g_root_Circle[ context.data.instance ].m_size;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Spiral< void >,
    __eg_type_path< relative >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Spiral< void > context, __eg_root< void >::__eg_Spiral< void >::__eg_relative< void >::Write value )
    {
        g_root_Spiral[ context.data.instance ].m_relative = value;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >,
    __eg_root< void >::__eg_Spiral< void >,
    __eg_type_path< __eg_root< void >, Get >,
    Get
>
{
    __eg_root< void > operator()( __eg_root< void >::__eg_Spiral< void > context )
    {
        return g_root[ context.data.instance ].g_root_reference;
    }
};

template<>
struct __invoke_impl
<
    __eg_Range< __eg_MultiIterator< __eg_root< void >::__eg_Shape< void >, 2U > >,
    __eg_root< void >,
    __eg_type_path< __eg_root< void >::__eg_Shape< void >, Range >,
    Range
>
{
    __eg_Range< __eg_MultiIterator< __eg_root< void >::__eg_Shape< void >, 2U > > operator()( __eg_root< void > context )
    {
        using IterType = typename __eg_root< void >::__eg_Shape< void >::Iterator;
        using MultiIterType = __eg_MultiIterator< __eg_root< void >::__eg_Shape< void >, 2 >;
        MultiIterType::IteratorArray iterators_begin = 
        {
            IterType( ( context.data.instance * 256 ) - 1U, ( context.data.instance + 1 ) * 256, static_cast< eg::TypeID >( 90 ) ), //root::StrokedCircle
            IterType( ( context.data.instance * 256 ) - 1U, ( context.data.instance + 1 ) * 256, static_cast< eg::TypeID >( 95 ) ), //root::Circle
        };
        ++iterators_begin[ 0 ];
        ++iterators_begin[ 1 ];
        MultiIterType::IteratorArray iterators_end = 
        {
            IterType( ( context.data.instance + 1 ) * 256, ( context.data.instance + 1 ) * 256, static_cast< eg::TypeID >( 90 ) ), //root::StrokedCircle
            IterType( ( context.data.instance + 1 ) * 256, ( context.data.instance + 1 ) * 256, static_cast< eg::TypeID >( 95 ) ), //root::Circle
        };
        return __eg_Range< MultiIterType >( MultiIterType( iterators_begin ), MultiIterType( iterators_end ) );
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Spiral< void >::__eg_speed< void >::Read,
    __eg_root< void >::__eg_Spiral< void >,
    __eg_type_path< speed >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Spiral< void >::__eg_speed< void >::Read operator()( __eg_root< void >::__eg_Spiral< void > context )
    {
        return g_root_Spiral[ context.data.instance ].m_speed;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Spiral< void >::__eg_relative< void >::Read,
    __eg_root< void >::__eg_Spiral< void >,
    __eg_type_path< relative >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Spiral< void >::__eg_relative< void >::Read operator()( __eg_root< void >::__eg_Spiral< void > context )
    {
        return g_root_Spiral[ context.data.instance ].m_relative;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Spiral< void >::__eg_radius< void >::Read,
    __eg_root< void >::__eg_Spiral< void >,
    __eg_type_path< radius >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Spiral< void >::__eg_radius< void >::Read operator()( __eg_root< void >::__eg_Spiral< void > context )
    {
        return g_root_Spiral[ context.data.instance ].m_radius;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Spiral< void >::__eg_curve< void >::Read,
    __eg_root< void >::__eg_Spiral< void >,
    __eg_type_path< curve >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Spiral< void >::__eg_curve< void >::Read operator()( __eg_root< void >::__eg_Spiral< void > context )
    {
        return g_root_Spiral[ context.data.instance ].m_curve;
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >,
    __eg_type_path< position >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void > context, __eg_root< void >::__eg_Shape< void >::__eg_position< void >::Write value )
    {
        switch( context.data.type )
        {
            case 90: //root::StrokedCircle
            {
                g_root_StrokedCircle[ context.data.instance ].m_position = value;
            }
            case 95: //root::Circle
            {
                g_root_Circle[ context.data.instance ].m_position = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    void,
    __eg_root< void >::__eg_Shape< void >,
    __eg_type_path< f >,
    __eg_ImpParams
>
{
    void operator()( __eg_root< void >::__eg_Shape< void > context, __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::__eg_f< void >::Write value )
    {
        switch( context.data.type )
        {
            case 90: //root::StrokedCircle
            {
                g_root_StrokedCircle_ColorChanger[ context.data.instance ].m_f = value;
            }
            case 95: //root::Circle
            {
                g_root_Circle_ColorChanger[ context.data.instance ].m_f = value;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >,
    __eg_root< void >::__eg_Attract< void >,
    __eg_type_path< __eg_root< void >, Get >,
    Get
>
{
    __eg_root< void > operator()( __eg_root< void >::__eg_Attract< void > context )
    {
        return g_root[ context.data.instance ].g_root_reference;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Shape< void >::__eg_position< void >::Read,
    __eg_root< void >::__eg_Shape< void >,
    __eg_type_path< position >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Shape< void >::__eg_position< void >::Read operator()( __eg_root< void >::__eg_Shape< void > context )
    {
        switch( context.data.type )
        {
            case 90: //root::StrokedCircle
            {
                return g_root_StrokedCircle[ context.data.instance ].m_position;
            }
            case 95: //root::Circle
            {
                return g_root_Circle[ context.data.instance ].m_position;
            }
        }
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >::__eg_Randomise< void >::__eg_range< void >::Read,
    __eg_root< void >::__eg_Randomise< void >,
    __eg_type_path< range >,
    __eg_ImpNoParams
>
{
    __eg_root< void >::__eg_Randomise< void >::__eg_range< void >::Read operator()( __eg_root< void >::__eg_Randomise< void > context )
    {
        return g_root_Randomise[ context.data.instance ].m_range;
    }
};

template<>
struct __invoke_impl
<
    __eg_root< void >,
    __eg_root< void >::__eg_Randomise< void >,
    __eg_type_path< __eg_root< void >, Get >,
    Get
>
{
    __eg_root< void > operator()( __eg_root< void >::__eg_Randomise< void > context )
    {
        return g_root[ context.data.instance ].g_root_reference;
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
     case 84: //root
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
      case 84: //root
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


//root::Shape
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Shape< void >, __eg_root< void >::__eg_Shape< void > >
{
    static constexpr const bool value = true;
};
template<>
struct eg::is_convertible< __eg_root< void >::__eg_StrokedCircle< void >, __eg_root< void >::__eg_Shape< void > >
{
    static constexpr const bool value = true;
};
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Circle< void >, __eg_root< void >::__eg_Shape< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Shape( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 90: //root::StrokedCircle
     case 95: //root::Circle
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
typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >& __eg_root< __eg1 >::__eg_Shape< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 90: //root::StrokedCircle
      case 95: //root::Circle
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Shape< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 90: //root::StrokedCircle
         return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_reference.data.timestamp;
      case 95: //root::Circle
         return g_root_Circle[ instance ].g_root_Circle_reference.data.timestamp;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Shape< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 90: //root::StrokedCircle
         return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_cycle;
      case 95: //root::Circle
         return g_root_Circle[ instance ].g_root_Circle_cycle;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Shape< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 90: //root::StrokedCircle
         return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_state;
      case 95: //root::Circle
         return g_root_Circle[ instance ].g_root_Circle_state;
      default: return eg::TOTAL_ACTION_STATES;
    }
}


//root::Shape::Default
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Shape< void >::__eg_Default< void >, __eg_root< void >::__eg_Shape< void >::__eg_Default< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Default< __eg3 >::__eg_Default( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void >::__eg_Default< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 117: //root::Shape::Default
     case 149: //root::Shape::Default
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
typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >::template __eg_Default< __eg3 >& __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Default< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void >::__eg_Default< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 117: //root::Shape::Default
      case 149: //root::Shape::Default
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Shape< void >::__eg_Default< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 117: //root::Shape::Default
         return g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_reference.data.timestamp;
      case 149: //root::Shape::Default
         return g_root_Circle_Default[ instance ].g_root_Circle_Default_reference.data.timestamp;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Shape< void >::__eg_Default< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 117: //root::Shape::Default
         return g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_cycle;
      case 149: //root::Shape::Default
         return g_root_Circle_Default[ instance ].g_root_Circle_Default_cycle;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Shape< void >::__eg_Default< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 117: //root::Shape::Default
         return g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_state;
      case 149: //root::Shape::Default
         return g_root_Circle_Default[ instance ].g_root_Circle_Default_state;
      default: return eg::TOTAL_ACTION_STATES;
    }
}


//root::Shape::ColorChanger
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >, __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_ColorChanger< __eg3 >::__eg_ColorChanger( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 122: //root::Shape::ColorChanger
     case 154: //root::Shape::ColorChanger
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
typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >::template __eg_ColorChanger< __eg3 >& __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_ColorChanger< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 122: //root::Shape::ColorChanger
      case 154: //root::Shape::ColorChanger
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 122: //root::Shape::ColorChanger
         return g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_reference.data.timestamp;
      case 154: //root::Shape::ColorChanger
         return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_reference.data.timestamp;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 122: //root::Shape::ColorChanger
         return g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_cycle;
      case 154: //root::Shape::ColorChanger
         return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_cycle;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 122: //root::Shape::ColorChanger
         return g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_state;
      case 154: //root::Shape::ColorChanger
         return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_state;
      default: return eg::TOTAL_ACTION_STATES;
    }
}


//root::Shape::Morph
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >, __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Morph< __eg3 >::__eg_Morph( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 127: //root::Shape::Morph
     case 159: //root::Shape::Morph
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
typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >::template __eg_Morph< __eg3 >& __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Morph< __eg3 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 127: //root::Shape::Morph
      case 159: //root::Shape::Morph
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 127: //root::Shape::Morph
         return g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_reference.data.timestamp;
      case 159: //root::Shape::Morph
         return g_root_Circle_Morph[ instance ].g_root_Circle_Morph_reference.data.timestamp;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 127: //root::Shape::Morph
         return g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_cycle;
      case 159: //root::Shape::Morph
         return g_root_Circle_Morph[ instance ].g_root_Circle_Morph_cycle;
      default: return eg::INVALID_TIMESTAMP;
    }
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > >( eg::TypeID type, eg::Instance instance )
{
    switch( type )
    {
      case 127: //root::Shape::Morph
         return g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_state;
      case 159: //root::Shape::Morph
         return g_root_Circle_Morph[ instance ].g_root_Circle_Morph_state;
      default: return eg::TOTAL_ACTION_STATES;
    }
}


//root::StrokedCircle
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Shape< void >, __eg_root< void >::__eg_StrokedCircle< void > >
{
    static constexpr const bool value = true;
};
template<>
struct eg::is_convertible< __eg_root< void >::__eg_StrokedCircle< void >, __eg_root< void >::__eg_StrokedCircle< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_StrokedCircle< __eg2 >::__eg_StrokedCircle( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_StrokedCircle< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 90: //root::StrokedCircle
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
typename __eg_root< __eg1 >::template __eg_StrokedCircle< __eg2 >& __eg_root< __eg1 >::__eg_StrokedCircle< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_StrokedCircle< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 90: //root::StrokedCircle
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_StrokedCircle< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_StrokedCircle< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_StrokedCircle< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_state;
}


//root::Circle
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Shape< void >, __eg_root< void >::__eg_Circle< void > >
{
    static constexpr const bool value = true;
};
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
     case 95: //root::Circle
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
      case 95: //root::Circle
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


//root::Spiral
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Spiral< void >, __eg_root< void >::__eg_Spiral< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Spiral< __eg2 >::__eg_Spiral( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Spiral< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 100: //root::Spiral
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
typename __eg_root< __eg1 >::template __eg_Spiral< __eg2 >& __eg_root< __eg1 >::__eg_Spiral< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Spiral< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 100: //root::Spiral
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Spiral< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Spiral[ instance ].g_root_Spiral_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Spiral< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Spiral[ instance ].g_root_Spiral_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Spiral< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Spiral[ instance ].g_root_Spiral_state;
}


//root::Attract
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Attract< void >, __eg_root< void >::__eg_Attract< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Attract< __eg2 >::__eg_Attract( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Attract< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 105: //root::Attract
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
typename __eg_root< __eg1 >::template __eg_Attract< __eg2 >& __eg_root< __eg1 >::__eg_Attract< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Attract< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 105: //root::Attract
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Attract< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Attract[ instance ].g_root_Attract_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Attract< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Attract[ instance ].g_root_Attract_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Attract< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Attract[ instance ].g_root_Attract_state;
}


//root::Randomise
template<>
struct eg::is_convertible< __eg_root< void >::__eg_Randomise< void >, __eg_root< void >::__eg_Randomise< void > >
{
    static constexpr const bool value = true;
};
template< typename __eg1 >
template< typename __eg2 >
template< typename TFrom >
__eg_root< __eg1 >::__eg_Randomise< __eg2 >::__eg_Randomise( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Randomise< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 110: //root::Randomise
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
typename __eg_root< __eg1 >::template __eg_Randomise< __eg2 >& __eg_root< __eg1 >::__eg_Randomise< __eg2 >::operator=( const TFrom& from )
{
  static_assert( eg::is_convertible< TFrom, __eg_root< void >::__eg_Randomise< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 110: //root::Randomise
         data = from.data;
         break;
     default:
         data.timestamp = eg::INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template<>
inline eg::TimeStamp getTimestamp< __eg_root< void >::__eg_Randomise< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Randomise[ instance ].g_root_Randomise_reference.data.timestamp;
}

template<>
inline eg::TimeStamp getStopCycle< __eg_root< void >::__eg_Randomise< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Randomise[ instance ].g_root_Randomise_cycle;
}

template<>
inline eg::ActionState getState< __eg_root< void >::__eg_Randomise< void > >( eg::TypeID type, eg::Instance instance )
{
    return g_root_Randomise[ instance ].g_root_Randomise_state;
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

//root::Shape
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Shape< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Shape< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Shape< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Shape::Default
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >::template __eg_Default< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Default< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Default< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Default< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Shape::ColorChanger
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >::template __eg_ColorChanger< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_ColorChanger< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_ColorChanger< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_ColorChanger< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Shape::Morph
template< typename __eg1 >
template< typename __eg2 >
template< typename __eg3 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Shape< __eg2 >::template __eg_Morph< __eg3 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Morph< __eg3 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Morph< __eg3 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Shape< __eg2 >::__eg_Morph< __eg3 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::StrokedCircle
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_StrokedCircle< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_StrokedCircle< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_StrokedCircle< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_StrokedCircle< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
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

//root::Spiral
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Spiral< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Spiral< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Spiral< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Spiral< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Attract
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Attract< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Attract< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Attract< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Attract< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}

//root::Randomise
template< typename __eg1 >
template< typename __eg2 >
template< typename TypePath, typename Operation, typename... Args >
typename eg::result_type< typename __eg_root< __eg1 >::template __eg_Randomise< __eg2 >, TypePath, Operation >::Type
__eg_root< __eg1 >::__eg_Randomise< __eg2 >::invoke( Args... args ) const
{
    using CanonicalTypePathType = typename eg::CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename eg::result_type< __eg_root< __eg1 >::__eg_Randomise< __eg2 >, TypePath, Operation >::Type, __eg_root< __eg1 >::__eg_Randomise< __eg2 >, CanonicalTypePathType, Operation >()( *this, args... );
}


