

#include "schema.hxx"
#include "schema-pimpl.hxx"
#include "schema-simpl.hxx"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <iostream>


void command_build( bool bHelp, const std::vector< std::string >& args )
{
    std::string strDirectory;
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Create Project Command");
    {
        commandOptions.add_options()
            ("dir",     po::value< std::string >( &strDirectory ), "Project directory")
        ;
    }
    
    po::positional_options_description p;
    p.add( "dir", -1 );
    
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
        
        const boost::filesystem::path projectFile = projectDirectory / ".eg";
        
        if( !boost::filesystem::exists( projectFile ) )
        {
            THROW_RTE( "Could not locate .eg file in directory: " << projectDirectory.generic_string() );
        }
        
        
        
        EG_paggr eg_p;
        xml_schema::document_pimpl doc_p( eg_p.root_parser(), eg_p.root_name() );
         
        eg_p.pre();
        doc_p.parse( projectFile.string() );
        EG* pEG = eg_p.post();
        
        std::cout << "Found eg project: " << pEG->Project().Name() << std::endl;
    }
    
}
