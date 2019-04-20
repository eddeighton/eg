
//buffers

//Buffer: b_root stride: 56 size: 1
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
static std::array< b_root, 1 > g_root;

//Buffer: b_root_Circle stride: 20054287 size: 2048
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
static std::array< b_root_Circle, 2048 > g_root_Circle;

//Buffer: b_root_Circle_ColorChanger stride: 6684835 size: 2048
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
static std::array< b_root_Circle_ColorChanger, 2048 > g_root_Circle_ColorChanger;

//Buffer: b_root_Spiral stride: 44 size: 1
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
static std::array< b_root_Spiral, 1 > g_root_Spiral;

void allocate_buffers()
{
    for( EGInstance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_timestamp_runnning = EG_INVALID_TIMESTAMP;
        g_root[ i ].g_root_timestamp_paused = EG_INVALID_TIMESTAMP;
        g_root[ i ].g_root_reference = __interface_root< void >{ i, 0 };
        g_root[ i ].g_root_timestamp_stopped = 0U;
        g_root[ i ].g_rootCircle_alloc_iter = 0UL;
        g_root[ i ].g_rootSpiral_alloc_iter = 0UL;
    }
    for( EGInstance i = 0U; i != 2048; ++i )
    {
        g_root_Circle[ i ].g_root_Circle_timestamp_runnning = EG_INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_root_Circle_timestamp_paused = EG_INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_root_Circle_reference = __interface_root< void >::__interface_Circle< void >{ i, 0 };
        g_root_Circle[ i ].g_root_Circle_timestamp_stopped = 0U;
        g_root_Circle[ i ].g_position_timestamp = EG_INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_color_timestamp = EG_INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_size_timestamp = EG_INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_root_Circle_alloc_data = i;
        g_root_Circle[ i ].g_root_CircleColorChanger_alloc_iter = 0UL;
    }
    for( EGInstance i = 0U; i != 2048; ++i )
    {
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_timestamp_runnning = EG_INVALID_TIMESTAMP;
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_timestamp_paused = EG_INVALID_TIMESTAMP;
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_reference = __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void >{ i, 0 };
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_timestamp_stopped = 0U;
        g_root_Circle_ColorChanger[ i ].g_f_timestamp = EG_INVALID_TIMESTAMP;
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_alloc_data = i;
    }
    for( EGInstance i = 0U; i != 1; ++i )
    {
        g_root_Spiral[ i ].g_root_Spiral_timestamp_runnning = EG_INVALID_TIMESTAMP;
        g_root_Spiral[ i ].g_root_Spiral_timestamp_paused = EG_INVALID_TIMESTAMP;
        g_root_Spiral[ i ].g_root_Spiral_reference = __interface_root< void >::__interface_Spiral< void >{ i, 0 };
        g_root_Spiral[ i ].g_root_Spiral_timestamp_stopped = 0U;
        g_root_Spiral[ i ].g_root_Spiral_alloc_data = i;
    }
}

void deallocate_buffers()
{
    for( EGInstance i = 0U; i != 1; ++i )
    {
    }
    for( EGInstance i = 0U; i != 2048; ++i )
    {
    }
    for( EGInstance i = 0U; i != 2048; ++i )
    {
    }
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
    
struct CinderHostEventLog : public __eg_event_log
{
    CinderHostEventLog( const IPC::PID& hostPID, const boost::filesystem::path& filePath )
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
struct CinderHostClock : public __eg_clock
{
public:
    typedef std::chrono::steady_clock ClockType;
    typedef ClockType::time_point Tick;
    typedef ClockType::duration TickDuration;
    typedef std::chrono::duration< float, std::ratio< 1 > > FloatTickDuration;
    
    CinderHostClock()
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
    
//clock implementation

 /*   
struct __eg_input
{
    virtual std::optional< cinder::app::MouseEvent > getMouseEvent() = 0;
    virtual std::optional< cinder::app::KeyEvent > getKeyEvent() = 0;
};
*/
class InputEvents : public __eg_input
{
public:
    virtual std::optional< cinder::app::MouseEvent > getMouseEvent()
    {
        std::optional< MouseEvent > event;
        if( !mouseEvents.empty() )
        {
            event = mouseEvents.back();
            mouseEvents.pop_back();
        }
        return event;
    }
    
    virtual std::optional< cinder::app::KeyEvent > getKeyEvent()
    {
        std::optional< KeyEvent > event;
        if( !keyEvents.empty() )
        {
            event = keyEvents.back();
            keyEvents.pop_back();
        }
        return event;
    }
    
    std::vector< cinder::app::MouseEvent > mouseEvents;
    std::vector< cinder::app::KeyEvent > keyEvents;
};

    
    //initialiser
extern void initialise( EGDependencyProvider* pDependencyProvider );

//Action functions
extern __interface_root< void > root_starter( EGInstance _gid );
extern void root_stopper( EGInstance _gid );
extern bool root_executor();
extern __interface_root< void >::__interface_Circle< void > root_Circle_starter( EGInstance _gid );
extern void root_Circle_stopper( EGInstance _gid );
extern bool root_Circle_executor();
extern __interface_root< void >::__interface_Spiral< void > root_Spiral_starter( EGInstance _gid );
extern void root_Spiral_stopper( EGInstance _gid );
extern bool root_Spiral_executor();
extern __interface_root< void >::__interface_Circle< void >::__interface_ColorChanger< void > root_Circle_ColorChanger_starter( EGInstance _gid );
extern void root_Circle_ColorChanger_stopper( EGInstance _gid );
extern bool root_Circle_ColorChanger_executor();

//Dependency Provider Implementation
struct CinderHost_EGDependencyProvider : public EGDependencyProvider
{
     __eg_clock* m_pClock;
     __eg_event_log* m_pEventLog;
     __eg_input* m_pInput;

     CinderHost_EGDependencyProvider( __eg_clock* pClock, __eg_event_log* pEventLog, __eg_input* pInput )
         :   m_pClock( pClock ),
             m_pEventLog( pEventLog ),
             m_pInput( pInput )
     {

     }

    virtual void* getBuffer( const char* pszName )
    {
        if( 0U == strcmp( pszName, "g_root" ) ) return g_root.data();
        if( 0U == strcmp( pszName, "g_root_Circle" ) ) return g_root_Circle.data();
        if( 0U == strcmp( pszName, "g_root_Circle_ColorChanger" ) ) return g_root_Circle_ColorChanger.data();
        if( 0U == strcmp( pszName, "g_root_Spiral" ) ) return g_root_Spiral.data();
        return nullptr;
    }
    virtual void* getInterface( const char* pszName )
    {
        if( 0U == strcmp( pszName, "__eg_clock" ) ) return m_pClock;
        if( 0U == strcmp( pszName, "__eg_event_log" ) ) return m_pEventLog;
        if( 0U == strcmp( pszName, "__eg_input" ) ) return m_pInput;
        return nullptr;
    }
};


bool executeSchedule()
{
    bool bWaited = false;
    bWaited = root_executor() || bWaited;
    bWaited = root_Circle_executor() || bWaited;
    bWaited = root_Spiral_executor() || bWaited;
    bWaited = root_Circle_ColorChanger_executor() || bWaited;
    return bWaited;
}


std::string strEventLogFolder = "circles_log";


    
using namespace ci;
using namespace ci::app;


// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App 
{
public:
    BasicApp();
    ~BasicApp();
    
    void update() override;
    
    // event handlers
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseWheel( MouseEvent event ) override;
    void mouseMove( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    
    void keyDown( KeyEvent event ) override;
    void keyUp( KeyEvent event ) override;

	// Cinder will call 'draw' each time the contents of the window need to be redrawn.
	void draw() override;

private:
    CinderHostClock::TickDuration sleepDuration = std::chrono::milliseconds( 10 );
    IPC::PID thePID;
    CinderHostClock theClock;
    CinderHostEventLog theEventLog;
    InputEvents inputEvents;
    CinderHost_EGDependencyProvider dependencies;
};


BasicApp::BasicApp()
    :   theEventLog( thePID, boost::filesystem::current_path() / std::string( strEventLogFolder ) ),
        dependencies( &theClock, &theEventLog, &inputEvents )
{
    allocate_buffers();
    
    initialise( &dependencies );
    
    root_starter( 0 );
}
BasicApp::~BasicApp()
{
    
    deallocate_buffers();
}

void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void BasicApp::update()
{
    if( g_root[ 0 ].g_root_timestamp_runnning > clock::subcycle() )
    {
        quit();
    }
}

void BasicApp::mouseDown( MouseEvent event )
{
    inputEvents.mouseEvents.push_back( event );
}
void BasicApp::mouseUp( MouseEvent event )
{
    inputEvents.mouseEvents.push_back( event );
}
void BasicApp::mouseWheel( MouseEvent event )
{
    inputEvents.mouseEvents.push_back( event );
}
void BasicApp::mouseMove( MouseEvent event )
{
    inputEvents.mouseEvents.push_back( event );
}
void BasicApp::mouseDrag( MouseEvent event )
{
    inputEvents.mouseEvents.push_back( event );
}

void BasicApp::keyDown( KeyEvent event )
{
    inputEvents.keyEvents.push_back( event );
}
void BasicApp::keyUp( KeyEvent event )
{
    inputEvents.keyEvents.push_back( event );
}

void BasicApp::draw()
{
	// Clear the contents of the window. This call will clear
	// both the color and depth buffers. 
	//gl::clear( Color::gray( 0.1f ) );
    //
	//// Set the current draw color to orange by setting values for
	//// red, green and blue directly. Values range from 0 to 1.
	//// See also: gl::ScopedColor
	//gl::color( 1.0f, 0.5f, 0.25f );
    
    const CinderHostClock::Tick cycleStart = theClock.nextCycle();
    
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
}


// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )

    