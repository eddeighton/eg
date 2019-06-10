
#include "schema.hxx"
#include "schema-pimpl.hxx"
#include "schema-simpl.hxx"

#include "common/file.hpp"

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
        
        
        const boost::filesystem::path projectDirectory = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDirectory ) );
        
        const boost::filesystem::path projectFile = projectDirectory / ".eg";
        
        
        
        
        
        EG newEG;
        Project project;
        project.Name( "test" );
        Host projectHost;
        {
            projectHost.Name( "testHost" );
            projectHost.Command( "thing.exe" );
            projectHost.License( "nobody may use this software under any circumstances even the developer" );
        }
        project.Host( &projectHost );
        Build build;
        {
            build.Name( "release" );
            build.CompilerFlags( "--donothing" );
            build.LinkerFlags( "--dontlink" );
        }
        project.Build().push_back( build );
        Run run;
        {
            run.Name( "default" );
        }
        project.Run().push_back( &run );
        newEG.Project( &project );
        
        EG_saggr newEGagg;
        xml_schema::document_simpl doc_s( newEGagg.root_serializer(), newEGagg.root_name() );
        newEGagg.pre( newEG );
        
        ensureFoldersExist( projectDirectory );
        
        std::unique_ptr< boost::filesystem::ofstream > pFileStream =
            createNewFileStream( projectFile );
        
        doc_s.serialize( *pFileStream, xml_schema::document_simpl::pretty_print );
        
        newEGagg.post();
        
        
    }
}