//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.



#include "eg/implementation_session.hpp"
#include "eg/codegen.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <iostream>
    
extern void generate_python( std::ostream& os, eg::ReadSession& session );

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
    
    const eg::interface::Root* pRoot = session.getTreeRoot();
    
    const eg::DerivationAnalysis& derivationAnalysis = session.getDerivationAnalysis();
    const eg::Layout& layout = session.getLayout();
    const eg::IndexedObject::Array& objects = session.getObjects( eg::IndexedObject::MASTER_FILE );
    
    os << "#include \"structures.hpp\"\n";
    os << "#include \"host_clock.hpp\"\n";
    os << "#include \"host_event_log.hpp\"\n";
    os << "#include \"pybind11/embed.h\"\n";
    os << "#include \"pybind11/numpy.h\"\n";
    os << "#include \"pybind11/stl.h\"\n";
    os << "#include \"pybind11/stl_bind.h\"\n";
    os << "#include \"py_eg_reference.hpp\"\n";
    os << "#include \"eg_runtime/eg_runtime.hpp\"\n";
    os << "#include <boost/program_options.hpp>\n";
    os << "#include <boost/filesystem.hpp>\n";
    
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
    os << "    for( " << eg::EG_INSTANCE << " i = 0U; i != " << pBuffer->getSize() << "; ++i )\n";
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
    os << "    for( " << eg::EG_INSTANCE << " i = 0U; i != " << pBuffer->getSize() << "; ++i )\n";
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
    os << "extern void initialise( " << eg::EG_DEPENDENCY_PROVIDER_TYPE << "* pDependencyProvider );\n";
    
    os << "\n";
    
    os << "//Action functions\n";
    std::vector< const eg::concrete::Action* > actions = 
        eg::many_cst< eg::concrete::Action >( objects );
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "extern "; pAction->printType( os ); os << " " << pAction->getName() << "_starter( " << eg::EG_INSTANCE << " _gid );\n";
    os << "extern void " << pAction->getName() << "_stopper( " << eg::EG_INSTANCE << " _gid );\n";
    
        }
    }
    
    os << "\n\n";
    
    //////Generate the python bindings
    os << "\n";
    generate_python( os, session );
    os << "\n";
    
    os << "//Dependency Provider Implementation\n";
    os << "struct CinderHost_EGDependencyProvider : public " << eg::EG_DEPENDENCY_PROVIDER_TYPE << "\n";
    os << "{\n";
    os << "     eg::_clock* m_pClock;\n";
    os << "     eg::_event_log* m_pEventLog;\n";
    os << "     __eg_input* m_pInput;\n";
    os << "\n";
    os << "     CinderHost_EGDependencyProvider( eg::_clock* pClock, eg::_event_log* pEventLog, __eg_input* pInput )\n";
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
    os << "        if( 0U == strcmp( pszName, \"_clock\" ) ) return m_pClock;\n";
    os << "        if( 0U == strcmp( pszName, \"_event_log\" ) ) return m_pEventLog;\n";
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

void runPython( const std::string& strDatabaseFile, const std::string& strScript )
{
    pybind11::scoped_interpreter guard{}; // start the interpreter and keep it alive

    try
    {
        pybind11::module pyeg_module = pybind11::module::import( "pyeg" );

        HostFunctions hostFunctions( strDatabaseFile, pyeg_module );

        g_pEGRefType = std::make_shared< eg::PythonEGReferenceType >( hostFunctions );

        pybind11::exec( strScript );
    }
    catch( std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }
    
}

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
    
    static std::string g_strPythonScript;
    static std::string g_strDatabase;

    void RunCycle();
private:
    HostClock::TickDuration sleepDuration = std::chrono::milliseconds( 10 );
    HostClock theClock;
    HostEventLog theEventLog;
    InputEvents inputEvents;
    CinderHost_EGDependencyProvider dependencies;
    std::unique_ptr< std::thread > pPythonThread;
};


BasicApp::BasicApp()
    :   theEventLog( "basicapp", strEventLogFolder.c_str() ),
        dependencies( &theClock, &theEventLog, &inputEvents )
{
    allocate_buffers();
    
    initialise( &dependencies );
    
    root_starter( 0 );
}

BasicApp::~BasicApp()
{
    if( pPythonThread ) pPythonThread->join();
    deallocate_buffers();
}

void BasicApp::setup()
{
    //load optional python script
    if( !g_strPythonScript.empty() && !g_strDatabase.empty() )
    {
        std::string strScript;
        const boost::filesystem::path pythonFilePath = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( g_strPythonScript ) );
        if( !boost::filesystem::exists( pythonFilePath ) )
        {
            THROW_RTE( "Cannot locate file: " << pythonFilePath.string() );
        } 
        const boost::filesystem::path databaseFilePath = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( g_strDatabase ) );
        if( !boost::filesystem::exists( databaseFilePath ) )
        {
            THROW_RTE( "Cannot locate file: " << databaseFilePath.string() );
        }
        
        boost::filesystem::loadAsciiFile( pythonFilePath, strScript );
        pPythonThread = std::make_unique< std::thread >( 
            std::bind( &runPython, g_strDatabase, strScript ) );
    }
}

void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void BasicApp::update()
{
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

void BasicApp::RunCycle()
{
}
    
void BasicApp::draw()
{
}


// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )

std::string BasicApp::g_strPythonScript;
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
                ("python",      po::value< std::string >( &BasicApp::g_strPythonScript ), "Python file to run" )
                ("database",    po::value< std::string >( &BasicApp::g_strDatabase ), "Program Database" )
            ;

            po::positional_options_description p;
            p.add( "python", -1 );

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

    )";
    
    os << pszMain;
    
    return 0;
}