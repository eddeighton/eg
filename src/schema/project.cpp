

#include "project.hpp"

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"
#include "egxml/eg_schema-simpl.hxx"

#include "installationxml/installation_schema.hxx"
#include "installationxml/installation_schema-pimpl.hxx"
#include "installationxml/installation_schema-simpl.hxx"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/escape.hpp"

#include <sstream>
#include <set>
#include <iomanip>
#include <iostream>

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
        try
        {
            egxml::EG_paggr eg_p;
            xml_schema::document_pimpl doc_p( eg_p.root_parser(), eg_p.root_name() );
             
            eg_p.pre();
            doc_p.parse( docPath.string() );
            
            XMLDocPtr pDocument( eg_p.post() );
            
            m_documents.insert( std::make_pair( docPath, pDocument ) );
        
            return pDocument;
        }
        catch( const xml_schema::parser_exception& e )
        {
            std::ostringstream os;
            os << "XML error opening: " << docPath.generic_string() << " " << e.line() << ":" << e.column() << ": " << e.text();
            throw std::runtime_error( os.str() );
        }
    }
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
const boost::filesystem::path Environment::EG_FILE_EXTENSION = std::string( ".eg" );
const boost::filesystem::path Environment::INSTALLATION_FILE = std::string( "installation.xml" );
const boost::filesystem::path Environment::WIZARD_FILE = std::string( "wizard.xml" );

const boost::filesystem::path Environment::PYTHON_FILE_EXTENSION = std::string( ".py" );

const std::string Environment::ENV_KEY_EG_INSTALLATION = "EG";
/*
const std::string Environment::ENV_KEY_WINDOWS_10_SDK = "WINDOWS_10_SDK";
const std::string Environment::ENV_KEY_VISUALSTUDIO = "EG_VISUAL_STUDIO";
const std::string Environment::ENV_KEY_PYTHONHOME = "PYTHONHOME";*/

const std::string Environment::ENV_KEY_CURRENT_PROJECT = "PROJECT";

void Environment::commonCtor()
{
	if( m_environment.count( ENV_KEY_EG_INSTALLATION ) == 0U )
	{
        THROW_RTE( "Missing EG environment variable for eg installation location." );
	}
	
    /*std::optional< boost::filesystem::path > egInstallationPath;
    std::optional< boost::filesystem::path > egInstallationXMLFile;
    {
        boost::system::error_code errorCode;
        boost::filesystem::path currentProgramLoc = boost::dll::program_location( errorCode );
        VERIFY_RTE_MSG( !errorCode, "Error attempting to locate program location: " << errorCode );
        boost::filesystem::path pathIter = currentProgramLoc.parent_path();
        while( !pathIter.empty() )
        {
            boost::filesystem::path nextPath = pathIter / "installation.xml";
            if( boost::filesystem::exists( nextPath ) && boost::filesystem::is_regular_file( nextPath ) )
            {
                egInstallationPath = pathIter;
                egInstallationXMLFile = nextPath;
                break;
            }
            else
            {
                pathIter = pathIter.parent_path();
            }
        }
    }
    if( !egInstallationPath || !egInstallationXMLFile )
    {
        THROW_RTE( "Failed to locate eg installation.  Missing " );
    }
    else
    {
        m_environment[ ENV_KEY_EG_INSTALLATION ] = egInstallationPath.value().generic_string();
        
        m_environment[ "PATH" ] += egInstallationPath.value().generic_string();
    }
    
    //std::cout << "loading installation: " << egInstallationXMLFile.value().generic_string() << std::endl;
    //m_environment[ ENV_KEY_WINDOWS_10_SDK ] = "C:/Program Files (x86)/Windows Kits/10";
    
    try
    {
        installationxml::Installation_paggr installation_p;
        xml_schema::document_pimpl doc_p( installation_p.root_parser(), installation_p.root_name() );
        installation_p.pre();
        doc_p.parse( egInstallationXMLFile.value().generic_string() );
        installationxml::Installation installation = installation_p.post();
        
        if( m_environment.count( ENV_KEY_WINDOWS_10_SDK ) == 0U )
        {
            boost::filesystem::path xmlPath = installation.SDK();
            if( boost::filesystem::exists( xmlPath ) )
            {
                m_environment[ ENV_KEY_WINDOWS_10_SDK ] = xmlPath.generic_string();
            }
            else
            {
                THROW_RTE( "Failed to locate Windows 10 SDK Installation" );
            }
        }
        
        if( m_environment.count( ENV_KEY_PYTHONHOME ) == 0U )
        {
            boost::filesystem::path xmlPath = installation.Python();
            if( boost::filesystem::exists( xmlPath ) )
            {
                m_environment[ ENV_KEY_PYTHONHOME ] = xmlPath.generic_string();
            }
            else
            {
                THROW_RTE( "Failed to locate Python3 Installation" );
            }
        }
        
        if( m_environment.count( ENV_KEY_VISUALSTUDIO ) == 0U )
        {
            boost::filesystem::path xmlPath = installation.Toolchain();
            if( boost::filesystem::exists( xmlPath ) )
            {
                m_environment[ ENV_KEY_VISUALSTUDIO ] = xmlPath.generic_string();
            }
            else
            {
                THROW_RTE( "Failed to locate Visual Studio 2017 Installation" );
            }
        }
    }
    catch( const xml_schema::parser_exception& e )
    {
        std::ostringstream os;
        os << "XML error opening: " << egInstallationXMLFile.value().generic_string() << 
            " " << e.line() << ":" << e.column() << ": " << e.text();
        throw std::runtime_error( os.str() );
    }*/
}

Environment::Environment()
    :   m_environment( boost::this_process::environment() )
{
    commonCtor();
}

Environment::Environment( const boost::filesystem::path& projectDir )
    :   m_environment( boost::this_process::environment() )
{
    m_environment[ ENV_KEY_CURRENT_PROJECT ] = projectDir.generic_string();
    commonCtor();
}

Environment::Environment( const boost::process::environment& env, const boost::filesystem::path& projectDir )
    :   m_environment( env )
{
    m_environment[ ENV_KEY_CURRENT_PROJECT ] = projectDir.generic_string();
    commonCtor();
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
    static boost::filesystem::path CLANG = boost::process::search_path( "clang.exe" );
    if( CLANG.empty() )
    {
		THROW_RTE("Failed to locate clang.exe in path.  Please ensure the correct clang.exe is configured.");
        /*boost::process::environment::const_iterator iFind = m_environment.find( "EG_CLANG" );
        if( iFind != m_environment.end() )
        {
            std::vector< std::string > strs = iFind->to_vector();
            std::ostringstream os;
            std::copy( strs.begin(), strs.end(), std::ostream_iterator< std::string >( os ) );
            CLANG = os.str();
        }
        if( CLANG.empty() )
            CLANG = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "third_party/install/llvm/bin/clang.exe";*/
    }
    os << printPath( CLANG ) << " ";
}


void Environment::startLogCommand( std::ostream& os ) const
{
    static boost::filesystem::path EGLOG;
    if( EGLOG.empty() )
    {
#ifdef _DEBUG
        EGLOG = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "bin/eglogd.exe";
#else
        EGLOG = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "bin/eglog.exe";
#endif
    }
    os << printPath( EGLOG ) << " ";
}

void Environment::startDriverCommand( std::ostream& os ) const
{
    static boost::filesystem::path EGDRIVER;
    if( EGDRIVER.empty() )
    {
#ifdef _DEBUG
        EGDRIVER = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "bin/egd.exe";
#else
        EGDRIVER = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "bin/eg.exe";
#endif
    }
    os << printPath( EGDRIVER ) << " ";
}

const boost::filesystem::path& Environment::getClangPluginDll() const
{
    static boost::filesystem::path EG_CLANG_PLUGIN;
    if( EG_CLANG_PLUGIN.empty() )
    {
#ifdef _DEBUG
        EG_CLANG_PLUGIN = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "bin/eg_clang_plugind.dll";
#else
        EG_CLANG_PLUGIN = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "bin/eg_clang_plugin.dll";
#endif
    }
    return EG_CLANG_PLUGIN;
}

const boost::filesystem::path& Environment::getEGLibraryInclude() const
{
    static boost::filesystem::path EG_LIBRARY;
    if( EG_LIBRARY.empty() )
    {
        EG_LIBRARY = boost::filesystem::path( get( ENV_KEY_EG_INSTALLATION ) ) / "include";
    }
    return EG_LIBRARY;
}

std::string Environment::expand( const std::string& strPath ) const
{
    return ::expand( strPath, [ this ]( const std::string& strKey ){ return get( strKey ); } );
}

const egxml::Host& Environment::getHost( const std::string& strHost ) const
{
    VERIFY_RTE_MSG( !strHost.empty(), "Empty host specification" );
	
	const boost::filesystem::path hostPath =
		boost::filesystem::edsCannonicalise(
			expand( strHost ) / Environment::EG_FILE_EXTENSION );

    //boost::filesystem::path hostPath = strHost / Environment::EG_FILE_EXTENSION;
    VERIFY_RTE_MSG( boost::filesystem::exists( hostPath ), "Failed to locate host at: " << hostPath.generic_string() );
    
    XMLManager::XMLDocPtr pDoc = XMLManager::load( hostPath );
    VERIFY_RTE_MSG( pDoc->choice_arm() == egxml::EG::Host_tag, "Directory is not a host: " << hostPath.generic_string() );
    return pDoc->Host();
}

const egxml::Package& Environment::getPackage( const std::string& strPackage ) const
{
    VERIFY_RTE_MSG( !strPackage.empty(), "Empty package specification" );
	
	const boost::filesystem::path packagePath =
		boost::filesystem::edsCannonicalise(
			expand( strPackage ) / Environment::EG_FILE_EXTENSION );
			
    //boost::filesystem::path packagePath = strPackage / Environment::EG_FILE_EXTENSION;
    VERIFY_RTE_MSG( boost::filesystem::exists( packagePath ), "Failed to locate package at: " << packagePath.generic_string() );
    
    XMLManager::XMLDocPtr pDoc = XMLManager::load( packagePath );
    VERIFY_RTE_MSG( pDoc->choice_arm() == egxml::EG::Package_tag, "Directory is not a package: " << packagePath.generic_string() );
    return pDoc->Package();
}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
Project::Project( const boost::filesystem::path& projectDir, 
    const Environment& environment, const egxml::Project& project )
    :   m_projectDir( projectDir ),
        m_environment( environment.getEnvironment(), projectDir ),
        m_project( project ),
        m_host( m_environment.getHost( m_project.Host() ) )
{
    for( const std::string& strPackage : m_project.Package() )
    {
        m_packages.push_back( m_environment.getPackage( strPackage ) );
    }
}
        
Project::Project( const boost::filesystem::path& projectDir, 
            const Environment& environment, const egxml::Project& project, const std::string& strBuildCommand )
    :   m_projectDir( projectDir ),
        m_environment( environment.getEnvironment(), projectDir ),
        m_project( project ),
        m_strBuildCommand( strBuildCommand ),
        m_host( m_environment.getHost( m_project.Host() ) )
{
    for( const std::string& strPackage : m_project.Package() )
    {
        m_packages.push_back( m_environment.getPackage( strPackage ) );
    }
}

const std::string Project::getCompilerFlags() const 
{
    std::ostringstream osFlags;
    if( !m_strBuildCommand.empty() )
    {
        for( const egxml::Build& build : m_project.Build() )
        {
            if( build.Name() == m_strBuildCommand )
            {
                osFlags << build.CompilerFlags(); 
                osFlags << " " << m_host.CompilerFlags();
                return osFlags.str();
            }
        }
    }
    THROW_RTE( "Failed to locate build command: " << m_strBuildCommand << " in project: " << m_projectDir );
}
const std::string Project::getLinkerFlags() const 
{ 
    std::ostringstream osFlags;
    if( !m_strBuildCommand.empty() )
    {
        for( const egxml::Build& build : m_project.Build() )
        {
            if( build.Name() == m_strBuildCommand )
            {
                osFlags << build.LinkerFlags(); 
                osFlags << " " << m_host.LinkerFlags();
                return osFlags.str();
            }
        }
    }
    THROW_RTE( "Failed to locate build command: " << m_strBuildCommand << " in project: " << m_projectDir );
}
    
std::vector< boost::filesystem::path > Project::getEGSourceCode() const
{
    std::vector< boost::filesystem::path > egSourceCode;
    
    for( boost::filesystem::directory_iterator iter( m_projectDir );
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


static void collateLibraryDirectories( 
    const Environment& environment,
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Directories& xmlDirectories )
{
    for( const std::string& strLibrary : xmlDirectories.Library() )
    {
        const boost::filesystem::path absPath =
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    environment.expand( strLibrary ) ) );
                
        if( 0 == uniquified.count( absPath ) )
        {
            uniquified.insert( absPath );
            directories.push_back( absPath );
        }
    }
}

std::vector< boost::filesystem::path > Project::getLibraryDirectories() const
{
    std::set< boost::filesystem::path > uniquified;
    std::vector< boost::filesystem::path > directories;
    
    if( m_host.Directories_present() )
        collateLibraryDirectories( m_environment, uniquified, directories, m_host.Directories() );
    
    for( const egxml::Package& package : m_packages )
    {
        if( package.Directories_present() )
            collateLibraryDirectories( m_environment, uniquified, directories, package.Directories() );
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
    
    if( m_project.Files_present() )
        collateUserSourceFiles( m_environment, uniquified, sourceFiles, m_project.Files() );
    
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
    boost::filesystem::path intermediateFolder = m_projectDir / ( std::string( "build" ) );
    if( !intermediateFolder.empty() )
    {
        boost::filesystem::ensureFoldersExist( intermediateFolder );
    }
    return intermediateFolder;
}

boost::filesystem::path Project::getParserDBFileName() const
{
    std::ostringstream os;
    os << "parser.db";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getInterfaceDBFileName() const
{
    std::ostringstream os;
    os << "interface.db";
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

boost::filesystem::path Project::getPreprocessedFile() const
{
    std::ostringstream os;
    os << "includes_pre.hpp";
    return boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( 
                getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getPreprocessedCompareFile() const
{
    std::ostringstream os;
    os << "includes_pre_cmp.hpp";
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


boost::filesystem::path Project::getOperationsHeader( const std::string& strTUName ) const
{
    std::ostringstream os;
    os << "operations_" << strTUName << ".hpp";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getTUDBName( const std::string& strTUName ) const
{
    std::ostringstream os;
    os << "tu_" << strTUName << ".db";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getOperationsPCH( const std::string& strTUName ) const
{
    std::ostringstream os;
    os << "operations_" << strTUName << ".pch";
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

boost::filesystem::path Project::getImplementationSource( const std::string& strTUName ) const
{
    std::ostringstream os;
    os << "impl_" << strTUName << ".cpp";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getRuntimeSource() const
{
    std::ostringstream os;
    os << "runtime.cpp";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}
    
boost::filesystem::path Project::getObjectName( const std::string& strTUName ) const
{
    std::ostringstream os;
    os << "object_" << strTUName << ".obj";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Project::getObjectFile( const boost::filesystem::path& sourceFile ) const
{
    std::ostringstream os;
    os << "object_" << sourceFile.stem().string() << ".obj";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}
    
std::vector< boost::filesystem::path > Project::getCommands() const
{
    std::vector< boost::filesystem::path > result;
    for( const std::string& strCommand : m_host.Command() )
    {
        result.push_back( m_environment.expand( strCommand ) );
    }
    
    for( const egxml::Package& package : m_packages )
    {
        for( const std::string& strCommand : package.Command() )
        {
            result.push_back( m_environment.expand( strCommand ) );
        }
    }
    
    return result;
}

bool Project::isPybindRequired() const
{
    for( const egxml::Package& package : m_packages )
    {
		if( package.Name() == "pybind11" )
		{
			return true;
		}
    }
	
	return false;
}
	
boost::filesystem::path Project::getProgramName() const
{
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( getProjectDir() / "program.exe" ) );
}
