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
#include "eg/interface.hpp"
#include "eg/codegen.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <iostream>

extern void generate_dynamic_interface( std::ostream& os, eg::ReadSession& session );
extern void generate_python( std::ostream& os, eg::ReadSession& session );

int main( int argc, const char* argv[] )
{
    bool bDebug = false;
    std::string strProgram;
    std::string strLogFileDefault = "log";
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
                //("src",         po::value< std::vector< std::string > >( &inputFiles ),  "input files to compile" )
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
    os << "#include \"pybind11/embed.h\"\n";
    os << "#include \"pybind11/numpy.h\"\n";
    os << "#include \"pybind11/stl.h\"\n";
    os << "#include \"pybind11/stl_bind.h\"\n";
    os << "#include \"hosts/py_eg_reference.hpp\"\n";
    os << "#include \"eg_runtime/eg_runtime.hpp\"\n";
    os << "#include \"eventlog/eventlog_api.hpp\"\n";
    os << "#include <boost/program_options.hpp>\n";
    os << "#include <boost/filesystem.hpp>\n";
    
    const char* pszClock = R"(

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

)";

os << pszClock;

const char* pszEventLog = R"(

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
    
    )";
    
    os << pszEventLog;
    
    os << "\n//buffers\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        os << "static std::array< " << pBuffer->getTypeName() << ", " << pBuffer->getSize() << " > " << pBuffer->getVariableName() << "_array;\n";
    }
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        os << pBuffer->getTypeName() << "* " << pBuffer->getVariableName() << " = " << pBuffer->getVariableName() << "_array.data();\n";
    }
    
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
    
    os << "//Action functions\n";
    os << "extern __eg_root< void > root_starter( std::vector< std::function< void() > >& );\n";
    std::vector< const eg::concrete::Action* > actions = 
        eg::many_cst< eg::concrete::Action >( objects );
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() && pAction->getParent()->getParent() )
        {
    os << "extern "; pAction->printType( os ); os << " " << pAction->getName() << "_starter( " << eg::EG_INSTANCE << " _gid );\n";
        }
        if( pAction->getParent() )
        {
    os << "extern void " << pAction->getName() << "_stopper( " << eg::EG_INSTANCE << " _gid );\n";
        }
    }
    
    os << "\n";
    generate_dynamic_interface( os, session );
    generate_python( os, session );
    os << "\n";
    
    const char* pszMainRoutine = R"(
    
int main( int argc, const char* argv[] )
{
    std::string strDatabaseFile;
    std::vector< std::string > scripts;
    int iMilliseconds = 16;
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
                ("database",    po::value< std::string >( &strDatabaseFile ), "Program Database" )
                ("python",      po::value< std::vector< std::string > >( &scripts ), "Python scripts" )
                ("rate",        po::value< int >( &iMilliseconds ), "Simulation rate in milliseconds" )
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
    
    
    try
    {
        const HostClock::TickDuration sleepDuration = 
            std::chrono::milliseconds( iMilliseconds );
        
        //allocate everything
        allocate_buffers();
        
        //be sure to initialise the clock before the scheduler
        boost::fibers::use_scheduling_algorithm< eg::eg_algorithm >();
    
        pybind11::scoped_interpreter guard{}; // start the python interpreter
        
        //start the root
        std::vector< std::function< void() > > pythonFunctions = 
            loadPythonScripts( scripts, strDatabaseFile );
        
        root_starter( pythonFunctions );
        
        boost::this_fiber::properties< eg::fiber_props >().setTimeKeeper();
        
        HostClock::Tick cycleStart = theClock.actual();
        while( boost::this_fiber::properties< eg::fiber_props >().shouldContinue() )
        {
            const HostClock::TickDuration elapsed = theClock.actual() - cycleStart;
            if( elapsed < sleepDuration )
            {
                boost::this_fiber::sleep_for( sleepDuration - elapsed );
            }
            theClock.nextCycle();
            updateEventLogAndWasEvent();
            eg::wait();
        }
        
        deallocate_buffers();
    }
    catch( std::exception& e )
    {
        std::cerr << "Error: " << e.what() << std::endl;
        deallocate_buffers();
    }
    
    return 0;
    
}
    
    )";
    os << pszMainRoutine << "\n";
    

    return 0;
}