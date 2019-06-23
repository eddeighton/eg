


#include "schema/installationxml/installation_schema.hxx"
#include "schema/installationxml/installation_schema-pimpl.hxx"
#include "schema/installationxml/installation_schema-simpl.hxx"
#include "schema/project.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>


int main( int argc, const char* argv[] )
{
    std::string strDirectory;
    std::string strPython;
    std::string strToolchain;
    std::string strSDK;
    
    namespace po = boost::program_options;
    po::options_description commandOptions("installation commands");
    {
        commandOptions.add_options()
            ("dir",        po::value< std::string >( &strDirectory ), "Installation folder" )
            ("python",     po::value< std::string >( &strPython ), "Python executable")
            ("toolchain",     po::value< std::string >( &strToolchain ), "C++ Toolchain path")
            ("SDK",     po::value< std::string >( &strSDK ), "SDK path")
        ;
    }
    
    po::variables_map vm;
    po::store( po::command_line_parser( argc, argv ).options( commandOptions ).run(), vm );
    po::notify( vm );
    
    const boost::filesystem::path projectDirectory = 
        boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( strDirectory ) );
    
    const boost::filesystem::path projectFile = 
        projectDirectory / Environment::INSTALLATION_FILE;
    
    ensureFoldersExist( projectDirectory );
    
    installationxml::Installation installation;
    {
        installation.Version( "1" );
        installation.Python( strPython );
        installation.Toolchain( strToolchain );
        installation.SDK( strSDK );
    }
    
    installationxml::Installation_saggr newInstallation;
    xml_schema::document_simpl doc_s( newInstallation.root_serializer(), newInstallation.root_name() );
    newInstallation.pre( installation );
    
    std::unique_ptr< boost::filesystem::ofstream > pFileStream =
        createNewFileStream( projectFile );
    
    doc_s.serialize( *pFileStream, xml_schema::document_simpl::pretty_print );
    
    newInstallation.post();
    
    
}