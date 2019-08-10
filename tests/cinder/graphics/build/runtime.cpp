#include "structures.hpp"
eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 67: return g_root[ instance ].g_root_reference.data.timestamp;
        case 74: return g_root_Line[ instance ].g_root_Line_reference.data.timestamp;
        case 83: return g_root_Line_Move[ instance ].g_root_Line_Move_reference.data.timestamp;
        case 89: return g_root_Line_Up[ instance ].g_root_Line_Up_reference.data.timestamp;
        case 95: return g_root_Line_Down[ instance ].g_root_Line_Down_reference.data.timestamp;
        case 101: return g_root_Line_Left[ instance ].g_root_Line_Left_reference.data.timestamp;
        case 107: return g_root_Line_Right[ instance ].g_root_Line_Right_reference.data.timestamp;
        case 113: return g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_reference.data.timestamp;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 67: return g_root[ instance ].g_root_state;
        case 74: return g_root_Line[ instance ].g_root_Line_state;
        case 83: return g_root_Line_Move[ instance ].g_root_Line_Move_state;
        case 89: return g_root_Line_Up[ instance ].g_root_Line_Up_state;
        case 95: return g_root_Line_Down[ instance ].g_root_Line_Down_state;
        case 101: return g_root_Line_Left[ instance ].g_root_Line_Left_state;
        case 107: return g_root_Line_Right[ instance ].g_root_Line_Right_state;
        case 113: return g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_state;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 67: return g_root[ instance ].g_root_fiber;
        case 74: return g_root_Line[ instance ].g_root_Line_fiber;
        case 83: return g_root_Line_Move[ instance ].g_root_Line_Move_fiber;
        case 89: return g_root_Line_Up[ instance ].g_root_Line_Up_fiber;
        case 95: return g_root_Line_Down[ instance ].g_root_Line_Down_fiber;
        case 101: return g_root_Line_Left[ instance ].g_root_Line_Left_fiber;
        case 107: return g_root_Line_Right[ instance ].g_root_Line_Right_fiber;
        case 113: return g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_fiber;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 67: return g_root[ instance ].g_root_cycle;
        case 74: return g_root_Line[ instance ].g_root_Line_cycle;
        case 83: return g_root_Line_Move[ instance ].g_root_Line_Move_cycle;
        case 89: return g_root_Line_Up[ instance ].g_root_Line_Up_cycle;
        case 95: return g_root_Line_Down[ instance ].g_root_Line_Down_cycle;
        case 101: return g_root_Line_Left[ instance ].g_root_Line_Left_cycle;
        case 107: return g_root_Line_Right[ instance ].g_root_Line_Right_cycle;
        case 113: return g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_cycle;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
__eg_root< void > get_root()
{
    return  __eg_root< void >( eg::reference{ 0, 67, getTimestamp( 67, 0 ) } );
}

void root_stopper( eg::Instance _gid );
void root_Line_stopper( eg::Instance _gid );
void root_Line_Move_stopper( eg::Instance _gid );
void root_Line_Up_stopper( eg::Instance _gid );
void root_Line_Down_stopper( eg::Instance _gid );
void root_Line_Left_stopper( eg::Instance _gid );
void root_Line_Right_stopper( eg::Instance _gid );
void root_Line_Sequencer_stopper( eg::Instance _gid );
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
            catch( std::exception& e )                                                      
            {                                                                               
                ERR( e.what() );                                                            
            }                                                                               
            catch( ... )                                                                    
            {                                                                               
                ERR( "Unknown exception occured in root" );
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
         for( eg::Instance childIndex = _gid * 2048; childIndex != ( _gid + 1 ) * 2048; ++childIndex )
         {
             root_Line_stopper( childIndex );
         }
         g_root[ _gid ].g_root_state = ::eg::action_stopped;
         g_root[ _gid ].g_root_cycle = clock::cycle();
         if( g_root[ _gid ].g_root_fiber.joinable() )
             g_root[ _gid ].g_root_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root[ _gid ].g_root_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Line< void > root_Line_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _parent_id ].g_rootLine_ring_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         const eg::Instance relativeNextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( relativeNextCellIndex == 2047 )
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
         //attempt to set the atomic
         if( g_root[ _parent_id ].g_rootLine_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 2048 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_Line[ nextRingIndex ].g_root_Line_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_Line< void >& reference = g_root_Line[ nextInstance ].g_root_Line_reference;
             reference.data.timestamp = startCycle;
             g_root_Line[ nextInstance ].g_root_Line_state = ::eg::action_running;
             g_root_Line[ nextInstance ].g_root_Line_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::Line", 30);
    __eg_root< void >::__eg_Line< void > nullInstance;
    return nullInstance;
}

void root_Line_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 2048;
     if( g_root_Line[ _gid ].g_root_Line_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootLine_ring_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              const eg::Instance ringBufferTailIndex = _parent_id * 2048 + static_cast< eg::Instance >( iter.tail );
              //if buffer is full then set the protection bit while freeing
              if( iter.full )
              {
                  iter.protection = 1U;
                  iter.full = 0U;
              }
              //claim the index to store free instance
              if( static_cast< eg::Instance >( iter.tail ) == 2048 - 1U )
              {
                  iter.tail = 0U;
              }
              else
              {
                  ++iter.tail;
              }
              //attempt to set the atomic
              if( g_root[ _parent_id ].g_rootLine_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_Line[ _gid ].g_root_Line_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_Line[ ringBufferTailIndex ].g_root_Line_ring;
                      g_root_Line[ ringIndex ].g_root_Line_ring = tailInstance;
                      g_root_Line[ tailInstance ].g_root_Line_ring_index = ringIndex;
                  }
                  g_root_Line[ ringBufferTailIndex ].g_root_Line_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootLine_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Line_Move_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Line_Up_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Line_Down_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Line_Left_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Line_Right_stopper( childIndex );
         }
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Line_Sequencer_stopper( childIndex );
         }
         g_root_Line[ _gid ].g_root_Line_state = ::eg::action_stopped;
         g_root_Line[ _gid ].g_root_Line_cycle = clock::cycle();
         if( g_root_Line[ _gid ].g_root_Line_fiber.joinable() )
             g_root_Line[ _gid ].g_root_Line_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Line[ _gid ].g_root_Line_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Line< void >::__eg_Move< void > root_Line_Move_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineMove_ring_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         const eg::Instance relativeNextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( relativeNextCellIndex == 0 )
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
         //attempt to set the atomic
         if( g_root_Line[ _parent_id ].g_root_LineMove_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 1 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_Line_Move[ nextRingIndex ].g_root_Line_Move_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_Line< void >::__eg_Move< void >& reference = g_root_Line_Move[ nextInstance ].g_root_Line_Move_reference;
             reference.data.timestamp = startCycle;
             g_root_Line_Move[ nextInstance ].g_root_Line_Move_state = ::eg::action_running;
             g_root_Line_Move[ nextInstance ].g_root_Line_Move_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::Line::Move", 36);
    __eg_root< void >::__eg_Line< void >::__eg_Move< void > nullInstance;
    return nullInstance;
}

void root_Line_Move_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Line_Move[ _gid ].g_root_Line_Move_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineMove_ring_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              const eg::Instance ringBufferTailIndex = _parent_id * 1 + static_cast< eg::Instance >( iter.tail );
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
              if( g_root_Line[ _parent_id ].g_root_LineMove_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_Line_Move[ _gid ].g_root_Line_Move_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_Line_Move[ ringBufferTailIndex ].g_root_Line_Move_ring;
                      g_root_Line_Move[ ringIndex ].g_root_Line_Move_ring = tailInstance;
                      g_root_Line_Move[ tailInstance ].g_root_Line_Move_ring_index = ringIndex;
                  }
                  g_root_Line_Move[ ringBufferTailIndex ].g_root_Line_Move_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Line[ _parent_id ].g_root_LineMove_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Line_Move[ _gid ].g_root_Line_Move_state = ::eg::action_stopped;
         g_root_Line_Move[ _gid ].g_root_Line_Move_cycle = clock::cycle();
         if( g_root_Line_Move[ _gid ].g_root_Line_Move_fiber.joinable() )
             g_root_Line_Move[ _gid ].g_root_Line_Move_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Line_Move[ _gid ].g_root_Line_Move_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Line< void >::__eg_Up< void > root_Line_Up_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineUp_ring_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         const eg::Instance relativeNextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( relativeNextCellIndex == 0 )
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
         //attempt to set the atomic
         if( g_root_Line[ _parent_id ].g_root_LineUp_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 1 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_Line_Up[ nextRingIndex ].g_root_Line_Up_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_Line< void >::__eg_Up< void >& reference = g_root_Line_Up[ nextInstance ].g_root_Line_Up_reference;
             reference.data.timestamp = startCycle;
             g_root_Line_Up[ nextInstance ].g_root_Line_Up_state = ::eg::action_running;
             g_root_Line_Up[ nextInstance ].g_root_Line_Up_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::Line::Up", 34);
    __eg_root< void >::__eg_Line< void >::__eg_Up< void > nullInstance;
    return nullInstance;
}

void root_Line_Up_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Line_Up[ _gid ].g_root_Line_Up_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineUp_ring_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              const eg::Instance ringBufferTailIndex = _parent_id * 1 + static_cast< eg::Instance >( iter.tail );
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
              if( g_root_Line[ _parent_id ].g_root_LineUp_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_Line_Up[ _gid ].g_root_Line_Up_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_Line_Up[ ringBufferTailIndex ].g_root_Line_Up_ring;
                      g_root_Line_Up[ ringIndex ].g_root_Line_Up_ring = tailInstance;
                      g_root_Line_Up[ tailInstance ].g_root_Line_Up_ring_index = ringIndex;
                  }
                  g_root_Line_Up[ ringBufferTailIndex ].g_root_Line_Up_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Line[ _parent_id ].g_root_LineUp_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Line_Up[ _gid ].g_root_Line_Up_state = ::eg::action_stopped;
         g_root_Line_Up[ _gid ].g_root_Line_Up_cycle = clock::cycle();
         if( g_root_Line_Up[ _gid ].g_root_Line_Up_fiber.joinable() )
             g_root_Line_Up[ _gid ].g_root_Line_Up_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Line_Up[ _gid ].g_root_Line_Up_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Line< void >::__eg_Down< void > root_Line_Down_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineDown_ring_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         const eg::Instance relativeNextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( relativeNextCellIndex == 0 )
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
         //attempt to set the atomic
         if( g_root_Line[ _parent_id ].g_root_LineDown_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 1 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_Line_Down[ nextRingIndex ].g_root_Line_Down_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_Line< void >::__eg_Down< void >& reference = g_root_Line_Down[ nextInstance ].g_root_Line_Down_reference;
             reference.data.timestamp = startCycle;
             g_root_Line_Down[ nextInstance ].g_root_Line_Down_state = ::eg::action_running;
             g_root_Line_Down[ nextInstance ].g_root_Line_Down_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::Line::Down", 36);
    __eg_root< void >::__eg_Line< void >::__eg_Down< void > nullInstance;
    return nullInstance;
}

void root_Line_Down_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Line_Down[ _gid ].g_root_Line_Down_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineDown_ring_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              const eg::Instance ringBufferTailIndex = _parent_id * 1 + static_cast< eg::Instance >( iter.tail );
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
              if( g_root_Line[ _parent_id ].g_root_LineDown_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_Line_Down[ _gid ].g_root_Line_Down_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_Line_Down[ ringBufferTailIndex ].g_root_Line_Down_ring;
                      g_root_Line_Down[ ringIndex ].g_root_Line_Down_ring = tailInstance;
                      g_root_Line_Down[ tailInstance ].g_root_Line_Down_ring_index = ringIndex;
                  }
                  g_root_Line_Down[ ringBufferTailIndex ].g_root_Line_Down_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Line[ _parent_id ].g_root_LineDown_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Line_Down[ _gid ].g_root_Line_Down_state = ::eg::action_stopped;
         g_root_Line_Down[ _gid ].g_root_Line_Down_cycle = clock::cycle();
         if( g_root_Line_Down[ _gid ].g_root_Line_Down_fiber.joinable() )
             g_root_Line_Down[ _gid ].g_root_Line_Down_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Line_Down[ _gid ].g_root_Line_Down_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Line< void >::__eg_Left< void > root_Line_Left_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineLeft_ring_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         const eg::Instance relativeNextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( relativeNextCellIndex == 0 )
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
         //attempt to set the atomic
         if( g_root_Line[ _parent_id ].g_root_LineLeft_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 1 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_Line_Left[ nextRingIndex ].g_root_Line_Left_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_Line< void >::__eg_Left< void >& reference = g_root_Line_Left[ nextInstance ].g_root_Line_Left_reference;
             reference.data.timestamp = startCycle;
             g_root_Line_Left[ nextInstance ].g_root_Line_Left_state = ::eg::action_running;
             g_root_Line_Left[ nextInstance ].g_root_Line_Left_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::Line::Left", 36);
    __eg_root< void >::__eg_Line< void >::__eg_Left< void > nullInstance;
    return nullInstance;
}

void root_Line_Left_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Line_Left[ _gid ].g_root_Line_Left_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineLeft_ring_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              const eg::Instance ringBufferTailIndex = _parent_id * 1 + static_cast< eg::Instance >( iter.tail );
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
              if( g_root_Line[ _parent_id ].g_root_LineLeft_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_Line_Left[ _gid ].g_root_Line_Left_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_Line_Left[ ringBufferTailIndex ].g_root_Line_Left_ring;
                      g_root_Line_Left[ ringIndex ].g_root_Line_Left_ring = tailInstance;
                      g_root_Line_Left[ tailInstance ].g_root_Line_Left_ring_index = ringIndex;
                  }
                  g_root_Line_Left[ ringBufferTailIndex ].g_root_Line_Left_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Line[ _parent_id ].g_root_LineLeft_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Line_Left[ _gid ].g_root_Line_Left_state = ::eg::action_stopped;
         g_root_Line_Left[ _gid ].g_root_Line_Left_cycle = clock::cycle();
         if( g_root_Line_Left[ _gid ].g_root_Line_Left_fiber.joinable() )
             g_root_Line_Left[ _gid ].g_root_Line_Left_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Line_Left[ _gid ].g_root_Line_Left_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Line< void >::__eg_Right< void > root_Line_Right_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineRight_ring_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         const eg::Instance relativeNextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( relativeNextCellIndex == 0 )
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
         //attempt to set the atomic
         if( g_root_Line[ _parent_id ].g_root_LineRight_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 1 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_Line_Right[ nextRingIndex ].g_root_Line_Right_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_Line< void >::__eg_Right< void >& reference = g_root_Line_Right[ nextInstance ].g_root_Line_Right_reference;
             reference.data.timestamp = startCycle;
             g_root_Line_Right[ nextInstance ].g_root_Line_Right_state = ::eg::action_running;
             g_root_Line_Right[ nextInstance ].g_root_Line_Right_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::Line::Right", 37);
    __eg_root< void >::__eg_Line< void >::__eg_Right< void > nullInstance;
    return nullInstance;
}

void root_Line_Right_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Line_Right[ _gid ].g_root_Line_Right_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineRight_ring_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              const eg::Instance ringBufferTailIndex = _parent_id * 1 + static_cast< eg::Instance >( iter.tail );
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
              if( g_root_Line[ _parent_id ].g_root_LineRight_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_Line_Right[ _gid ].g_root_Line_Right_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_Line_Right[ ringBufferTailIndex ].g_root_Line_Right_ring;
                      g_root_Line_Right[ ringIndex ].g_root_Line_Right_ring = tailInstance;
                      g_root_Line_Right[ tailInstance ].g_root_Line_Right_ring_index = ringIndex;
                  }
                  g_root_Line_Right[ ringBufferTailIndex ].g_root_Line_Right_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Line[ _parent_id ].g_root_LineRight_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Line_Right[ _gid ].g_root_Line_Right_state = ::eg::action_stopped;
         g_root_Line_Right[ _gid ].g_root_Line_Right_cycle = clock::cycle();
         if( g_root_Line_Right[ _gid ].g_root_Line_Right_fiber.joinable() )
             g_root_Line_Right[ _gid ].g_root_Line_Right_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Line_Right[ _gid ].g_root_Line_Right_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > root_Line_Sequencer_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineSequencer_ring_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         const eg::Instance relativeNextCellIndex = static_cast< eg::Instance >( iter.head );
         //claim the next free index
         if( relativeNextCellIndex == 0 )
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
         //attempt to set the atomic
         if( g_root_Line[ _parent_id ].g_root_LineSequencer_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 1 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_Line_Sequencer[ nextRingIndex ].g_root_Line_Sequencer_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >& reference = g_root_Line_Sequencer[ nextInstance ].g_root_Line_Sequencer_reference;
             reference.data.timestamp = startCycle;
             g_root_Line_Sequencer[ nextInstance ].g_root_Line_Sequencer_state = ::eg::action_running;
             g_root_Line_Sequencer[ nextInstance ].g_root_Line_Sequencer_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::Line::Sequencer", 41);
    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > nullInstance;
    return nullInstance;
}

void root_Line_Sequencer_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_Line_Sequencer[ _gid ].g_root_Line_Sequencer_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root_Line[ _parent_id ].g_root_LineSequencer_ring_iter.load() );
              if( iter.protection )
                  continue;
              expected = iter;
              const eg::Instance ringBufferTailIndex = _parent_id * 1 + static_cast< eg::Instance >( iter.tail );
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
              if( g_root_Line[ _parent_id ].g_root_LineSequencer_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_Line_Sequencer[ _gid ].g_root_Line_Sequencer_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_Line_Sequencer[ ringBufferTailIndex ].g_root_Line_Sequencer_ring;
                      g_root_Line_Sequencer[ ringIndex ].g_root_Line_Sequencer_ring = tailInstance;
                      g_root_Line_Sequencer[ tailInstance ].g_root_Line_Sequencer_ring_index = ringIndex;
                  }
                  g_root_Line_Sequencer[ ringBufferTailIndex ].g_root_Line_Sequencer_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root_Line[ _parent_id ].g_root_LineSequencer_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Line_Sequencer[ _gid ].g_root_Line_Sequencer_state = ::eg::action_stopped;
         g_root_Line_Sequencer[ _gid ].g_root_Line_Sequencer_cycle = clock::cycle();
         if( g_root_Line_Sequencer[ _gid ].g_root_Line_Sequencer_fiber.joinable() )
             g_root_Line_Sequencer[ _gid ].g_root_Line_Sequencer_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_Line_Sequencer[ _gid ].g_root_Line_Sequencer_reference, sizeof( eg::reference ) );
     }
}


