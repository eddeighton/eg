#include "structures.hpp"

extern std::vector< std::function< void() > > loadPythonScripts( const std::vector< std::string >& scripts, const std::string& strDatabaseFile );
std::vector< std::function< void() > > loadScripts( const std::vector< std::string >& scripts, const std::string& strDatabase )
{
    return loadPythonScripts( scripts, strDatabase );
}
    

struct HostClock
{
public:
    typedef std::chrono::steady_clock ClockType;
    typedef ClockType::time_point Tick;
    typedef ClockType::duration TickDuration;
    typedef std::chrono::duration< float, std::ratio< 1 > > FloatTickDuration;
    
    HostClock()
    {
        m_lastTick = m_startTick = ClockType::now();
        m_cycle = 1U;
        m_ct = m_dt = 0.0f;
    }
    
    inline Tick nextCycle()
    {
        const Tick nowTick = ClockType::now();
        m_dt = FloatTickDuration( nowTick - m_lastTick  ).count();
        m_ct = FloatTickDuration( nowTick - m_startTick ).count();
        m_lastTick = nowTick;
        ++m_cycle;
        return nowTick;
    }
    
    inline Tick actual()           const { return ClockType::now(); }
    inline eg::TimeStamp cycle()   const { return m_cycle; }
    inline float ct()              const { return m_ct; }
    inline float dt()              const { return m_dt; }
    
private:
    Tick m_lastTick, m_startTick;
    eg::TimeStamp m_cycle;
    float m_ct, m_dt;
} theClock;

eg::TimeStamp clock::cycle()
{
    return theClock.cycle();
}
float clock::ct()
{
    return theClock.ct();
}
float clock::dt()
{
    return theClock.dt();
}



std::unique_ptr< eg::EventLogServer > g_eventLogServer( eg::EventLogServer::create( "log" ) );

eg::event_iterator events::getIterator()
{
    return g_eventLogServer->head();
}

bool events::get( eg::event_iterator& iterator, Event& event )
{
    const char* type;
    eg::TimeStamp timestamp;
    const void* value;
    std::size_t size;
    while( g_eventLogServer->read( iterator, type, timestamp, value, size ) )
    {
        if( 0U == strcmp( type, "stop" ) )
        {
            event.data = *reinterpret_cast< const eg::reference* >( value );
            return true;
        }
    }  
    return false;
}

void events::put( const char* type, eg::TimeStamp timestamp, const void* value, std::size_t size )
{
    g_eventLogServer->write( type, strlen( type ), timestamp, value, size );
}
    
bool updateEventLogAndWasEvent()
{
    return g_eventLogServer->updateHead();
}
    
    

std::deque< cinder::app::InputEvent > _cinder_events;
    
boost::optional< cinder::app::InputEvent > Input::getEvent()
{
    boost::optional< cinder::app::InputEvent > event;
    if( !_cinder_events.empty() )
    {
        event = _cinder_events.front();
        _cinder_events.pop_front();
    }
    return event;
}
    
    
//buffers
static std::unique_ptr< std::array< b_root, 1 > > g_root_allocation;
b_root* g_root = nullptr;
static std::unique_ptr< std::array< b_root_Line, 2048 > > g_root_Line_allocation;
b_root_Line* g_root_Line = nullptr;
static std::unique_ptr< std::array< b_root_Line_Move, 2048 > > g_root_Line_Move_allocation;
b_root_Line_Move* g_root_Line_Move = nullptr;
static std::unique_ptr< std::array< b_root_Line_Up, 2048 > > g_root_Line_Up_allocation;
b_root_Line_Up* g_root_Line_Up = nullptr;
static std::unique_ptr< std::array< b_root_Line_Down, 2048 > > g_root_Line_Down_allocation;
b_root_Line_Down* g_root_Line_Down = nullptr;
static std::unique_ptr< std::array< b_root_Line_Left, 2048 > > g_root_Line_Left_allocation;
b_root_Line_Left* g_root_Line_Left = nullptr;
static std::unique_ptr< std::array< b_root_Line_Right, 2048 > > g_root_Line_Right_allocation;
b_root_Line_Right* g_root_Line_Right = nullptr;
static std::unique_ptr< std::array< b_root_Line_Sequencer, 2048 > > g_root_Line_Sequencer_allocation;
b_root_Line_Sequencer* g_root_Line_Sequencer = nullptr;

void allocate_buffers()
{
    g_root_allocation = std::make_unique< std::array< b_root, 1 > >();
    g_root = g_root_allocation->data();
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 67, 0 } );
        g_root[ i ].g_root_ring_index = i;
        ::eg::DimensionTraits< struct glm::tvec2<float, glm::highp> >::initialise( g_root[ i ].m_screen);
        g_root[ i ].g_rootLine_ring_iter = 0UL;
    }
    g_root_Line_allocation = std::make_unique< std::array< b_root_Line, 2048 > >();
    g_root_Line = g_root_Line_allocation->data();
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
        g_root_Line[ i ].g_root_Line_cycle = eg::INVALID_TIMESTAMP;
        g_root_Line[ i ].g_root_Line_state = ::eg::action_stopped;
        g_root_Line[ i ].g_root_Line_reference = __eg_root< void >::__eg_Line< void >( eg::reference { i, 74, 0 } );
        g_root_Line[ i ].g_root_Line_ring_index = i;
        ::eg::DimensionTraits< struct glm::tvec2<float, glm::highp> >::initialise( g_root_Line[ i ].m_start);
        ::eg::DimensionTraits< struct glm::tvec2<float, glm::highp> >::initialise( g_root_Line[ i ].m_end);
        ::eg::DimensionTraits< struct glm::tvec3<float, glm::highp> >::initialise( g_root_Line[ i ].m_colour);
        ::eg::DimensionTraits< float >::initialise( g_root_Line[ i ].m_width);
        ::eg::DimensionTraits< float >::initialise( g_root_Line[ i ].m_Distance);
        ::eg::DimensionTraits< float >::initialise( g_root_Line[ i ].m_Speed);
        ::eg::DimensionTraits< class std::vector<int, class std::allocator<int> > >::initialise( g_root_Line[ i ].m_Sequence);
        g_root_Line[ i ].g_root_Line_ring = i;
        g_root_Line[ i ].g_root_LineMove_ring_iter = 0UL;
        g_root_Line[ i ].g_root_LineUp_ring_iter = 0UL;
        g_root_Line[ i ].g_root_LineDown_ring_iter = 0UL;
        g_root_Line[ i ].g_root_LineLeft_ring_iter = 0UL;
        g_root_Line[ i ].g_root_LineRight_ring_iter = 0UL;
        g_root_Line[ i ].g_root_LineSequencer_ring_iter = 0UL;
    }
    g_root_Line_Move_allocation = std::make_unique< std::array< b_root_Line_Move, 2048 > >();
    g_root_Line_Move = g_root_Line_Move_allocation->data();
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
        g_root_Line_Move[ i ].g_root_Line_Move_cycle = eg::INVALID_TIMESTAMP;
        g_root_Line_Move[ i ].g_root_Line_Move_state = ::eg::action_stopped;
        g_root_Line_Move[ i ].g_root_Line_Move_reference = __eg_root< void >::__eg_Line< void >::__eg_Move< void >( eg::reference { i, 83, 0 } );
        g_root_Line_Move[ i ].g_root_Line_Move_ring_index = i;
        ::eg::DimensionTraits< struct glm::tvec2<float, glm::highp> >::initialise( g_root_Line_Move[ i ].m_dir);
        g_root_Line_Move[ i ].g_root_Line_Move_ring = i;
    }
    g_root_Line_Up_allocation = std::make_unique< std::array< b_root_Line_Up, 2048 > >();
    g_root_Line_Up = g_root_Line_Up_allocation->data();
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
        g_root_Line_Up[ i ].g_root_Line_Up_cycle = eg::INVALID_TIMESTAMP;
        g_root_Line_Up[ i ].g_root_Line_Up_state = ::eg::action_stopped;
        g_root_Line_Up[ i ].g_root_Line_Up_reference = __eg_root< void >::__eg_Line< void >::__eg_Up< void >( eg::reference { i, 89, 0 } );
        g_root_Line_Up[ i ].g_root_Line_Up_ring_index = i;
        g_root_Line_Up[ i ].g_root_Line_Up_ring = i;
    }
    g_root_Line_Down_allocation = std::make_unique< std::array< b_root_Line_Down, 2048 > >();
    g_root_Line_Down = g_root_Line_Down_allocation->data();
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
        g_root_Line_Down[ i ].g_root_Line_Down_cycle = eg::INVALID_TIMESTAMP;
        g_root_Line_Down[ i ].g_root_Line_Down_state = ::eg::action_stopped;
        g_root_Line_Down[ i ].g_root_Line_Down_reference = __eg_root< void >::__eg_Line< void >::__eg_Down< void >( eg::reference { i, 95, 0 } );
        g_root_Line_Down[ i ].g_root_Line_Down_ring_index = i;
        g_root_Line_Down[ i ].g_root_Line_Down_ring = i;
    }
    g_root_Line_Left_allocation = std::make_unique< std::array< b_root_Line_Left, 2048 > >();
    g_root_Line_Left = g_root_Line_Left_allocation->data();
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
        g_root_Line_Left[ i ].g_root_Line_Left_cycle = eg::INVALID_TIMESTAMP;
        g_root_Line_Left[ i ].g_root_Line_Left_state = ::eg::action_stopped;
        g_root_Line_Left[ i ].g_root_Line_Left_reference = __eg_root< void >::__eg_Line< void >::__eg_Left< void >( eg::reference { i, 101, 0 } );
        g_root_Line_Left[ i ].g_root_Line_Left_ring_index = i;
        g_root_Line_Left[ i ].g_root_Line_Left_ring = i;
    }
    g_root_Line_Right_allocation = std::make_unique< std::array< b_root_Line_Right, 2048 > >();
    g_root_Line_Right = g_root_Line_Right_allocation->data();
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
        g_root_Line_Right[ i ].g_root_Line_Right_cycle = eg::INVALID_TIMESTAMP;
        g_root_Line_Right[ i ].g_root_Line_Right_state = ::eg::action_stopped;
        g_root_Line_Right[ i ].g_root_Line_Right_reference = __eg_root< void >::__eg_Line< void >::__eg_Right< void >( eg::reference { i, 107, 0 } );
        g_root_Line_Right[ i ].g_root_Line_Right_ring_index = i;
        g_root_Line_Right[ i ].g_root_Line_Right_ring = i;
    }
    g_root_Line_Sequencer_allocation = std::make_unique< std::array< b_root_Line_Sequencer, 2048 > >();
    g_root_Line_Sequencer = g_root_Line_Sequencer_allocation->data();
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
        g_root_Line_Sequencer[ i ].g_root_Line_Sequencer_cycle = eg::INVALID_TIMESTAMP;
        g_root_Line_Sequencer[ i ].g_root_Line_Sequencer_state = ::eg::action_stopped;
        g_root_Line_Sequencer[ i ].g_root_Line_Sequencer_reference = __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >( eg::reference { i, 113, 0 } );
        g_root_Line_Sequencer[ i ].g_root_Line_Sequencer_ring_index = i;
        g_root_Line_Sequencer[ i ].g_root_Line_Sequencer_ring = i;
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 2048; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
}

//Action functions
extern __eg_root< void > root_starter( std::vector< std::function< void() > >& );
extern void root_stopper( eg::Instance _gid );


    
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
    void mouseMove( MouseEvent event ) override;
    void mouseWheel( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    
    void keyDown( KeyEvent event ) override;
    void keyUp( KeyEvent event ) override;

    void setup();

	// Cinder will call 'draw' each time the contents of the window need to be redrawn.
	void draw() override;
    
    static std::vector< std::string > g_strScripts;
    static std::string g_strDatabase;

private:
    HostClock::TickDuration sleepDuration = std::chrono::milliseconds( 10 );
    std::vector< std::function< void() > > extraRootFunctions;
    
};

BasicApp::BasicApp()
{
    allocate_buffers();
}

BasicApp::~BasicApp()
{
    root_stopper( 0U );
    
    while( boost::this_fiber::properties< eg::fiber_props >().shouldContinue() )
    {
        theClock.nextCycle();
        eg::wait();
    }
    
    deallocate_buffers();
}

void BasicApp::setup()
{
    boost::fibers::use_scheduling_algorithm< eg::eg_algorithm >();
    
    ImGui::initialize();
        
    extraRootFunctions = loadScripts( g_strScripts, g_strDatabase );
        
    root_starter( extraRootFunctions );
    
    boost::this_fiber::properties< eg::fiber_props >().setTimeKeeper();
}

void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void BasicApp::mouseDown( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseDown, event ) );
}
void BasicApp::mouseUp( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseUp, event ) );
}
void BasicApp::mouseMove( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseMove, event ) );
}
void BasicApp::mouseWheel( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseWheel, event ) );
}
void BasicApp::mouseDrag( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseDrag, event ) );
}

void BasicApp::keyDown( KeyEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eKeyDown, event ) );
}
void BasicApp::keyUp( KeyEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eKeyUp, event ) );
}

void BasicApp::update()
{
    if( !boost::this_fiber::properties< eg::fiber_props >().shouldContinue() )
    {
        quit();
    }
}
    
void BasicApp::draw()
{
    theClock.nextCycle();
    eg::wait();
    _cinder_events.clear();
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )

std::vector< std::string > BasicApp::g_strScripts;
std::string BasicApp::g_strDatabase;

int main( int argc, const char* argv[] )
{    
    {
        bool bDebug = false;
        namespace po = boost::program_options;
        po::variables_map vm;
        try
        {
            po::options_description desc("Allowed options");
            desc.add_options()
                ("help", "produce help message")
                
                //options
                ("debug",       po::value< bool >( &bDebug )->implicit_value( true ), 
                    "Wait at startup to allow attaching a debugger" )
                ("script",      po::value< std::vector< std::string > >( &BasicApp::g_strScripts ), "Script file to run" )
                ("database",    po::value< std::string >( &BasicApp::g_strDatabase ), "Program Database" )
            ;

            po::positional_options_description p;
            p.add( "script", -1 );

            po::store( po::command_line_parser( argc, argv).
                        options( desc ).
                        positional( p ).run(),
                        vm );
            po::notify(vm);

            if (vm.count("help"))
            {
                std::cout << desc << "\n";
                return 1;
            }
        }
        catch( std::exception& )
        {
            std::cout << "Invalid input. Type '--help' for options\n";
            return 1;
        }
        //wait for input 
        if( bDebug )
        {
#ifdef _DEBUG
            Common::debug_break();
#else
            char c;
            std::cin >> c;
#endif 
        }
    }
    try
    {
        return WinMain( nullptr, nullptr, nullptr, 0 );
    }
    catch( std::exception& e )
    {
        ERR( e.what() );
        return 0;
    }
}

    