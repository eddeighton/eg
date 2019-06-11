
#include "project.hpp"

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
            ("host",    po::value< std::string >( &strHost ), "Host type: test, basic, cinder" )
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
        
        const boost::filesystem::path projectFile = 
            projectDirectory / Environment::EG_FILE_EXTENSION;
        
        Environment environment( projectFile );
        
        egxml::EG newEG;
        {
            egxml::Project* pProject = new egxml::Project;
            {
                pProject->Name( "test" );
                pProject->Host( strHost );
                egxml::Build build;
                {
                    build.Name( "release" );
                    build.CompilerFlags( "--donothing" );
                    build.LinkerFlags( "--dontlink" );
                }
                pProject->Build().push_back( build );
                egxml::Run* pRun = new egxml::Run;
                {
                    pRun->Name( "default" );
                }
                pProject->Run().push_back( pRun );
            }
            newEG.Project( pProject );
        }
        
        egxml::EG_saggr newEGagg;
        xml_schema::document_simpl doc_s( newEGagg.root_serializer(), newEGagg.root_name() );
        newEGagg.pre( newEG );
        
        ensureFoldersExist( projectDirectory );
        
        std::unique_ptr< boost::filesystem::ofstream > pFileStream =
            createNewFileStream( projectFile );
        
        doc_s.serialize( *pFileStream, xml_schema::document_simpl::pretty_print );
        
        newEGagg.post();
        
        
    }
}