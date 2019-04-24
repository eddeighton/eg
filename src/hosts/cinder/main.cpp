

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
    
    os << "#include \"structures.hpp\"\n";
    os << "#include \"host_clock.hpp\"\n";
    os << "#include \"host_event_log.hpp\"\n";
    
    os << "\n//buffers\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
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
    
    os << "//clock implementation\n";
    const char* pszInputEventsImpl = R"(

class InputEvents : public __eg_input
{
public:
    virtual std::optional< cinder::app::InputEvent > getEvent()
    {
        std::optional< cinder::app::InputEvent > event;
        if( !events.empty() )
        {
            event = events.front();
            events.pop_front();
        }
        return event;
    }
    
    std::deque< cinder::app::InputEvent > events;
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
    //os << "extern bool " << pAction->getName() << "_executor();\n";
    
    ////executor
    os << "bool " << pAction->getName() << "_executor()\n";
    os << "{\n";
    
    const eg::DataMember* pRunningTimestamp = layout.getDataMember( pAction->getRunningTimestamp() );
    const eg::DataMember* pPauseTimestamp   = layout.getDataMember( pAction->getPauseTimestamp()   );
    const eg::DataMember* pCoroutine        = layout.getDataMember( pAction->getCoroutine()        );
    
    os << "    const EGTimeStamp subcycle = clock::subcycle();\n";
    os << "    for( EGInstance i = 0; i != " << pAction->getTotalDomainSize() << "; ++i )\n";
    os << "    {\n";
    os << "        if( " << eg::Printer( pRunningTimestamp, "i" ) << " <= subcycle )\n";
    os << "        {\n";
    os << "             if( " << eg::Printer( pPauseTimestamp, "i" ) << " <= subcycle )\n";
    os << "             {\n";
    os << "                 if( " << eg::Printer( pCoroutine, "i" ) << ".done() )\n";
    os << "                 {\n";
    os << "                     " << pAction->getName() << "_stopper( i );\n";
    os << "                 }\n";
    os << "                 else\n";
    os << "                 {\n";
    os << "                     " << eg::Printer( pCoroutine, "i" ) << ".resume();\n";
    os << "                 }\n";
    os << "             }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    return false;\n";
    os << "}\n";
    os << "\n";
        }
    }
    
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
    void mouseMove( MouseEvent event ) override;
    void mouseWheel( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    
    void keyDown( KeyEvent event ) override;
    void keyUp( KeyEvent event ) override;

	// Cinder will call 'draw' each time the contents of the window need to be redrawn.
	void draw() override;

private:
    HostClock::TickDuration sleepDuration = std::chrono::milliseconds( 10 );
    IPC::PID thePID;
    HostClock theClock;
    HostEventLog theEventLog;
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
    cinder::app::InputEvent e( cinder::app::InputEvent::eMouseDown, event );
    inputEvents.events.push_back( e );
}
void BasicApp::mouseUp( MouseEvent event )
{
    cinder::app::InputEvent e( cinder::app::InputEvent::eMouseUp, event );
    inputEvents.events.push_back( e );
}
void BasicApp::mouseMove( MouseEvent event )
{
    cinder::app::InputEvent e( cinder::app::InputEvent::eMouseMove, event );
    inputEvents.events.push_back( e );
}
void BasicApp::mouseWheel( MouseEvent event )
{
    cinder::app::InputEvent e( cinder::app::InputEvent::eMouseWheel, event );
    inputEvents.events.push_back( e );
}
void BasicApp::mouseDrag( MouseEvent event )
{
    cinder::app::InputEvent e( cinder::app::InputEvent::eMouseDrag, event );
    inputEvents.events.push_back( e );
}

void BasicApp::keyDown( KeyEvent event )
{
    cinder::app::InputEvent e( cinder::app::InputEvent::eKeyDown, event );
    inputEvents.events.push_back( e );
}
void BasicApp::keyUp( KeyEvent event )
{
    cinder::app::InputEvent e( cinder::app::InputEvent::eKeyUp, event );
    inputEvents.events.push_back( e );
}

void BasicApp::draw()
{
    const HostClock::Tick cycleStart = theClock.nextCycle();
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