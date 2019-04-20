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
    std::atomic< std::uint64_t > g_rootTest_alloc_iter;
};
static b_root *g_root;

//Buffer: b_root_Test size: 1
struct b_root_Test
{
    EGTimeStamp g_root_Test_timestamp_runnning;
    EGTimeStamp g_root_Test_timestamp_paused;
    EGCoroutine g_root_Test_coroutine;
    __eg_event_iterator g_root_Test_event_iter;
    __interface_root< void >::__interface_Test< void > g_root_Test_reference;
    EGTimeStamp g_root_Test_timestamp_stopped;
    EGInstance g_root_Test_alloc_data;
};
static b_root_Test *g_root_Test;

//input::Action function forward declarations

__interface_root< void > root_starter( EGInstance _gid );
void root_stopper( EGInstance _gid );
bool root_executor();

__interface_root< void >::__interface_Test< void > root_Test_starter( EGInstance _gid );
void root_Test_stopper( EGInstance _gid );
bool root_Test_executor();


     
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
    g_root_Test = reinterpret_cast< b_root_Test* >( pDependencyProvider->getBuffer( "g_root_Test" ) );
    //global dependencies
    g_eg_clock = reinterpret_cast< __eg_clock* >( pDependencyProvider->getInterface( "__eg_clock" ) );
    g_eg_event_log = reinterpret_cast< __eg_event_log* >( pDependencyProvider->getInterface( "__eg_event_log" ) );
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
         for( EGInstance childIndex = _gid * 1; childIndex != ( _gid + 1 ) * 1; ++childIndex )
         {
             root_Test_stopper( childIndex );
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


__interface_root< void >::__interface_Test< void > root_Test_starter( EGInstance _gid )
{
    //claim next free index
    EGIterator iter, expected;
    while( true )
    {
         iter = EGIterator( g_root[ _gid ].g_rootTest_alloc_iter.load() );
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
         EGInstance nextInstance = g_root_Test[ _gid * 1 + nextCellIndex ].g_root_Test_alloc_data;
         if( g_root_Test[ nextInstance ].g_root_Test_timestamp_stopped <= clock::subcycle() )
         {
             //attempt to set the atomic
             if( g_root[ _gid ].g_rootTest_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
             {
                 //successfully claimed index so get the actual instance from the ring buffer
                 const EGInstance startSubCycle = clock::subcycle() + 1;
                 __interface_root< void >::__interface_Test< void >& reference = g_root_Test[ nextInstance ].g_root_Test_reference;
                 reference.data.timestamp = startSubCycle;
                 g_root_Test[ nextInstance ].g_root_Test_timestamp_runnning = startSubCycle;
                 g_root_Test[ nextInstance ].g_root_Test_timestamp_paused = startSubCycle;
                 __eg_event ev = { "start", startSubCycle, &reference.data, sizeof( __eg_reference ) };
                 g_eg_event_log->PutEvent( ev );
                 g_root_Test[ nextInstance ].g_root_Test_event_iter = g_eg_event_log->GetEventIterator();
                 g_root_Test[ nextInstance ].g_root_Test_coroutine = reference();
                 return reference;
             }
         }
         else
         {
             break;
         }
    }
    //failure return null handle
    __interface_root< void >::__interface_Test< void > nullInstance;
    return nullInstance;
}

void root_Test_stopper( EGInstance _gid )
{
     EGInstance _parent_id = _gid / 1;
     if( g_root_Test[ _gid ].g_root_Test_timestamp_runnning != EG_INVALID_TIMESTAMP )
     {
         EGIterator iter, expected;
         while( true )
         {
              iter = EGIterator( g_root[ _parent_id ].g_rootTest_alloc_iter.load() );
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
              if( g_root[ _parent_id ].g_rootTest_alloc_iter.compare_exchange_weak( expected.data, iter.data ) )
              {
                  //successfully freed index
                  g_root_Test[ freeCellIndex ].g_root_Test_alloc_data = _gid;
                  if( iter.protection )
                  {
                      //turn off the protection bit
                      expected = iter;
                      iter.protection = 0;
                      while( g_root[ _parent_id ].g_rootTest_alloc_iter.compare_exchange_weak( expected.data, iter.data ) );
                  }
                  break;
              }
         }
         g_root_Test[ _gid ].g_root_Test_timestamp_runnning = EG_INVALID_TIMESTAMP;
         g_root_Test[ _gid ].g_root_Test_timestamp_paused = EG_INVALID_TIMESTAMP;
         g_root_Test[ _gid ].g_root_Test_timestamp_stopped = clock::subcycle() + 2U;
         g_root_Test[ _gid ].g_root_Test_coroutine.destroy();
         __eg_event ev = { "stop", clock::subcycle(), &g_root_Test[ _gid ].g_root_Test_reference, sizeof( __eg_reference ) };
         g_eg_event_log->PutEvent( ev );
     }
}

bool root_Test_executor()
{
    const EGTimeStamp subcycle = clock::subcycle();
    for( EGInstance i = 0; i != 1; ++i )
    {
        if( g_root_Test[ i ].g_root_Test_timestamp_runnning <= subcycle )
        {
             if( g_root_Test[ i ].g_root_Test_timestamp_paused <= subcycle )
             {
                 if( g_root_Test[ i ].g_root_Test_coroutine.done() )
                 {
                     root_Test_stopper( i );
                 }
                 else
                 {
                     g_root_Test[ i ].g_root_Test_coroutine.resume();
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
__interface_root< void >::__interface_Test< void >
inline __invoke_impl
<
    __interface_root< void >::__interface_Test< void >,
    __interface_root< void >,
    __eg_type_path< __interface_root< void >::__interface_Test< void >,Start >,
    Start
>
( __interface_root< void > context )
{
    return root_Test_starter( context.data.instance );
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
     case 6: //root
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
      case 6: //root
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

//root::Test
template<>
struct eg_is_convertible< __interface_root< void >::__interface_Test< void >, __interface_root< void >::__interface_Test< void > >
{
    static constexpr const bool value = true;
};
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TFrom >
__interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >::__interface_Test( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Test< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
     case 8: //Test
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
typename __interface_root< __EGInterface1 >::template __interface_Test< __EGInterface2 >& __interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >::operator=( const TFrom& from )
{
  static_assert( eg_is_convertible< TFrom, __interface_root< void >::__interface_Test< void > >::value, "Incompatible eg type conversion" );
  switch( from.data.type )
  {
      case 8: //Test
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
EGEvent __interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >::get_next_event() const
{
    EGEvent ev;
    __eg_event _event;
    while( g_eg_event_log->GetEvent( g_root_Test[ data.instance ].g_root_Test_event_iter, _event ) )
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
__interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >::operator const void*() const
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
typename __interface_root< __EGInterface1 >::template __interface_Test< __EGInterface2 >::Iterator& __interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >::Iterator::operator++()
{
    while( instance != sentinal )
    {
        ++instance;
        if( g_root_Test[ instance ].g_root_Test_timestamp_runnning < clock::subcycle() )
        {
            break;
        }
    }
    return *this;
}

template< typename __EGInterface1 >
template< typename __EGInterface2 >
typename __interface_root< __EGInterface1 >::template __interface_Test< __EGInterface2 >& __interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >::Iterator::operator*()
{
    return g_root_Test[ instance ].g_root_Test_reference;
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

//root::Test
template< typename __EGInterface1 >
template< typename __EGInterface2 >
template< typename TypePath, typename Operation, typename... Args >
typename result_type< typename __interface_root< __EGInterface1 >::template __interface_Test< __EGInterface2 >, TypePath, Operation >::Type
__interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >::invoke( Args... args )
{
    using CanonicalTypePathType = typename __eg_CanonicaliseTypePath< TypePath >::Type;
    return __invoke_impl< typename result_type< __interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >, TypePath, Operation >::Type, __interface_root< __EGInterface1 >::__interface_Test< __EGInterface2 >, CanonicalTypePathType, Operation >( *this, args... );
}


