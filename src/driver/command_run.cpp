


#include "project.hpp"

#include "schema.hxx"
#include "schema-pimpl.hxx"
#include "schema-simpl.hxx"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <iostream>

void command_run( bool bHelp, const std::vector< std::string >& args )
{
    std::string strDirectory;
    std::string strCommand = "default";
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Create Project Command");
    {
        commandOptions.add_options()
            ("dir",     po::value< std::string >( &strDirectory ), "Project directory")
            ("cmd",     po::value< std::string >( &strCommand ), "Run command name" )
        ;
    }
    
    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );
    
    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        
        const boost::filesystem::path projectDirectory = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDirectory ) );

        if( !boost::filesystem::exists( projectDirectory ) )
        {
            THROW_RTE( "Specified directory does not exist: " << projectDirectory.generic_string() );
        }
        else if( !boost::filesystem::is_directory( projectDirectory ) )
        {
            THROW_RTE( "Specified path is not a directory: " << projectDirectory.generic_string() );
        }
        
        const boost::filesystem::path projectFile = projectDirectory / Environment::EG_FILE_EXTENSION;
        
        if( !boost::filesystem::exists( projectFile ) )
        {
            THROW_RTE( "Could not locate " << Environment::EG_FILE_EXTENSION << " file in directory: " << projectDirectory.generic_string() );
        }
        
        XMLManager::XMLDocPtr pDocument = XMLManager::load( projectFile );
        
        Environment environment( projectDirectory );
        
        const egxml::Project& project = pDocument->Project();
        
        boost::optional< const egxml::Run& > foundCommand;
            
        for( const egxml::Run& runCmd : project.Run() )
        {
            if( runCmd.Name() == strCommand )
            {
                foundCommand = runCmd;
                break;
            }
        }
        
        if( foundCommand )
        {
            std::ostringstream osCmd;
            
            osCmd << environment.expand( foundCommand.get().Command() );
            for( const auto& arg : foundCommand.get().Argument() )
            {
                osCmd << " " << environment.expand( arg );
            }
            
            {
                const int iResult = boost::process::system( osCmd.str() );
                if( iResult )
                {
                    THROW_RTE( "Error invoking clang++ " << iResult );
                }
            }
        }
        else
        {
            THROW_RTE( "Failed to locate command: " << strCommand );
        }
            
        
    }
}