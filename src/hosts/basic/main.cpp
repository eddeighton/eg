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
    
    
    os << "\n";
    generate_python( os, session );
    os << "\n";
    
    os << "//Dependency Provider Implementation\n";
    os << "struct BasicHost_EGDependencyProvider : public " << eg::EG_DEPENDENCY_PROVIDER_TYPE << "\n";
    os << "{\n";
    os << "     eg::_clock* m_pClock;\n";
    os << "     eg::_event_log* m_pEventLog;\n";
    os << "\n";
    os << "     BasicHost_EGDependencyProvider( eg::_clock* pClock, eg::_event_log* pEventLog )\n";
    os << "         :   m_pClock( pClock ),\n";
    os << "             m_pEventLog( pEventLog )\n";
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
    os << "        return nullptr;\n";
    os << "    }\n";
    os << "};\n";
    
    //const std::string strProgramNameIsh = 
    //    boost::filesystem::path( strProgram ).filename().stem().generic_string();
    
    os << "\n\nstd::string strEventLogFolder = \"" << strLogFileDefault << "\";\n\n";
    
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
        
        //setup the dependencies
        HostClock theClock;
        HostEventLog theEventLog( "basic", strEventLogFolder.c_str() );
        
        BasicHost_EGDependencyProvider dependencies( &theClock, &theEventLog );
        initialise( &dependencies );
        
        //be sure to initialise the clock before the scheduler
        boost::fibers::use_scheduling_algorithm< eg::eg_algorithm >();
    
        //start the root
        root_starter( 0 );
        
        if( !scripts.empty() )
        {
            if( strDatabaseFile.empty() )
            {
                std::cout << "Missing database file path" << std::endl;
                return 0;
            }
            const boost::filesystem::path databaseFilePath = 
                boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( strDatabaseFile ) );
            if( !boost::filesystem::exists( databaseFilePath ) )
            {
                std::cout << "Cannot locate file: " << databaseFilePath.string() << std::endl;
                return 0;
            } 
                
            for( const std::string& strPythonScript : scripts )
            {
                std::string strScript;
                const boost::filesystem::path pythonFilePath = 
                    boost::filesystem::edsCannonicalise(
                        boost::filesystem::absolute( strPythonScript ) );
                if( !boost::filesystem::exists( pythonFilePath ) )
                {
                    std::cout << "Cannot locate file: " << pythonFilePath.string() << std::endl;
                    return 0;
                } 
                boost::filesystem::loadAsciiFile( pythonFilePath, strScript );
                
                //start python fiber
            }
        }
        
        boost::fibers::fiber timeKeeperFiber
        (
            [ sleepDuration, &theClock ]()
            {
                boost::this_fiber::properties< eg::fiber_props >().setTimeKeeper();
                boost::this_fiber::yield();
                
                HostClock::Tick cycleStart = theClock.actual();
                while( g_root[ 0 ].g_root_state != eg::action_stopped )
                {
                    const HostClock::TickDuration elapsed = theClock.actual() - cycleStart;
                    if( elapsed < sleepDuration )
                    {
                        eg::sleep( sleepDuration - elapsed );
                    }
                    theClock.nextCycle();
                    cycleStart = theClock.actual();
                    std::cout << "tick: " << theClock.cycle() << " : " << theClock.ct() << "s" << std::endl;
                    eg::wait();
                }
            }
        );
        
        timeKeeperFiber.join();
        
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