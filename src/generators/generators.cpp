
#include "generators.hpp"

#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <iostream>

namespace eg
{
    CmdLine handleCommandLine( int argc, const char* argv[] )
    {
        bool bDebug = false;
        std::string strProgram;
        CmdLine cmdLine;
        std::vector< std::string > packages;
        {
            namespace po = boost::program_options;
            po::variables_map vm;
            try
            {
                po::options_description desc("Allowed options");
                desc.add_options()
                    ("help", "produce help message")
                    
                    //options
                    ("debug",       po::bool_switch( &bDebug ), 
                        "Wait at startup to allow attaching a debugger" )
                    ("database",    po::value< std::string >( &strProgram ),                        "EG program database" )
                    ("dir",         po::value< boost::filesystem::path >( &cmdLine.strBuildDir ),   "Build directory" )
                    ("target",      po::value< std::string >( &cmdLine.targetFileName ),            "Target File Name" )
                    ("name",        po::value< std::string >( &cmdLine.strName ),                   "Program Name" )
                    ("package",     po::value< std::vector< std::string > >( &packages ),           "Packages" )
					
					
                ;

                po::positional_options_description p;
                p.add( "package", -1 );

                po::store( po::command_line_parser( argc, argv).
                            options( desc ).
                            positional( p ).run(),
                            vm );
                po::notify(vm);

                if (vm.count("help"))
                {
                    std::cout << desc << "\n";
                    std::abort();
                }
            }
            catch( std::exception& )
            {
                std::cout << "Invalid input. Type '--help' for options\n";
                std::abort();
            }
        }
        
        for( const std::string& strPackage : packages )
        {
            cmdLine.packages.insert( strPackage );
        }
        
        cmdLine.programDataBaseFile =
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strProgram ) );

        if( !boost::filesystem::exists( cmdLine.programDataBaseFile ) )
        {
            std::cout << "Error cannot locate: " << cmdLine.programDataBaseFile.string() << std::endl;
            std::abort();
        }
        
        if( bDebug )
        {
            char c;
            std::cin >> c;
        }
        
        return cmdLine;
    }

}