
#include "schema/project.hpp"

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"
#include "egxml/eg_schema-simpl.hxx"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>

#include <iostream>

void command_create( bool bHelp, const std::vector< std::string >& args )
{
    std::string strDirectory;
    std::string strHost;
    std::vector< std::string > packages;
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Create Project Command");
    {
        commandOptions.add_options()
            ("dir",     po::value< std::string >( &strDirectory ), "Project directory")
            ("host",    po::value< std::string >( &strHost ), "Host type: test, basic, cinder" )
            ("package", po::value< std::vector< std::string > >( &packages ), "Packages" )
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
        VERIFY_RTE_MSG( !strHost.empty(), "No host specified" );
        
        const boost::filesystem::path projectDirectory = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDirectory ) );
        
        const boost::filesystem::path projectFile = 
            projectDirectory / Environment::EG_FILE_EXTENSION;
            
        const std::string strName = projectDirectory.stem().string();
        VERIFY_RTE_MSG( !strName.empty(), "Project name cannot be empty: " << projectDirectory.generic_string() );
        
        //create main.eg
        {
            std::ostringstream osFile;
            osFile << strName << Environment::EG_FILE_EXTENSION.string();
            const boost::filesystem::path mainEGFile = 
                projectDirectory / osFile.str();
            if( !boost::filesystem::exists( mainEGFile ) )
            {
                std::unique_ptr< boost::filesystem::ofstream > pFileStream =
                    createNewFileStream( mainEGFile );
            }
        }
        //create main.py
        {
            std::ostringstream osFile;
            osFile << strName << Environment::PYTHON_FILE_EXTENSION.string();
            const boost::filesystem::path mainPythonScript = 
                projectDirectory / osFile.str();
            if( !boost::filesystem::exists( mainPythonScript ) )
            {
                std::unique_ptr< boost::filesystem::ofstream > pFileStream =
                    createNewFileStream( mainPythonScript );
            }
        }
        
        egxml::EG newEG;
        {
            egxml::Project* pProject = new egxml::Project;
            {
                pProject->Name( strName );
                pProject->Host( strHost );
                
                for( const std::string& strPackage : packages )
                    pProject->Package().push_back( strPackage );
                
                //release build
                {
                    egxml::Build build;
                    {
                        build.Name( "release" );
                        build.CompilerFlags( "-DWIN32_LEAN_AND_MEAN -D_MT -D_DLL -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DBOOST_USE_WINDOWS_H -Ofast -fexceptions -Xclang -std=c++17 -Xclang -flto -Xclang -flto-visibility-public-std -Wno-deprecated -Wno-inconsistent-missing-override" );
                        build.LinkerFlags( "-nostdlib -lmsvcrt -Xlinker /SUBSYSTEM:CONSOLE" );
                    }
                    pProject->Build().push_back( build );
                }
                //quick build
                {
                    egxml::Build build;
                    {
                        build.Name( "quick" );
                        build.CompilerFlags( "-DWIN32_LEAN_AND_MEAN -D_MT -D_DLL -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DBOOST_USE_WINDOWS_H -fexceptions -Xclang -std=c++17 -Xclang -flto-visibility-public-std -Wno-deprecated -Wno-inconsistent-missing-override" );
                        build.LinkerFlags( "-nostdlib -lmsvcrt -Xlinker /SUBSYSTEM:CONSOLE" );
                    }
                    pProject->Build().push_back( build );
                }
                //debug build
                {
                    egxml::Build build;
                    {
                        build.Name( "debug" );
                        build.CompilerFlags( "-DWIN32_LEAN_AND_MEAN -g -D_MT -D_DLL -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DBOOST_USE_WINDOWS_H -fexceptions -Xclang -std=c++17 -Xclang -flto-visibility-public-std -Wno-deprecated -Wno-inconsistent-missing-override" );
                        build.LinkerFlags( "-nostdlib -lmsvcrt -Xlinker /SUBSYSTEM:CONSOLE" );
                    }
                    pProject->Build().push_back( build );
                }
                
                egxml::Run* pRun = new egxml::Run;
                {
                    pRun->Name( "default" );
                    pRun->Command( "${PROJECT}/program.exe" );
                    pRun->Argument().push_back( "--database ${PROJECT}/build/database.db" );
                    
                    {
                        std::ostringstream osCmd;
                        osCmd << "--script ${PROJECT}/" << strName << ".py";
                        pRun->Argument().push_back( osCmd.str() );
                    }
                }
                pProject->Run().push_back( pRun );
                
                egxml::Defaults defaults;
                {
                    egxml::Fibers fibers;
                    {
                        egxml::Stack stack;
                        {
                            stack.Size( 16384 );
                        }
                        fibers.Stack( stack );
                    }
                    defaults.Fibers( fibers );
                }
                pProject->Defaults( defaults );
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