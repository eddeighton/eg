
//buffers

//Buffer: b_root stride: 40 size: 1
struct b_root
{
    EGTimeStamp g_root_timestamp_runnning;
    EGTimeStamp g_root_timestamp_paused;
    EGCoroutine g_root_coroutine;
    __eg_event_iterator g_root_event_iter;
    __interface_root< void > g_root_reference;
    EGTimeStamp g_root_timestamp_stopped;
};
static std::array< b_root, 1 > g_root;

void allocate_buffers()
{
    for( EGInstance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_timestamp_runnning = EG_INVALID_TIMESTAMP;
        g_root[ i ].g_root_timestamp_paused = EG_INVALID_TIMESTAMP;
        g_root[ i ].g_root_reference = __interface_root< void >{ i, 0 };
        g_root[ i ].g_root_timestamp_stopped = 0U;
    }
}

void deallocate_buffers()
{
    for( EGInstance i = 0U; i != 1; ++i )
    {
    }
}


    
//Event log impl
struct __eg_event
{
    const char* type;
    std::size_t timestamp;
    const void* value;
    std::size_t size;
};

struct __eg_event_log
{
    virtual __eg_event_iterator GetEventIterator() = 0;
    virtual bool GetEvent( __eg_event_iterator& iterator, __eg_event& event ) = 0;
    virtual void PutEvent( const __eg_event& event ) = 0;
};
    
struct BasicHostEventLog : public __eg_event_log
{
    BasicHostEventLog( const IPC::PID& hostPID, const boost::filesystem::path& filePath )
        :   m_eventLog( hostPID, filePath )
    {
    }
    
    
    virtual __eg_event_iterator GetEventIterator()
    {
        return m_eventLog.head();
    }
    
    virtual bool GetEvent( __eg_event_iterator& iterator, __eg_event& event )
    {
        IPC::Event::Event readEvent;
        if( m_eventLog.read( iterator, readEvent ) )
        {
            event.timestamp     = readEvent.getTimeStamp();
            event.type          = readEvent.getType_c_str();
            event.value         = readEvent.getValue();
            event.size          = readEvent.getValueSize();
            return true;
        }
        else
        {
            return false;
        }
    }
    
    virtual void PutEvent( const __eg_event& event )
    {
        m_eventLog.write( IPC::Event::Event( event.type, 
            strlen( event.type ), event.timestamp, event.value, event.size ) );
    }
    
    bool updateAndWasEvent()
    {
        return m_eventLog.updateHead();
    }
    
    IPC::Event::Server m_eventLog;
};
    
//clock implementation

    
struct __eg_clock
{
    virtual EGTimeStamp cycle()    const = 0;
    virtual EGTimeStamp subcycle() const = 0;
    virtual float ct()     const = 0;
    virtual float dt()     const = 0;
};

//Basic Host clock implementation using std::chrono
struct BasicHostClock : public __eg_clock
{
public:
    typedef std::chrono::steady_clock ClockType;
    typedef ClockType::time_point Tick;
    typedef ClockType::duration TickDuration;
    typedef std::chrono::duration< float, std::ratio< 1 > > FloatTickDuration;
    
    BasicHostClock()
    {
        m_lastTick = m_startTick = ClockType::now();
        m_cycle = m_subcycle = m_cycleSubCycle = 0;
        m_ct = m_dt = 0.0f;
    }
    
    inline Tick nextCycle()
    {
        const Tick nowTick = ClockType::now();
        m_dt = FloatTickDuration( nowTick - m_lastTick  ).count();
        m_ct = FloatTickDuration( nowTick - m_startTick ).count();
        
        m_lastTick = nowTick;
        
        nextSubCycle();
        m_cycleSubCycle = m_subcycle;
        
        ++m_cycle;
        
        return nowTick;
    }
    
    inline void nextSubCycle()
    {
        ++m_subcycle;
    }
    
    inline std::size_t cycleSubCycles() const { return m_cycleSubCycle; }
    
    Tick actual() const { return ClockType::now(); }
    
    //interface
    virtual EGTimeStamp cycle()    const { return m_cycle; }
    virtual EGTimeStamp subcycle() const { return m_subcycle; }
    virtual float ct()     const { return m_ct; }
    virtual float dt()     const { return m_dt; }
    
private:
    Tick m_lastTick, m_startTick;
    EGTimeStamp m_cycle, m_subcycle, m_cycleSubCycle;
    float m_ct, m_dt;
};
    
//initialiser
extern void initialise( EGDependencyProvider* pDependencyProvider );

//Action functions
extern __interface_root< void > root_starter( EGInstance _gid );
extern void root_stopper( EGInstance _gid );
extern bool root_executor();

//Dependency Provider Implementation
struct BasicHost_EGDependencyProvider : public EGDependencyProvider
{
     __eg_clock* m_pClock;
     __eg_event_log* m_pEventLog;

     BasicHost_EGDependencyProvider( __eg_clock* pClock, __eg_event_log* pEventLog )
         :   m_pClock( pClock ),
             m_pEventLog( pEventLog )
     {

     }

    virtual void* getBuffer( const char* pszName )
    {
        if( 0U == strcmp( pszName, "g_root" ) ) return g_root.data();
        return nullptr;
    }
    virtual void* getInterface( const char* pszName )
    {
        if( 0U == strcmp( pszName, "__eg_clock" ) ) return m_pClock;
        if( 0U == strcmp( pszName, "__eg_event_log" ) ) return m_pEventLog;
        return nullptr;
    }
};


bool executeSchedule()
{
    bool bWaited = false;
    bWaited = root_executor() || bWaited;
    return bWaited;
}


std::string strEventLogFolder = "test1_log";


    
int main( int argc, const char* argv[] )
{
    //try
    //{
        BasicHostClock::TickDuration sleepDuration = std::chrono::milliseconds( 10 );
        if( argc > 1 )
        {
            sleepDuration = std::chrono::milliseconds( atoi( argv[ 1 ] ) );
        }
        
        //wait for input 
        if( argc > 2 )
        {
            char c;
            std::cin >> c;
        }
        
        IPC::PID thePID;
        
        //allocate everything
        allocate_buffers();
        
        //setup the dependencies
        const boost::filesystem::path eventLogPath = 
            boost::filesystem::current_path() / std::string( strEventLogFolder );
        
        //std::cout << "pid: " << thePID << std::endl;
        //std::cout << "log: " << eventLogPath.generic_string() << std::endl;
        
        BasicHostClock theClock;
        BasicHostEventLog theEventLog( thePID, eventLogPath );
        
        BasicHost_EGDependencyProvider dependencies( &theClock, &theEventLog );
        initialise( &dependencies );
        
        //start the root
        root_starter( 0 );
        
        while( g_root[ 0 ].g_root_timestamp_runnning <= clock::subcycle() )
        {
            const BasicHostClock::Tick cycleStart = theClock.nextCycle();
            
            if( g_root[ 0 ].g_root_timestamp_paused <= clock::subcycle() )
            {
                //run the subcycle
                bool bMoreSubCycles = true;
                while( bMoreSubCycles )
                {
                    const bool bWasThereAWait = executeSchedule();
                    theClock.nextSubCycle();
                    if( !bWasThereAWait && !theEventLog.updateAndWasEvent() )
                        bMoreSubCycles = false;
                }
            }
            
            //actually sleep...
            {
                const BasicHostClock::TickDuration elapsed = theClock.actual() - cycleStart;
                if( elapsed < sleepDuration )
                {
                    std::this_thread::sleep_for( sleepDuration - elapsed );
                }
            }
        }
        
    //}
    //catch( std::exception& e )
    //{
    //    std::cerr << "Error: " << e.what() << std::endl;
    //    deallocate_buffers();
    //}
    
    return 0;
    
}
    
    
