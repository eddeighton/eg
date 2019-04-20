//eg implementation source code

//buffers

//Buffer: b_root size: 1
struct b_root
{
    EGTimeStamp g_root_timestamp_runnning;
    EGTimeStamp g_root_timestamp_paused;
    EGCoroutine g_root_coroutine;
    __eg_event_iterator g_root_event_iter;
    __interface_root< void > g_root_reference;
    EGTimeStamp g_root_timestamp_stopped;
    std::atomic< std::uint64_t > g_rootCircle_alloc_iter;
    std::atomic< std::uint64_t > g_rootSpiral_alloc_iter;
};
static b_root *g_root;

//Buffer: b_root_Circle size: 2048
struct b_root_Circle
{
    EGTimeStamp g_root_Circle_timestamp_runnning;
    EGTimeStamp g_root_Circle_timestamp_paused;
    EGCoroutine g_root_Circle_coroutine;
    __eg_event_iterator g_root_Circle_event_iter;
    __interface_root< void >::__interface_Circle< void > g_root_Circle_reference;
    EGTimeStamp g_root_Circle_timestamp_stopped;
    struct glm::tvec2<float, glm::packed_highp> m_position;
    EGTimeStamp g_position_timestamp;
    class cinder::ColorT<float> m_color;
    EGTimeStamp g_color_timestamp;
    float m_size;
    EGTimeStamp g_size_timestamp;
    EGInstance g_root_Circle_alloc_data;
    std::atomic< std::uint64_t > g_root_CircleColorChanger_alloc_iter;
};
static b_root_Circle *g_root_Circle;

//Buffer: b_root_Circle_ColorChanger size: 2048
struct b_root_Circle_ColorChanger
{
    EGTimeStamp g_root_Circle_ColorChanger_timestamp_runnning;
    EGTimeStamp g_root_Circle_ColorChanger_timestamp_paused;
    EGCoroutine g_root_Circle_ColorChanger_coroutine;
    __eg_event_iterator g_root_Circle_ColorChanger_event_iter;
    __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > g_root_Circle_ColorChanger_reference;
    EGTimeStamp g_root_Circle_ColorChanger_timestamp_stopped;
    float m_f;
    EGTimeStamp g_f_timestamp;
    EGInstance g_root_Circle_ColorChanger_alloc_data;
};
static b_root_Circle_ColorChanger *g_root_Circle_ColorChanger;

//Buffer: b_root_Spiral size: 1
struct b_root_Spiral
{
    EGTimeStamp g_root_Spiral_timestamp_runnning;
    EGTimeStamp g_root_Spiral_timestamp_paused;
    EGCoroutine g_root_Spiral_coroutine;
    __eg_event_iterator g_root_Spiral_event_iter;
    __interface_root< void >::__interface_Spiral< void > g_root_Spiral_reference;
    EGTimeStamp g_root_Spiral_timestamp_stopped;
    EGInstance g_root_Spiral_alloc_data;
};
static b_root_Spiral *g_root_Spiral;

//input::Action function forward declarations

__interface_root< void > root_starter( EGInstance _gid );
void root_stopper( EGInstance _gid );
bool root_executor();

__interface_root< void >::__interface_Circle< void > root_Circle_starter( EGInstance _gid );
void root_Circle_stopper( EGInstance _gid );
bool root_Circle_executor();

__interface_root< void >::__interface_Spiral< void > root_Spiral_starter( EGInstance _gid );
void root_Spiral_stopper( EGInstance _gid );
bool root_Spiral_executor();

__interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > root_Circle_ColorChanger_starter( EGInstance _gid );
void root_Circle_ColorChanger_stopper( EGInstance _gid );
bool root_Circle_ColorChanger_executor();


     
//global dependencies
struct __eg_clock
{
    virtual EGTimeStamp cycle()     const = 0;
    virtual EGTimeStamp subcycle()  const = 0;
    virtual float ct()              const = 0;
    virtual float dt()              const = 0;
};
static __eg_clock* g_eg_clock;

//clock impl\n";
EGTimeStamp clock::cycle()      { return g_eg_clock->cycle(); }
EGTimeStamp clock::subcycle()   { return g_eg_clock->subcycle(); }
float clock::ct()               { return g_eg_clock->ct(); }
float clock::dt()               { return g_eg_clock->dt(); }

struct __eg_event
{
    const char* type;
    EGTimeStamp timestamp;
    const void* value;
    std::size_t size;
};

struct __eg_event_log
{
    virtual __eg_event_iterator GetEventIterator() = 0;
    virtual bool GetEvent( __eg_event_iterator& iterator, __eg_event& event ) = 0;
    virtual void PutEvent( const __eg_event& event ) = 0;
};
static __eg_event_log* g_eg_event_log;

void events::put( const char* type, EGTimeStamp timestamp, const void* value, std::size_t size )
{
    __eg_event ev = { type, timestamp, value, size };
    g_eg_event_log->PutEvent( ev );
}
    
void initialise( EGDependencyProvider* pDependencyProvider )
{    
    //buffers
    g_root = reinterpret_cast< b_root* >( pDependencyProvider->getBuffer( "g_root" ) );
    g_root_Circle = reinterpret_cast< b_root_Circle* >( pDependencyProvider->getBuffer( "g_root_Circle" ) );
    g_root_Circle_ColorChanger = reinterpret_cast< b_root_Circle_ColorChanger* >( pDependencyProvider->getBuffer( "g_root_Circle_ColorChanger" ) );
    g_root_Spiral = reinterpret_cast< b_root_Spiral* >( pDependencyProvider->getBuffer( "g_root_Spiral" ) );
    //global dependencies
    g_eg_clock = reinterpret_cast< __eg_clock* >( pDependencyProvider->getInterface( "__eg_clock" ) );
    g_eg_event_log = reinterpret_cast< __eg_event_log* >( pDependencyProvider->getInterface( "__eg_event_log" ) );
    g_eg_input = reinterpret_cast< __eg_input* >( pDependencyProvider->getInterface( "__eg_input" ) );
}

//input::Action Function Implementations

__interface_root< void > root_starter( EGInstance _gid )
{
    const EGInstance startSubCycle = clock::subcycle();
    __interface_root< void >& reference = g_root[ 0 ].g_root_reference;
    reference.data.timestamp = startSubCycle;
    g_root[ 0 ].g_root_timestamp_runnning = startSubCycle;
    g_root[ 0 ].g_root_timestamp_paused = startSubCycle;
    __eg_event ev = { "start", startSubCycle, &reference.data, sizeof( __eg_reference ) };
    g_eg_event_log->PutEvent( ev );
    g_root[ 0 ].g_root_event_iter = g_eg_event_log->GetEventIterator();
    g_root[ 0 ].g_root_coroutine = reference();
    return reference;
}

void root_stopper( EGInstance _gid )
{
     EGInstance _parent_id = _gid / 1;
     if( g_root[ _gid ].g_root_timestamp_runnning != EG_INVALID_TIMESTAMP )
     {
         for( EGInstance childIndex = _gid * 2048; childIndex != ( _gid + 1 ) * 2048; ++childIndex )
         {
             root_Circle_stopper( childIndex );
         }
         for( EGInstance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Spiral_stopper( childIndex );
         }
         g_root[ _gid ].g_root_timestamp_runnning = EG_INVALID_TIMESTAMP;
         g_root[ _gid ].g_root_timestamp_paused = EG_INVALID_TIMESTAMP;
         g_root[ _gid ].g_root_coroutine.destroy();
         __eg_event ev = { "stop", clock::subcycle(), &g_root[ _gid ].g_root_reference, sizeof( __eg_reference ) };
         g_eg_event_log->PutEvent( ev );
     }
}

bool root_executor()
{
    const EGTimeStamp subcycle = clock::subcycle();
    for( EGInstance i = 0; i != 1; ++i )
    {
        if( g_root[ i ].g_root_timestamp_runnning <= subcycle )
        {
             if( g_root[ i ].g_root_timestamp_paused <= subcycle )
             {
                 if( g_root[ i ].g_root_coroutine.done() )
                 {
                     root_stopper( i );
                 }
                 else
                 {
                     g_root[ i ].g_root_coroutine.resume();
                 }
             }
        }
    }
    return false;
}


__interface_root< void >::__interface_Circle< void > root_Circle_starter( EGInstance _gid )
{
    //claim next free index
    EGIterator iter, expected;
    while( true )
    {
         iter = EGIterator( g_root[ _gid ].g_rootCircle_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         EGInstance nextCellIndex = static_cast< EGInstance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 2047 )
         {
             iter.head = 0U;
         }
         else
         {
             ++iter.head;
         }
         if( static_cast< EGInstance >( iter.head ) == static_cast< EGInstance >( iter.tail ) )
         {
             iter.full = 1U;
         }
         EGInstance nextInstance = g_root_Circle[ _gid * 2048 + nextCellIndex ].g_root_Circle_alloc_data;
         if( g_root_Circle[ nextInstance ].g_root_Circle_timestamp_stopped <= clock::subcycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootCircle_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const EGInstance startSubCycle = clock::subcycle() + 1;
                 __interface_root< void >::__interface_Circle< void >& reference = g_root_Circle[ nextInstance ].g_root_Circle_reference;
                 reference.data.timestamp = startSubCycle;
                 g_root_Circle[ nextInstance ].g_root_Circle_timestamp_runnning = startSubCycle;
                 g_root_Circle[ nextInstance ].g_root_Circle_timestamp_paused = startSubCycle;
                 __eg_event ev = { "start", startSubCycle, &reference.data, sizeof( __eg_reference ) };
                 g_eg_event_log->PutEvent( ev );
                 g_root_Circle[ nextInstance ].g_root_Circle_event_iter = g_eg_event_log->GetEventIterator();
                 g_root_Circle[ nextInstance ].g_root_Circle_coroutine = reference();
                 return reference;
             }
         }
         else
         {
             break;
         }
    }
    //failure return null handle
    __interface_root< void >::__interface_Circle< void > nullInstance;
    return nullInstance;
}

void root_Circle_stopper( EGInstance _gid )
{
     EGInstance _parent_id = _gid / 2048;
     if( g_root_Circle[ _gid ].g_root_Circle_timestamp_runnning != EG_INVALID_TIMESTAMP )
     {
         EGIterator iter, expected;
         while( true )
         {
              iter = EGIterator( g_root[ _parent_id ].g_rootCircle_alloc_iter.load() );
              if( iter.protection )
                  break;
              expected = iter;
              EGInstance freeCellIndex = static_cast< EGInstance >( iter.tail );
              //if buffer is full then set the protection bit while freeing
              if( iter.full )
              {
                  iter.protection = 1U;
                  iter.full = 0U;
              }
              //claim the index to store free instance
              if( static_cast< EGInstance >( iter.tail ) == 2048 - 1U )
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
         for( EGInstance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Circle_ColorChanger_stopper( childIndex );
         }
         g_root_Circle[ _gid ].g_root_Circle_timestamp_runnning = EG_INVALID_TIMESTAMP;
         g_root_Circle[ _gid ].g_root_Circle_timestamp_paused = EG_INVALID_TIMESTAMP;
         g_root_Circle[ _gid ].g_root_Circle_timestamp_stopped = clock::subcycle() + 2U;
         g_root_Circle[ _gid ].g_root_Circle_coroutine.destroy();
         __eg_event ev = { "stop", clock::subcycle(), &g_root_Circle[ _gid ].g_root_Circle_reference, sizeof( __eg_reference ) };
         g_eg_event_log->PutEvent( ev );
     }
}

bool root_Circle_executor()
{
    const EGTimeStamp subcycle = clock::subcycle();
    for( EGInstance i = 0; i != 2048; ++i )
    {
        if( g_root_Circle[ i ].g_root_Circle_timestamp_runnning <= subcycle )
        {
             if( g_root_Circle[ i ].g_root_Circle_timestamp_paused <= subcycle )
             {
                 if( g_root_Circle[ i ].g_root_Circle_coroutine.done() )
                 {
                     root_Circle_stopper( i );
                 }
                 else
                 {
                     g_root_Circle[ i ].g_root_Circle_coroutine.resume();
                 }
             }
        }
    }
    return false;
}


__interface_root< void >::__interface_Spiral< void > root_Spiral_starter( EGInstance _gid )
{
    //claim next free index
    EGIterator iter, expected;
    while( true )
    {
         iter = EGIterator( g_root[ _gid ].g_rootSpiral_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         EGInstance nextCellIndex = static_cast< EGInstance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 0 )
         {
             iter.head = 0U;
         }
         else
         {
             ++iter.head;
         }
         if( static_cast< EGInstance >( iter.head ) == static_cast< EGInstance >( iter.tail ) )
         {
             iter.full = 1U;
         }
         EGInstance nextInstance = g_root_Spiral[ _gid * 1 + nextCellIndex ].g_root_Spiral_alloc_data;
         if( g_root_Spiral[ nextInstance ].g_root_Spiral_timestamp_stopped <= clock::subcycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootSpiral_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const EGInstance startSubCycle = clock::subcycle() + 1;
                 __interface_root< void >::__interface_Spiral< void >& reference = g_root_Spiral[ nextInstance ].g_root_Spiral_reference;
                 reference.data.timestamp = startSubCycle;
                 g_root_Spiral[ nextInstance ].g_root_Spiral_timestamp_runnning = startSubCycle;
                 g_root_Spiral[ nextInstance ].g_root_Spiral_timestamp_paused = startSubCycle;
                 __eg_event ev = { "start", startSubCycle, &reference.data, sizeof( __eg_reference ) };
                 g_eg_event_log->PutEvent( ev );
                 g_root_Spiral[ nextInstance ].g_root_Spiral_event_iter = g_eg_event_log->GetEventIterator();
                 g_root_Spiral[ nextInstance ].g_root_Spiral_coroutine = reference();
                 return reference;
             }
         }
         else
         {
             break;
         }
    }
    //failure return null handle
    __interface_root< void >::__interface_Spiral< void > nullInstance;
    return nullInstance;
}

void root_Spiral_stopper( EGInstance _gid )
{
     EGInstance _parent_id = _gid / 1;
     if( g_root_Spiral[ _gid ].g_root_Spiral_timestamp_runnning != EG_INVALID_TIMESTAMP )
     {
         EGIterator iter, expected;
         while( true )
         {
              iter = EGIterator( g_root[ _parent_id ].g_rootSpiral_alloc_iter.load() );
              if( iter.protection )
                  break;
              expected = iter;
              EGInstance freeCellIndex = static_cast< EGInstance >( iter.tail );
              //if buffer is full then set the protection bit while freeing
              if( iter.full )
              {
                  iter.protection = 1U;
                  iter.full = 0U;
              }
              //claim the index to store free instance
              if( static_cast< EGInstance >( iter.tail ) == 1 - 1U )
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
         g_root_Spiral[ _gid ].g_root_Spiral_timestamp_runnning = EG_INVALID_TIMESTAMP;
         g_root_Spiral[ _gid ].g_root_Spiral_timestamp_paused = EG_INVALID_TIMESTAMP;
         g_root_Spiral[ _gid ].g_root_Spiral_timestamp_stopped = clock::subcycle() + 2U;
         g_root_Spiral[ _gid ].g_root_Spiral_coroutine.destroy();
         __eg_event ev = { "stop", clock::subcycle(), &g_root_Spiral[ _gid ].g_root_Spiral_reference, sizeof( __eg_reference ) };
         g_eg_event_log->PutEvent( ev );
     }
}

bool root_Spiral_executor()
{
    const EGTimeStamp subcycle = clock::subcycle();
    for( EGInstance i = 0; i != 1; ++i )
    {
        if( g_root_Spiral[ i ].g_root_Spiral_timestamp_runnning <= subcycle )
        {
             if( g_root_Spiral[ i ].g_root_Spiral_timestamp_paused <= subcycle )
             {
                 if( g_root_Spiral[ i ].g_root_Spiral_coroutine.done() )
                 {
                     root_Spiral_stopper( i );
                 }
                 else
                 {
                     g_root_Spiral[ i ].g_root_Spiral_coroutine.resume();
                 }
             }
        }
    }
    return false;
}


__interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > root_Circle_ColorChanger_starter( EGInstance _gid )
{
    //claim next free index
    EGIterator iter, expected;
    while( true )
    {
         iter = EGIterator( g_root_Circle[ _gid ].g_root_CircleColorChanger_alloc_iter.load() );
         if( iter.protection )
             continue;
         else if( iter.full )
             break;
         expected = iter;
         EGInstance nextCellIndex = static_cast< EGInstance >( iter.head );
         //claim the next free index
         if( nextCellIndex == 0 )
         {
             iter.head = 0U;
         }
         else
         {
             ++iter.head;
         }
         if( static_cast< EGInstance >( iter.head ) == static_cast< EGInstance >( iter.tail ) )
         {
             iter.full = 1U;
         }
         EGInstance nextInstance = g_root_Circle_ColorChanger[ _gid * 1 + nextCellIndex ].g_root_Circle_ColorChanger_alloc_data;
         if( g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_timestamp_stopped <= clock::subcycle() )
         {
             //attempt to set the atomic
             if( g_root_Circle[ _gid ].g_root_CircleColorChanger_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const EGInstance startSubCycle = clock::subcycle() + 1;
                 __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >& reference = g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_reference;
                 reference.data.timestamp = startSubCycle;
                 g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_timestamp_runnning = startSubCycle;
                 g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_timestamp_paused = startSubCycle;
                 __eg_event ev = { "start", startSubCycle, &reference.data, sizeof( __eg_reference ) };
                 g_eg_event_log->PutEvent( ev );
                 g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_event_iter = g_eg_event_log->GetEventIterator();
                 g_root_Circle_ColorChanger[ nextInstance ].g_root_Circle_ColorChanger_coroutine = reference();
                 return reference;
             }
         }
         else
         {
             break;
         }
    }
    //failure return null handle
    __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > nullInstance;
    return nullInstance;
}

void root_Circle_ColorChanger_stopper( EGInstance _gid )
{
     EGInstance _parent_id = _gid / 1;
     if( g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_timestamp_runnning != EG_INVALID_TIMESTAMP )
     {
         EGIterator iter, expected;
         while( true )
         {
              iter = EGIterator( g_root_Circle[ _parent_id ].g_root_CircleColorChanger_alloc_iter.load() );
              if( iter.protection )
                  break;
              expected = iter;
              EGInstance freeCellIndex = static_cast< EGInstance >( iter.tail );
              //if buffer is full then set the protection bit while freeing
              if( iter.full )
              {
                  iter.protection = 1U;
                  iter.full = 0U;
              }
              //claim the index to store free instance
              if( static_cast< EGInstance >( iter.tail ) == 1 - 1U )
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
         g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_timestamp_runnning = EG_INVALID_TIMESTAMP;
         g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_timestamp_paused = EG_INVALID_TIMESTAMP;
         g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_timestamp_stopped = clock::subcycle() + 2U;
         g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_coroutine.destroy();
         __eg_event ev = { "stop", clock::subcycle(), &g_root_Circle_ColorChanger[ _gid ].g_root_Circle_ColorChanger_reference, sizeof( __eg_reference ) };
         g_eg_event_log->PutEvent( ev );
     }
}

bool root_Circle_ColorChanger_executor()
{
    const EGTimeStamp subcycle = clock::subcycle();
    for( EGInstance i = 0; i != 2048; ++i )
    {
        if( g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_timestamp_runnning <= subcycle )
        {
             if( g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_timestamp_paused <= subcycle )
             {
                 if( g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_coroutine.done() )
                 {
                     root_Circle_ColorChanger_stopper( i );
                 }
                 else
                 {
                     g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_coroutine.resume();
                 }
             }
        }
    }
    return false;
}




//invocation implementations
template< typename ResultType, typename ContextType, typename TypePathType, typename OperationType, typename... Args >
inline ResultType __invoke_impl( ContextType context, Args... args );


template<>
__interface_root< void >::__interface_Circle< void >
inline __invoke_impl
<
    __interface_root< void >::__interface_Circle< void >,
    __interface_root< void >,
    __eg_type_path< __interface_root< void >::__interface_Circle< void >,Start >,
    Start
>
( __interface_root< void > context )
{
    return root_Circle_starter( context.data.instance );
}

template<>
__interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >
inline __invoke_impl
<
    __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >,
    __interface_root< void >::__interface_Circle< void >,
    __eg_type_path< ColorChanger,Start >,
    Start
>
( __interface_root< void >::__interface_Circle< void > context )
{
    return root_Circle_ColorChanger_starter( context.data.instance );
}

template<>
__interface_root< void >::__interface_Spiral< void >
inline __invoke_impl
<
    __interface_root< void >::__interface_Spiral< void >,
    __interface_root< void >,
    __eg_type_path< __interface_root< void >::__interface_Spiral< void >,Start >,
    Start
>
( __interface_root< void > context )
{
    return root_Spiral_starter( context.data.instance );
}

template<>
bool
inline __invoke_impl
<
    bool,
    __interface_root< void >,
    __eg_type_path< __interface_root< void >::__interface_Spiral< void >,Empty >,
    Empty
>
( __interface_root< void > context )
{
    EGIterator iter = EGIterator( g_root[ context.data.instance ].g_rootSpiral_alloc_iter.load() );
    return !iter.full && ( iter.tail == iter.head );
}

template<>
void
inline __invoke_impl
<
    void,
    __interface_root< void >,
    __eg_type_path< __interface_root< void >::__interface_Spiral< void >,Stop >,
    Stop
>
( __interface_root< void > context )
{
    root_Spiral_stopper( context.data.instance );
}

template<>
__interface_root< void >::__interface_Circle< void >::__interface_color< void >::Read
inline __invoke_impl
<
    __interface_root< void >::__interface_Circle< void >::__interface_color< void >::Read,
    __interface_root< void >::__interface_Circle< void >,
    __eg_type_path< color >,
    Read
>
( __interface_root< void >::__interface_Circle< void > context )
{
    EGInstance var_1 = context.data.instance;
    return g_root_Circle[ var_1 ].m_color;
}

template<>
__interface_root< void >::__interface_Circle< void >::__interface_position< void >::Read
inline __invoke_impl
<
    __interface_root< void >::__interface_Circle< void >::__interface_position< void >::Read,
    __interface_root< void >::__interface_Circle< void >,
    __eg_type_path< position >,
    Read
>
( __interface_root< void >::__interface_Circle< void > context )
{
    EGInstance var_1 = context.data.instance;
    return g_root_Circle[ var_1 ].m_position;
}

template<>
__interface_root< void >::__interface_Circle< void >::__interface_size< void >::Read
inline __invoke_impl
<
    __interface_root< void >::__interface_Circle< void >::__interface_size< void >::Read,
    __interface_root< void >::__interface_Circle< void >,
    __eg_type_path< size >,
    Read
>
( __interface_root< void >::__interface_Circle< void > context )
{
    EGInstance var_1 = context.data.instance;
    return g_root_Circle[ var_1 ].m_size;
}

template<>
__interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >::__interface_f< void >::Read
inline __invoke_impl
<
    __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >::__interface_f< void >::Read,
    __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >,
    __eg_type_path< f >,
    Read
>
( __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > context )
{
    EGInstance var_1 = context.data.instance;
    return g_root_Circle_ColorChanger[ var_1 ].m_f;
}

template<>
void
inline __invoke_impl
<
    void,
    __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >,
    __eg_type_path< f >,
    Write,
__interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >::__interface_f< void >::Write
>
( __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > context,__interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >::__interface_f< void >::Write value )
{
    EGInstance var_1 = context.data.instance;
    g_root_Circle_ColorChanger[ var_1 ].m_f = value;
}

template<>
void
inline __invoke_impl
<
    void,
    __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >,
    __eg_type_path< color >,
    Write,
__interface_root< void >::__interface_Circle< void >::__interface_color< void >::Write
>
( __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > context,__interface_root< void >::__interface_Circle< void >::__interface_color< void >::Write value )
{
    EGInstance var_2 = context.data.instance;
    g_root_Circle[ var_2 ].m_color = value;
}

template<>
__interface_root< void >::__interface_Circle< void >::EGRangeType
inline __invoke_impl
<
    __interface_root< void >::__interface_Circle< void >::EGRangeType,
    __interface_root< void >::__interface_Spiral< void >,
    __eg_type_path< __interface_root< void >::__interface_Circle< void >,Range >,
    Range
>
( __interface_root< void >::__interface_Spiral< void > context )
{
    const EGInstance iBegin = context.data.instance * 2048;
    const EGInstance iEnd = ( context.data.instance + 1 ) * 2048;
    __interface_root< void >::__interface_Circle< void >::Iterator begin( iBegin, iEnd );
    __interface_root< void >::__interface_Circle< void >::Iterator end( iEnd, iEnd );
    while( begin != end )
    {
        if( g_root_Circle[ begin.instance ].g_root_Circle_timestamp_runnning <= clock::subcycle() )
        {
            break;
        }
        ++begin.instance;
    }
    return __interface_root< void >::__interface_Circle< void >::EGRangeType( begin, end );
}

template<>
void
inline __invoke_impl
<
    void,
    __interface_root< void >::__interface_Circle< void >,
    __eg_type_path< position >,
    Write,
__interface_root< void >::__interface_Circle< void >::__interface_position< void >::Write
>
( __interface_root< void >::__interface_Circle< void > context,__interface_root< void >::__interface_Circle< void >::__interface_position< void >::Write value )
{
    EGInstance var_1 = context.data.instance;
    g_root_Circle[ var_1 ].m_position = value;
}

template<>
void
inline __invoke_impl
<
    void,
    __interface_root< void >::__interface_Circle< void >,
    __eg_type_path< size >,
    Write,
__interface_root< void >::__interface_Circle< void >::__interface_size< void >::Write
>
( __interface_root< void >::__interface_Circle< void > context,__interface_root< void >::__interface_Circle< void >::__interface_size< void >::Write value )
{
    EGInstance var_1 = context.data.instance;
    g_root_Circle[ var_1 ].m_size = value;
}

//generic variant invocation adaptor
template< typename... Ts >
template< typename TypePath, typename Operation, typename... Args >
typename result_type< __eg_variant< Ts... >, TypePath, Operation >::Type
__eg_variant< Ts... >::invoke( Args... args )
{
    using CanonicalTypePathType = typename __eg_CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename result_type< __eg_variant< Ts... >, TypePath, Operation >::Type, __eg_variant< Ts... >, CanonicalTypePathType, Operation >( *this, args... );
}

//root
template<>
struct eg_is_convertible< __interface_root< void >, __interface_root< void > >
{
    static constexpr const bool value = true;
};
template< typename __EGInterface1 >
template< typename TFrom >
__interface_root< __EGInterface1 >::__interface_root( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 18: //root
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
}
template< typename __EGInterface1 >
template< typename TFrom >
__interface_root< __EGInterface1 >& __interface_root< __EGInterface1 >::operator=( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 18: //root
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template< typename __EGInterface1 >
EGEvent __interface_root< __EGInterface1 >::get_next_event() const
{
    EGEvent ev;
    __eg_event _event;
    while( g_eg_event_log->GetEvent( g_root[ data.instance ].g_root_event_iter, _event ) )
    {
         if( 0 == strcmp( _event.type, "stop" ) )
         {
             ev.data = *reinterpret_cast< const __eg_reference* >( _event.value );
             break;
         }
    }
    return ev;
}
template< typename __EGInterface1 >
__interface_root< __EGInterface1 >::operator const void*() const
{
      if( data.timestamp != EG_INVALID_TIMESTAMP )
      {
          return reinterpret_cast< const void* >( &data );
      }
      else
      {
          return nullptr;
      }
}
template< typename __EGInterface1 >
typename __interface_root< __EGInterface1 >::Iterator& __interface_root< __EGInterface1 >::Iterator::operator++()
{
    while( instance != sentinal )
    {
        ++instance;
        if( g_root[ instance ].g_root_timestamp_runnning < clock::subcycle() )
        {
            break;
        }
    }
    return *this;
}

template< typename __EGInterface1 >
__interface_root< __EGInterface1 >& __interface_root< __EGInterface1 >::Iterator::operator*()
{
    return g_root[ instance ].g_root_reference;
}

//root::Circle
template<>
struct eg_is_convertible< __interface_root< void >::__interface_Circle< void >, __interface_root< void >::__interface_Circle< void > >
{
    static constexpr const bool value = true;
};
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TFrom >
__interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_Circle( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Circle< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 19: //Circle
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TFrom >
typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >& __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::operator=( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Circle< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 19: //Circle
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
EGEvent __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::get_next_event() const
{
    EGEvent ev;
    __eg_event _event;
    while( g_eg_event_log->GetEvent( g_root_Circle[ data.instance ].g_root_Circle_event_iter, _event ) )
    {
         if( 0 == strcmp( _event.type, "stop" ) )
         {
             ev.data = *reinterpret_cast< const __eg_reference* >( _event.value );
             break;
         }
    }
    return ev;
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
__interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::operator const void*() const
{
      if( data.timestamp != EG_INVALID_TIMESTAMP )
      {
          return reinterpret_cast< const void* >( &data );
      }
      else
      {
          return nullptr;
      }
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >::Iterator& __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::Iterator::operator++()
{
    while( instance != sentinal )
    {
        ++instance;
        if( g_root_Circle[ instance ].g_root_Circle_timestamp_runnning < clock::subcycle() )
        {
            break;
        }
    }
    return *this;
}

template< typename __EGInterface1 >
template< typename __EGInterface2 >
typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >& __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::Iterator::operator*()
{
    return g_root_Circle[ instance ].g_root_Circle_reference;
}

//root::Circle::ColorChanger
template<>
struct eg_is_convertible< __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >, __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > >
{
    static constexpr const bool value = true;
};
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename __EGInterface3 >
template< typename TFrom >
__interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >::__interface_ColorChanger( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 24: //ColorChanger
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename __EGInterface3 >
template< typename TFrom >
typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >::template __interface_ColorChanger< __EGInterface3 >& __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >::operator=( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 24: //ColorChanger
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename __EGInterface3 >
EGEvent __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >::get_next_event() const
{
    EGEvent ev;
    __eg_event _event;
    while( g_eg_event_log->GetEvent( g_root_Circle_ColorChanger[ data.instance ].g_root_Circle_ColorChanger_event_iter, _event ) )
    {
         if( 0 == strcmp( _event.type, "stop" ) )
         {
             ev.data = *reinterpret_cast< const __eg_reference* >( _event.value );
             break;
         }
    }
    return ev;
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename __EGInterface3 >
__interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >::operator const void*() const
{
      if( data.timestamp != EG_INVALID_TIMESTAMP )
      {
          return reinterpret_cast< const void* >( &data );
      }
      else
      {
          return nullptr;
      }
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename __EGInterface3 >
typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >::template __interface_ColorChanger< __EGInterface3 >::Iterator& __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >::Iterator::operator++()
{
    while( instance != sentinal )
    {
        ++instance;
        if( g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_timestamp_runnning < clock::subcycle() )
        {
            break;
        }
    }
    return *this;
}

template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename __EGInterface3 >
typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >::template __interface_ColorChanger< __EGInterface3 >& __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >::Iterator::operator*()
{
    return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_reference;
}

//root::Spiral
template<>
struct eg_is_convertible< __interface_root< void >::__interface_Spiral< void >, __interface_root< void >::__interface_Spiral< void > >
{
    static constexpr const bool value = true;
};
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TFrom >
__interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >::__interface_Spiral( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Spiral< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 27: //Spiral
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TFrom >
typename __interface_root< __EGInterface1 >::template __interface_Spiral< __EGInterface2 >& __interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >::operator=( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Spiral< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 27: //Spiral
         data = from.data;
         break;
     default:
         data.timestamp = EG_INVALID_TIMESTAMP;
         break;
  }
  return *this;
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
EGEvent __interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >::get_next_event() const
{
    EGEvent ev;
    __eg_event _event;
    while( g_eg_event_log->GetEvent( g_root_Spiral[ data.instance ].g_root_Spiral_event_iter, _event ) )
    {
         if( 0 == strcmp( _event.type, "stop" ) )
         {
             ev.data = *reinterpret_cast< const __eg_reference* >( _event.value );
             break;
         }
    }
    return ev;
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
__interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >::operator const void*() const
{
      if( data.timestamp != EG_INVALID_TIMESTAMP )
      {
          return reinterpret_cast< const void* >( &data );
      }
      else
      {
          return nullptr;
      }
}
template< typename __EGInterface1 >
template< typename __EGInterface2 >
typename __interface_root< __EGInterface1 >::template __interface_Spiral< __EGInterface2 >::Iterator& __interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >::Iterator::operator++()
{
    while( instance != sentinal )
    {
        ++instance;
        if( g_root_Spiral[ instance ].g_root_Spiral_timestamp_runnning < clock::subcycle() )
        {
            break;
        }
    }
    return *this;
}

template< typename __EGInterface1 >
template< typename __EGInterface2 >
typename __interface_root< __EGInterface1 >::template __interface_Spiral< __EGInterface2 >& __interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >::Iterator::operator*()
{
    return g_root_Spiral[ instance ].g_root_Spiral_reference;
}

//root
template< typename __EGInterface1 >
template< typename TypePath, typename Operation, typename... Args >
typename result_type< __interface_root< __EGInterface1 >, TypePath, Operation >::Type
__interface_root< __EGInterface1 >::invoke( Args... args )
{
    using CanonicalTypePathType = typename __eg_CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename result_type< __interface_root< __EGInterface1 >, TypePath, Operation >::Type, __interface_root< __EGInterface1 >, CanonicalTypePathType, Operation >( *this, args... );
}

//root::Circle
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TypePath, typename Operation, typename... Args >
typename result_type< typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >, TypePath, Operation >::Type
__interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::invoke( Args... args )
{
    using CanonicalTypePathType = typename __eg_CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename result_type< __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >, TypePath, Operation >::Type, __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >, CanonicalTypePathType, Operation >( *this, args... );
}

//root::Circle::ColorChanger
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename __EGInterface3 >
template< typename TypePath, typename Operation, typename... Args >
typename result_type< typename __interface_root< __EGInterface1 >::template __interface_Circle< __EGInterface2 >::template __interface_ColorChanger< __EGInterface3 >, TypePath, Operation >::Type
__interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >::invoke( Args... args )
{
    using CanonicalTypePathType = typename __eg_CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename result_type< __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >, TypePath, Operation >::Type, __interface_root< __EGInterface1 >::__interface_Circle< __EGInterface2 >::__interface_ColorChanger< __EGInterface3 >, CanonicalTypePathType, Operation >( *this, args... );
}

//root::Spiral
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TypePath, typename Operation, typename... Args >
typename result_type< typename __interface_root< __EGInterface1 >::template __interface_Spiral< __EGInterface2 >, TypePath, Operation >::Type
__interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >::invoke( Args... args )
{
    using CanonicalTypePathType = typename __eg_CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename result_type< __interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >, TypePath, Operation >::Type, __interface_root< __EGInterface1 >::__interface_Spiral< __EGInterface2 >, CanonicalTypePathType, Operation >( *this, args... );
}


