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
    return false;//g_eventLogServer->read( iterator, event.type, event.timestamp, event.value, event.size );
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
    
std::optional< cinder::app::InputEvent > Input::getEvent()
{
    std::optional< cinder::app::InputEvent > event;
    if( !_cinder_events.empty() )
    {
        event = _cinder_events.front();
        _cinder_events.pop_front();
    }
    return event;
}
    
    
//buffers
static std::array< b_root, 1 > g_root_array;
static std::array< b_root_StrokedCircle, 256 > g_root_StrokedCircle_array;
static std::array< b_root_StrokedCircle_Default, 256 > g_root_StrokedCircle_Default_array;
static std::array< b_root_StrokedCircle_ColorChanger, 256 > g_root_StrokedCircle_ColorChanger_array;
static std::array< b_root_StrokedCircle_Morph, 256 > g_root_StrokedCircle_Morph_array;
static std::array< b_root_Circle, 256 > g_root_Circle_array;
static std::array< b_root_Circle_Default, 256 > g_root_Circle_Default_array;
static std::array< b_root_Circle_ColorChanger, 256 > g_root_Circle_ColorChanger_array;
static std::array< b_root_Circle_Morph, 256 > g_root_Circle_Morph_array;
static std::array< b_root_Spiral, 1 > g_root_Spiral_array;
static std::array< b_root_Attract, 1 > g_root_Attract_array;
static std::array< b_root_Randomise, 1 > g_root_Randomise_array;
static std::array< b_root_imgui, 1 > g_root_imgui_array;
b_root* g_root = g_root_array.data();
b_root_StrokedCircle* g_root_StrokedCircle = g_root_StrokedCircle_array.data();
b_root_StrokedCircle_Default* g_root_StrokedCircle_Default = g_root_StrokedCircle_Default_array.data();
b_root_StrokedCircle_ColorChanger* g_root_StrokedCircle_ColorChanger = g_root_StrokedCircle_ColorChanger_array.data();
b_root_StrokedCircle_Morph* g_root_StrokedCircle_Morph = g_root_StrokedCircle_Morph_array.data();
b_root_Circle* g_root_Circle = g_root_Circle_array.data();
b_root_Circle_Default* g_root_Circle_Default = g_root_Circle_Default_array.data();
b_root_Circle_ColorChanger* g_root_Circle_ColorChanger = g_root_Circle_ColorChanger_array.data();
b_root_Circle_Morph* g_root_Circle_Morph = g_root_Circle_Morph_array.data();
b_root_Spiral* g_root_Spiral = g_root_Spiral_array.data();
b_root_Attract* g_root_Attract = g_root_Attract_array.data();
b_root_Randomise* g_root_Randomise = g_root_Randomise_array.data();
b_root_imgui* g_root_imgui = g_root_imgui_array.data();

void allocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 92, 0 } );
        g_root[ i ].g_rootStrokedCircle_alloc_iter = 0UL;
        g_root[ i ].g_rootCircle_alloc_iter = 0UL;
        g_root[ i ].g_rootSpiral_alloc_iter = 0UL;
        g_root[ i ].g_rootAttract_alloc_iter = 0UL;
        g_root[ i ].g_rootRandomise_alloc_iter = 0UL;
        g_root[ i ].g_rootimgui_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_state = ::eg::action_stopped;
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_reference = __eg_root< void >::__eg_StrokedCircle< void >( eg::reference { i, 98, 0 } );
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_alloc_data = i;
        g_root_StrokedCircle[ i ].g_root_StrokedCircleDefault_alloc_iter = 0UL;
        g_root_StrokedCircle[ i ].g_root_StrokedCircleColorChanger_alloc_iter = 0UL;
        g_root_StrokedCircle[ i ].g_root_StrokedCircleMorph_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_state = ::eg::action_stopped;
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_reference = __eg_root< void >::__eg_Shape< void >::__eg_Default< void >( eg::reference { i, 132, 0 } );
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_state = ::eg::action_stopped;
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_reference = __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >( eg::reference { i, 137, 0 } );
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_state = ::eg::action_stopped;
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_reference = __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >( eg::reference { i, 142, 0 } );
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle[ i ].g_root_Circle_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_root_Circle_state = ::eg::action_stopped;
        g_root_Circle[ i ].g_root_Circle_reference = __eg_root< void >::__eg_Circle< void >( eg::reference { i, 103, 0 } );
        g_root_Circle[ i ].g_root_Circle_alloc_data = i;
        g_root_Circle[ i ].g_root_CircleDefault_alloc_iter = 0UL;
        g_root_Circle[ i ].g_root_CircleColorChanger_alloc_iter = 0UL;
        g_root_Circle[ i ].g_root_CircleMorph_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle_Default[ i ].g_root_Circle_Default_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_Default[ i ].g_root_Circle_Default_state = ::eg::action_stopped;
        g_root_Circle_Default[ i ].g_root_Circle_Default_reference = __eg_root< void >::__eg_Shape< void >::__eg_Default< void >( eg::reference { i, 162, 0 } );
        g_root_Circle_Default[ i ].g_root_Circle_Default_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_state = ::eg::action_stopped;
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_reference = __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >( eg::reference { i, 167, 0 } );
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_state = ::eg::action_stopped;
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_reference = __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >( eg::reference { i, 172, 0 } );
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Spiral[ i ].g_root_Spiral_cycle = eg::INVALID_TIMESTAMP;
        g_root_Spiral[ i ].g_root_Spiral_state = ::eg::action_stopped;
        g_root_Spiral[ i ].g_root_Spiral_reference = __eg_root< void >::__eg_Spiral< void >( eg::reference { i, 108, 0 } );
        g_root_Spiral[ i ].g_root_Spiral_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Attract[ i ].g_root_Attract_cycle = eg::INVALID_TIMESTAMP;
        g_root_Attract[ i ].g_root_Attract_state = ::eg::action_stopped;
        g_root_Attract[ i ].g_root_Attract_reference = __eg_root< void >::__eg_Attract< void >( eg::reference { i, 113, 0 } );
        g_root_Attract[ i ].g_root_Attract_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Randomise[ i ].g_root_Randomise_cycle = eg::INVALID_TIMESTAMP;
        g_root_Randomise[ i ].g_root_Randomise_state = ::eg::action_stopped;
        g_root_Randomise[ i ].g_root_Randomise_reference = __eg_root< void >::__eg_Randomise< void >( eg::reference { i, 118, 0 } );
        g_root_Randomise[ i ].g_root_Randomise_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_imgui[ i ].g_root_imgui_cycle = eg::INVALID_TIMESTAMP;
        g_root_imgui[ i ].g_root_imgui_state = ::eg::action_stopped;
        g_root_imgui[ i ].g_root_imgui_reference = __eg_root< void >::__eg_imgui< void >( eg::reference { i, 123, 0 } );
        g_root_imgui[ i ].g_root_imgui_alloc_data = i;
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
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
    
    return WinMain( nullptr, nullptr, nullptr, 0 );
}

    