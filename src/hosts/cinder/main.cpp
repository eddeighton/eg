

#include "eg/implementation_session.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <iostream>
    
int main( int argc, const char* argv[] )
{
    bool bDebug = false;
    std::string strProgram;
    std::string strLogFileDefault = "log";
    std::string strName = "Demo";
    boost::filesystem::path strBuildDir = boost::filesystem::current_path();
    {
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
                ("program",     po::value< std::string >( &strProgram ),                        "EG program database" )
                ("log",         po::value< std::string >( &strLogFileDefault ),                 "Default log file path to use" )
                ("dir",         po::value< boost::filesystem::path >( &strBuildDir ),           "Build directory" )
                ("name",        po::value< std::string >( &strName ),                           "Program Name" )
            ;

            po::positional_options_description p;
            //p.add( "src", -1 );

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
    }

    if( bDebug )
    {
        char c;
        std::cin >> c;
    }
    
    const boost::filesystem::path programDataBaseFile =
        boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( strProgram ) );

    if( !boost::filesystem::exists( programDataBaseFile ) )
    {
        std::cout << "Error cannot locate: " << programDataBaseFile.string() << std::endl;
        return 0;
    }
    
    eg::ReadSession session( programDataBaseFile );
    
    std::unique_ptr< boost::filesystem::ofstream > pIncludesFileStream =
            boost::filesystem::createNewFileStream( strBuildDir / std::string( "main.cpp" ) );
    
    std::ostream& os = *pIncludesFileStream;
    
    const eg::abstract::Root* pRoot = session.getTreeRoot();
    
    const eg::DerivationAnalysis& derivationAnalysis = session.getDerivationAnalysis();
    const eg::Layout& layout = session.getLayout();
    const eg::IndexedObject::Array& objects = session.getObjects( eg::IndexedObject::MASTER_FILE );
    
    
    os << "\n//buffers\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        os << "\n//Buffer: " << pBuffer->getTypeName() << " stride: " << pBuffer->getStride() << " size: " << pBuffer->getSize() << "\n";
        os << "struct " << pBuffer->getTypeName() << "\n{\n";
        for( const eg::DataMember* pDimension : pBuffer->getDimensions() )
        {
            os << "    ";
            pDimension->print( os );
            os << " " << pDimension->getName() << ";\n";
        }
        
        os << "};\n";
        os << "static std::array< " << pBuffer->getTypeName() << ", " << pBuffer->getSize() << " > " << pBuffer->getVariableName() << ";\n";
        //os << "static " << pBuffer->getTypeName() << " *" << pBuffer->getVariableName() << ";\n";
    }
    
    os << "\n";
    
    os << "void allocate_buffers()\n";
    os << "{\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
    os << "    for( EGInstance i = 0U; i != " << pBuffer->getSize() << "; ++i )\n";
    os << "    {\n";
        for( const eg::DataMember* pDimension : pBuffer->getDimensions() )
        {
            pDimension->printAllocation( os, "i" );
        }
    
    os << "    }\n";
    }
    os << "}\n";
    
    os << "\n";
    
    os << "void deallocate_buffers()\n";
    os << "{\n";
    //deallocate in reverse
    for( std::size_t sz = layout.getBuffers().size(); sz > 0U; --sz )
    {
        const eg::Buffer* pBuffer = layout.getBuffers()[ sz - 1U ];
    os << "    for( EGInstance i = 0U; i != " << pBuffer->getSize() << "; ++i )\n";
    os << "    {\n";
        for( const eg::DataMember* pDimension : pBuffer->getDimensions() )
        {
            pDimension->printDeallocation( os, "i" );
        }
    os << "    }\n";
    }
    os << "}\n";
    
    os << "\n";
    
    
    
    
    const char* pszEventLogImpl = R"(
    
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
    )";
    os << pszEventLogImpl << "\n";
    
    
    os << "//clock implementation\n";
    const char* pszClockImpl = R"(
    
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
    )";
    os << pszClockImpl;
    
    os << "\n";
    
    
    os << "//clock implementation\n";
    const char* pszInputEventsImpl = R"(
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

    
    )";
    os << pszInputEventsImpl;
    
    
    os << "//initialiser\n";
    os << "extern void initialise( EGDependencyProvider* pDependencyProvider );\n";
    
    os << "\n";
    
    os << "//Action functions\n";
    std::vector< const eg::concrete::Action* > actions = 
        eg::many_cst< eg::concrete::Action >( objects );
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "extern "; pAction->printType( os ); os << " " << pAction->getName() << "_starter( EGInstance _gid );\n";
    os << "extern void " << pAction->getName() << "_stopper( EGInstance _gid );\n";
    os << "extern bool " << pAction->getName() << "_executor();\n";
        }
    }
    
    os << "\n";
    
    os << "//Dependency Provider Implementation\n";
    os << "struct CinderHost_EGDependencyProvider : public EGDependencyProvider\n";
    os << "{\n";
    os << "     __eg_clock* m_pClock;\n";
    os << "     __eg_event_log* m_pEventLog;\n";
    os << "     __eg_input* m_pInput;\n";
    os << "\n";
    os << "     CinderHost_EGDependencyProvider( __eg_clock* pClock, __eg_event_log* pEventLog, __eg_input* pInput )\n";
    os << "         :   m_pClock( pClock ),\n";
    os << "             m_pEventLog( pEventLog ),\n";
    os << "             m_pInput( pInput )\n";
    os << "     {\n";
    os << "\n";         
    os << "     }\n"; 
    os << "\n";
    os << "    virtual void* getBuffer( const char* pszName )\n";
    os << "    {\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
    os << "        if( 0U == strcmp( pszName, \"" << pBuffer->getVariableName() << "\" ) ) return " << pBuffer->getVariableName() << ".data();\n";
    }
    os << "        return nullptr;\n";
    os << "    }\n";
    os << "    virtual void* getInterface( const char* pszName )\n";
    os << "    {\n";
    os << "        if( 0U == strcmp( pszName, \"__eg_clock\" ) ) return m_pClock;\n";
    os << "        if( 0U == strcmp( pszName, \"__eg_event_log\" ) ) return m_pEventLog;\n";
    os << "        if( 0U == strcmp( pszName, \"__eg_input\" ) ) return m_pInput;\n";
    os << "        return nullptr;\n";
    os << "    }\n";
    os << "};\n";
    
    os << "\n\n";
    
    os << "bool executeSchedule()\n";
    os << "{\n";
    os << "    bool bWaited = false;\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
    os << "    bWaited = " << pAction->getName() << "_executor() || bWaited;\n";
    }
    os << "    return bWaited;\n";
    os << "}\n";
    
    
    //const std::string strProgramNameIsh = 
    //    boost::filesystem::path( strProgram ).filename().stem().generic_string();
    
    os << "\n\nstd::string strEventLogFolder = \"" << strLogFileDefault << "\";\n\n";
    
    const char* pszMain = R"(
    
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

    )";
    
    os << pszMain;
    
    return 0;
}