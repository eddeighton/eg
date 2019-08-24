#include "structures.hpp"
eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 19: return g_root[ instance ].g_root_reference.data.timestamp;
        case 26: return g_root_PrintRes[ instance ].g_root_PrintRes_reference.data.timestamp;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 19: return g_root[ instance ].g_root_state;
        case 26: return g_root_PrintRes[ instance ].g_root_PrintRes_state;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 19: return g_root[ instance ].g_root_fiber;
        case 26: return g_root_PrintRes[ instance ].g_root_PrintRes_fiber;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 19: return g_root[ instance ].g_root_cycle;
        case 26: return g_root_PrintRes[ instance ].g_root_PrintRes_cycle;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
__eg_root< void > get_root()
{
    return  __eg_root< void >( eg::reference{ 0, 19, getTimestamp( 19, 0 ) } );
}

void root_stopper( eg::Instance _gid );
void root_PrintRes_stopper( eg::Instance _gid );
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
         for( eg::Instance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_PrintRes_stopper( childIndex );
         }
         g_root[ _gid ].g_root_state = ::eg::action_stopped;
         g_root[ _gid ].g_root_cycle = clock::cycle();
         if( g_root[ _gid ].g_root_fiber.joinable() )
             g_root[ _gid ].g_root_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root[ _gid ].g_root_reference, sizeof( eg::reference ) );
     }
}


__eg_root< void >::__eg_PrintRes< void > root_PrintRes_starter( eg::Instance _parent_id )
{
    //claim next free index
    eg::Iterator iter, expected;
    while( true )
    {
         iter = eg::Iterator( g_root[ _parent_id ].g_rootPrintRes_ring_iter.load() );
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
         if( g_root[ _parent_id ].g_rootPrintRes_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
         {
             const eg::Instance nextRingIndex = _parent_id * 1 + relativeNextCellIndex;
             const eg::Instance nextInstance = g_root_PrintRes[ nextRingIndex ].g_root_PrintRes_ring;
             //successfully claimed index so get the actual instance from the ring buffer
             const eg::Instance startCycle = clock::cycle();
             __eg_root< void >::__eg_PrintRes< void >& reference = g_root_PrintRes[ nextInstance ].g_root_PrintRes_reference;
             reference.data.timestamp = startCycle;
             g_root_PrintRes[ nextInstance ].g_root_PrintRes_state = ::eg::action_running;
             g_root_PrintRes[ nextInstance ].g_root_PrintRes_ring_index = nextRingIndex;
             events::put( "start", startCycle, &reference.data, sizeof( eg::reference ) );
             return reference;
         }
    }
    //failure return null handle
    events::put( "error", clock::cycle(), "Failed to allocate root::PrintRes", 34);
    __eg_root< void >::__eg_PrintRes< void > nullInstance;
    return nullInstance;
}

void root_PrintRes_stopper( eg::Instance _gid )
{
     eg::Instance _parent_id = _gid / 1;
     if( g_root_PrintRes[ _gid ].g_root_PrintRes_state != ::eg::action_stopped )
     {
         eg::Iterator iter, expected;
         while( true )
         {
              iter = eg::Iterator( g_root[ _parent_id ].g_rootPrintRes_ring_iter.load() );
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
              if( g_root[ _parent_id ].g_rootPrintRes_ring_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  const eg::Instance ringIndex = g_root_PrintRes[ _gid ].g_root_PrintRes_ring_index;
                  if( ringBufferTailIndex != ringIndex )
                  {
                      const eg::Instance tailInstance = g_root_PrintRes[ ringBufferTailIndex ].g_root_PrintRes_ring;
                      g_root_PrintRes[ ringIndex ].g_root_PrintRes_ring = tailInstance;
                      g_root_PrintRes[ tailInstance ].g_root_PrintRes_ring_index = ringIndex;
                  }
                  g_root_PrintRes[ ringBufferTailIndex ].g_root_PrintRes_ring = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootPrintRes_ring_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_PrintRes[ _gid ].g_root_PrintRes_state = ::eg::action_stopped;
         g_root_PrintRes[ _gid ].g_root_PrintRes_cycle = clock::cycle();
         if( g_root_PrintRes[ _gid ].g_root_PrintRes_fiber.joinable() )
             g_root_PrintRes[ _gid ].g_root_PrintRes_fiber.detach();
         events::put( "stop", clock::cycle(), &g_root_PrintRes[ _gid ].g_root_PrintRes_reference, sizeof( eg::reference ) );
     }
}


