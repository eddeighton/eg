



#include "project.hpp"

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"
#include "egxml/eg_schema-simpl.hxx"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <iostream>

void command_log( bool bHelp, const std::vector< std::string >& args )
{
    std::string strDirectory;
    std::string strCommand = "default";
    std::vector< std::string > filters;
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Read Project Log");
    {
        commandOptions.add_options()
            ("dir",         po::value< std::string >( &strDirectory ),              "Project directory")
            ("filters",     po::value< std::vector< std::string > >( &filters ),    "Filters ( default parameter )" )
        ;
    }
    
    po::positional_options_description p;
    p.add( "filters", -1 );
    
    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
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
        
        //const egxml::Project& project = pDocument->Project();
        
        std::ostringstream osCmd;
        environment.startLogCommand( osCmd );
        
        osCmd << "--log " << environment.printPath( projectDirectory / "log" ) << " " <<
              "--data " << environment.printPath( projectDirectory / "build/database.db" ) << " ";
              
        std::copy( filters.begin(), filters.end(), std::ostream_iterator< std::string >( osCmd, " " ) );
        
        const int iResult = boost::process::system( osCmd.str() );
        if( iResult )
        {
            THROW_RTE( "Error invoking eglog " << iResult );
        }
        
    }
}