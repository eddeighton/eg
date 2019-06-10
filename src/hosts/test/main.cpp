
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
    
    os << "\n//buffers\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        os << "static std::array< " << pBuffer->getTypeName() << ", " << pBuffer->getSize() << " > " << pBuffer->getVariableName() << ";\n";
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
    
        
    generate_dynamic_interface( os, session );
    
    os << "//Action functions\n";
    os << "extern __eg_root< void > root_starter( std::vector< std::function< void() > >& );\n";
        
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
        
    os << "\n\nstd::string strEventLogFolder = \"" << strLogFileDefault << "\";\n\n";
    
    const char* pszMainRoutine = R"(
    
int main( int argc, const char* argv[] )
{
    try
    {
        if( argc > 1 )
        {
            std::cout << "waiting for character input...\n";
            char c;
            std::cin >> c;
        }
        
        HostClock::TickDuration sleepDuration = std::chrono::milliseconds( 1000 / 60 );
                
        //allocate everything
        allocate_buffers();
        
        //setup the dependencies
        HostClock theClock;
        HostEventLog theEventLog( "test", strEventLogFolder.c_str() );
        
        BasicHost_EGDependencyProvider dependencies( &theClock, &theEventLog );
        initialise( &dependencies );
        
        //be sure to initialise the clock before the scheduler
        boost::fibers::use_scheduling_algorithm< eg::eg_algorithm >();
        
        //start the root
        std::vector< std::function< void() > > pythonFunctions =  {};
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
            eg::wait();
        }
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