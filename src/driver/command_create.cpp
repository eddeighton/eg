
#include "schema.hxx"
#include "schema-pimpl.hxx"
#include "schema-simpl.hxx"

#include <boost/program_options.hpp>

#include <iostream>

void command_create( bool bHelp, const std::vector< std::string >& args )
{
    std::string strDirectory;
    std::string strHost;
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Create Project Command");
    {
        commandOptions.add_options()
            ("dir",     po::value< std::string >( &strDirectory ), "Project directory")
            ("host",    po::value< std::string >( &strHost ), "Host type: test_host, basic_host, cinder_host" )
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
        
        EG newEGProject;
        Host h;
        {
            h.Name( "test" );
            h.Command( "thing.exe" );
            h.License( "nobody may use this software under any circumstances even the developer" );
        }
        newEGProject.Host( &h );
        
        EG_saggr newEG;
        xml_schema::document_simpl doc_s( newEG.root_serializer(), newEG.root_name() );
        newEG.pre( newEGProject );
        doc_s.serialize( std::cout, xml_schema::document_simpl::pretty_print );
        newEG.post();
        
        
    }
}