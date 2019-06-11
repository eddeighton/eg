

#include "project.hpp"

#include "schema.hxx"
#include "schema-pimpl.hxx"
#include "schema-simpl.hxx"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/escape.hpp"

#include <sstream>
#include <set>
#include <iomanip>

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////

XMLManager::XMLDocPtrMap XMLManager::m_documents;

XMLManager::XMLDocPtr XMLManager::load( const boost::filesystem::path& docPath )
{
    XMLDocPtrMap::const_iterator iFind = m_documents.find( docPath );
    if( iFind != m_documents.end() )
    {
        return iFind->second;
    }
    else
    {
        egxml::EG_paggr eg_p;
        xml_schema::document_pimpl doc_p( eg_p.root_parser(), eg_p.root_name() );
         
        eg_p.pre();
        doc_p.parse( docPath.string() );
        
        XMLDocPtr pDocument( eg_p.post() );
        
        m_documents.insert( std::make_pair( docPath, pDocument ) );
    
        return pDocument;
    }
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
const boost::filesystem::path Environment::EG_FILE_EXTENSION = std::string( ".eg" );
const std::string Environment::EG_INSTALLATION = "EG";
const std::string Environment::CURRENT_PROJECT = "PROJECT";

Environment::Environment( const boost::filesystem::path& projectDir )
    :   m_projectDir( projectDir ),
        m_environment( boost::this_process::environment() )
{
    m_environment[ CURRENT_PROJECT ] = m_projectDir.generic_string();
    
    boost::filesystem::path egInstallationPath;
    {
        boost::system::error_code errorCode;
        boost::filesystem::path currentProgramLoc = boost::dll::program_location( errorCode );
        
        if( !currentProgramLoc.empty() )
        {
            egInstallationPath = boost::filesystem::absolute( 
                boost::filesystem::edsCannonicalise( currentProgramLoc / "../../" ) );
        }
    }
    if( egInstallationPath.empty() )
    {
        THROW_RTE( "Failed to locate eg installation" );
    }
    else
    {
        m_environment[ EG_INSTALLATION ] = egInstallationPath.generic_string();
    }
}

const std::string Environment::get( const std::string& strKey ) const
{
    boost::process::environment::const_iterator iFind = m_environment.find( strKey );
    VERIFY_RTE_MSG( iFind != m_environment.end(), "Failed to locate environment variable: " << strKey );
    std::vector< std::string > strs = iFind->to_vector();
    VERIFY_RTE_MSG( !strs.empty(), "Environment variable is empty: " << strKey );
    std::ostringstream os;
    std::copy( strs.begin(), strs.end(), std::ostream_iterator< std::string >( os ) );
    return os.str();
}
    
std::string Environment::printPath( const boost::filesystem::path& thePath ) const
{
    return Common::escapeString( thePath.generic_string() );
}

void Environment::startCompilationCommand( std::ostream& os ) const
{
    static boost::filesystem::path CLANG;// = boost::process::search_path( "clang.exe" );
    if( CLANG.empty() )
    {
        CLANG = boost::filesystem::path( get( EG_INSTALLATION ) ) / "third_party/install/llvm/bin/clang.exe";
    }
    
    os << printPath( CLANG ) << " ";
}

const boost::filesystem::path& Environment::getEGLibraryInclude() const
{
    static boost::filesystem::path EG_LIBRARY;
    if( EG_LIBRARY.empty() )
    {
        EG_LIBRARY = boost::filesystem::path( get( EG_INSTALLATION ) ) / "runtime";
    }
    return EG_LIBRARY;
}

std::string Environment::expand( const std::string& strPath ) const
{
    return ::expand( strPath, [ this ]( const std::string& strKey ){ return get( strKey ); } );
}

const egxml::Host& Environment::getHost( const std::string& strHost ) const
{
    VERIFY_RTE_MSG( !strHost.empty(), "Empty host specification: " << getProjectDir() );
    
    boost::filesystem::path hostPath = 
        boost::filesystem::path( get( EG_INSTALLATION ) ) / "third_party/install" / strHost / Environment::EG_FILE_EXTENSION;
    VERIFY_RTE_MSG( boost::filesystem::exists( hostPath ), "Failed to locate host at: " << hostPath.generic_string() );
    
    XMLManager::XMLDocPtr pDoc = XMLManager::load( hostPath );
    VERIFY_RTE_MSG( pDoc->choice_arm() == egxml::EG::Host_tag, "Directory is not a host: " << hostPath.generic_string() );
    return pDoc->Host();
}

const egxml::Package& Environment::getPackage( const std::string& strPackage ) const
{
    boost::filesystem::path packagePath = 
        boost::filesystem::path( get( EG_INSTALLATION ) ) / "third_party/install" / strPackage / Environment::EG_FILE_EXTENSION;
    VERIFY_RTE_MSG( boost::filesystem::exists( packagePath ), "Failed to locate package at: " << packagePath.generic_string() );
    
    XMLManager::XMLDocPtr pDoc = XMLManager::load( packagePath );
    VERIFY_RTE_MSG( pDoc->choice_arm() == egxml::EG::Package_tag, "Directory is not a package: " << packagePath.generic_string() );
    return pDoc->Package();
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
Project::Project( const Environment& environment, const egxml::Project& project, const std::string& strBuildCommand )
    :   m_environment( environment ),
        m_project( project ),
        m_strBuildCommand( strBuildCommand ),
        m_host( m_environment.getHost( m_project.Host() ) )
{
    for( const std::string& strPackage : m_project.Package() )
    {
        m_packages.push_back( m_environment.getPackage( strPackage ) );
    }
}

const std::string& Project::getCompilerFlags() const 
{
    for( const egxml::Build& build : m_project.Build() )
    {
        if( build.Name() == m_strBuildCommand )
        {
            return build.CompilerFlags(); 
        }
    }
    THROW_RTE( "Failed to locate build command: " << m_strBuildCommand << " in project: " << m_environment.getProjectDir() );
}
const std::string& Project::getLinkerFlags() const 
{ 
    for( const egxml::Build& build : m_project.Build() )
    {
        if( build.Name() == m_strBuildCommand )
        {
            return build.LinkerFlags(); 
        }
    }
    THROW_RTE( "Failed to locate build command: " << m_strBuildCommand << " in project: " << m_environment.getProjectDir() );
}

std::vector< boost::filesystem::path > Project::getEGSourceCode() const
{
    std::vector< boost::filesystem::path > egSourceCode;
    
    for( boost::filesystem::directory_iterator iter( m_environment.getProjectDir() );
        iter != boost::filesystem::directory_iterator(); ++iter )
    {
        const boost::filesystem::path& filePath = *iter;
        if( !boost::filesystem::is_directory( filePath ) )
        {
            if( boost::filesystem::extension( *iter ) == Environment::EG_FILE_EXTENSION )
            {
                if( !filePath.stem().empty() ) //ignore .eg xml files
                {
                    egSourceCode.push_back( *iter );
                }
                else
                {
                    //make this recursive...
                }
            }
        }
    }
    return egSourceCode;
}

static void collateSystemIncludeFiles( 
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Files& xmlFiles )
{
    for( const std::string& strFile : xmlFiles.System() )
    {
        VERIFY_RTE_MSG( !strFile.empty(), "System include file is empty" );
        if( strFile.front() != '<' )
        {
            std::ostringstream os;
            os << '<' << strFile << '>';
            const boost::filesystem::path absPath = os.str();
            if( 0 == uniquified.count( absPath ) )
            {
                uniquified.insert( absPath );
                directories.push_back( absPath );
            }
        }
        else
        {
            const boost::filesystem::path absPath = strFile;
            if( 0 == uniquified.count( absPath ) )
            {
                uniquified.insert( absPath );
                directories.push_back( absPath );
            }
        }
        
    }
}

std::vector< boost::filesystem::path > Project::getHostSystemIncludes() const
{
    std::set< boost::filesystem::path > uniquified;
    std::vector< boost::filesystem::path > includes;
    
    if( m_host.Files_present() )
        collateSystemIncludeFiles( uniquified, includes, m_host.Files() );
    
    for( const egxml::Package& package : m_packages )
    {
        if( package.Files_present() )
            collateSystemIncludeFiles( uniquified, includes, package.Files() );
    }
    
    return includes;
}

static void collateUserIncludeFiles( 
    const Environment& environment,
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Files& xmlFiles )
{
    for( const std::string& strFile : xmlFiles.Include() )
    {
        const boost::filesystem::path absPath =
            boost::filesystem::edsCannonicalise(
                environment.expand( strFile ) );
                
        if( 0 == uniquified.count( absPath ) )
        {
            uniquified.insert( absPath );
            directories.push_back( absPath );
        }
    }
}

std::vector< boost::filesystem::path > Project::getHostUserIncludes() const
{
    std::set< boost::filesystem::path > uniquified;
    std::vector< boost::filesystem::path > includes;
    
    if( m_host.Files_present() )
        collateUserIncludeFiles( m_environment, uniquified, includes, m_host.Files() );
    
    for( const egxml::Package& package : m_packages )
    {
        if( package.Files_present() )
            collateUserIncludeFiles( m_environment, uniquified, includes, package.Files() );
    }
    
    return includes;
}

static void collateIncludeDirectories( 
    const Environment& environment,
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Directories& xmlDirectories )
{
    for( const std::string& strInclude : xmlDirectories.Include() )
    {
        const boost::filesystem::path absPath =
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    environment.expand( strInclude ) ) );
                
        if( 0 == uniquified.count( absPath ) )
        {
            uniquified.insert( absPath );
            directories.push_back( absPath );
        }
    }
}

std::vector< boost::filesystem::path > Project::getIncludeDirectories() const
{
    std::set< boost::filesystem::path > uniquified;
    std::vector< boost::filesystem::path > directories;
    
    if( m_host.Directories_present() )
        collateIncludeDirectories( m_environment, uniquified, directories, m_host.Directories() );
    
    for( const egxml::Package& package : m_packages )
    {
        if( package.Directories_present() )
            collateIncludeDirectories( m_environment, uniquified, directories, package.Directories() );
    }
    
    return directories;
}

static void collateUserSourceFiles( 
    const Environment& environment,
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Files& xmlFiles )
{
    for( const std::string& strFile : xmlFiles.Source() )
    {
        const boost::filesystem::path absPath =
            boost::filesystem::edsCannonicalise(
                environment.expand( strFile ) );
                
        if( 0 == uniquified.count( absPath ) )
        {
            uniquified.insert( absPath );
            directories.push_back( absPath );
        }
    }
}

std::vector< boost::filesystem::path > Project::getCPPSourceCode() const
{
    std::set< boost::filesystem::path > uniquified;
    std::vector< boost::filesystem::path > sourceFiles;
    
    if( m_host.Files_present() )
        collateUserSourceFiles( m_environment, uniquified, sourceFiles, m_host.Files() );
    
    for( const egxml::Package& package : m_packages )
    {
        if( package.Files_present() )
            collateUserSourceFiles( m_environment, uniquified, sourceFiles, package.Files() );
    }
    
    return sourceFiles;
}

static void collateUserLibraryFiles( 
    const Environment& environment,
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Files& xmlFiles )
{
    for( const std::string& strFile : xmlFiles.Library() )
    {
        const boost::filesystem::path absPath =
            boost::filesystem::edsCannonicalise(
                environment.expand( strFile ) );
                
        if( 0 == uniquified.count( absPath ) )
        {
            uniquified.insert( absPath );
            directories.push_back( absPath );
        }
    }
}

std::vector< boost::filesystem::path > Project::getCPPLibs() const
{
    std::set< boost::filesystem::path > uniquified;
    std::vector< boost::filesystem::path > libFiles;
    
    if( m_host.Files_present() )
        collateUserLibraryFiles( m_environment, uniquified, libFiles, m_host.Files() );
    
    for( const egxml::Package& package : m_packages )
    {
        if( package.Files_present() )
            collateUserLibraryFiles( m_environment, uniquified, libFiles, package.Files() );
    }
    
    return libFiles;
}

boost::filesystem::path Project::getIntermediateFolder() const
{
    boost::filesystem::path intermediateFolder = 
        m_environment.getProjectDir() / ( std::string( "build" ) );
    if( !intermediateFolder.empty() )
    {
        boost::filesystem::ensureFoldersExist( intermediateFolder );
    }
    return intermediateFolder;
}

boost::filesystem::path Project::getTreeFileName() const
{
    std::ostringstream os;
    os << "master.db";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getIncludeHeader() const
{
    std::ostringstream os;
    os << "includes.hpp";
    return boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( 
                getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getIncludePCH() const
{
    std::ostringstream os;
    os << "includes.pch";
    return boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( 
                getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getInterfaceHeader() const
{
    std::ostringstream os;
    os << "interface.hpp";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getInterfacePCH() const
{
    std::ostringstream os;
    os << "interface.pch";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}


boost::filesystem::path Project::getOperationsHeader( std::size_t szUnitID ) const
{
    std::ostringstream os;
    os << "operations_" << std::setw( 3 ) << std::setfill( '0' ) << szUnitID - 2 << ".hpp";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getTUName( std::size_t szUnitID ) const
{
    std::ostringstream os;
    os << "tu_" << std::setw( 3 ) << std::setfill( '0' ) << szUnitID - 2 << ".db";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getOperationsPCH( std::size_t szUnitID ) const
{
    std::ostringstream os;
    os << "operations_" << std::setw( 3 ) << std::setfill( '0' ) << szUnitID - 2 << ".pch";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getAnalysisFileName() const
{
    std::ostringstream os;
    os << "database.db";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getDataStructureSource() const
{
    std::ostringstream os;
    os << "structures.hpp";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getImplementationSource( int szUnitID ) const
{
    std::ostringstream os;
    os << "impl_" << std::setw( 3 ) << std::setfill( '0' ) << szUnitID - 2 << ".cpp";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getObjectName( int szUnitID ) const
{
    std::ostringstream os;
    os << "object_" << std::setw( 3 ) << std::setfill( '0' ) << szUnitID - 2 << ".obj";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getHostCommand() const
{
    return m_environment.expand( m_host.Command() );
}
        
boost::filesystem::path Project::getProgramName() const
{
    std::ostringstream os;
    os << "program.exe";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( os.str() ) );
}
