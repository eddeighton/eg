

#include "project.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/escape.hpp"

#pragma warning( push )
#pragma warning( disable : 4996) //iterator thing
#pragma warning( disable : 4244) //conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

#include <sstream>
#include <set>

const boost::filesystem::path Environment::EG_FILE_EXTENSION = std::string( ".eg" );

std::string printPath( const boost::filesystem::path& thePath )
{
    return Common::escapeString( thePath.generic_string() );
}

void Environment::startCompilationCommand( std::ostream& os )
{
    static boost::filesystem::path CLANG = boost::process::search_path( "clang.exe" );
    
    os << printPath( CLANG ) << " ";
}
const boost::filesystem::path& Environment::getEGLibraryInclude()
{
    static boost::filesystem::path EG_LIBRARY;
    if( const char* pszEnvPath = getenv( "EG" ) )
    {
        EG_LIBRARY = std::string( pszEnvPath );
    }
    return EG_LIBRARY;
}

std::vector< boost::filesystem::path > Project::getEGSourceCode() const
{
    std::vector< boost::filesystem::path > egSourceCode;
    
    for( boost::filesystem::directory_iterator iter( m_directory );
        iter != boost::filesystem::directory_iterator(); ++iter )
    {
        const boost::filesystem::path& filePath = *iter;
        if( !boost::filesystem::is_directory( filePath ) && 
            !filePath.stem().empty() )
        {
            if( boost::filesystem::extension( *iter ) == Environment::EG_FILE_EXTENSION )
            {
                egSourceCode.push_back( *iter );
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
    
    collateSystemIncludeFiles( uniquified, includes, m_project.Host().Files() );
    
    for( const egxml::Package& package : m_project.Package() )
    {
        collateSystemIncludeFiles( uniquified, includes, package.Files() );
    }
    
    return includes;
}

static void collateUserIncludeFiles( 
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Files& xmlFiles )
{
    for( const std::string& strFile : xmlFiles.User() )
    {
        const boost::filesystem::path absPath =
            boost::filesystem::edsCannonicalise( strFile );
                //boost::filesystem::absolute( strFile ) );
                
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
    
    collateUserIncludeFiles( uniquified, includes, m_project.Host().Files() );
    
    for( const egxml::Package& package : m_project.Package() )
    {
        collateUserIncludeFiles( uniquified, includes, package.Files() );
    }
    
    return includes;
}

static void collateIncludeDirectories( 
    std::set< boost::filesystem::path >& uniquified, 
    std::vector< boost::filesystem::path >& directories,
    const egxml::Directories& xmlDirectories )
{
    for( const std::string& strInclude : xmlDirectories.Include() )
    {
        const boost::filesystem::path absPath =
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strInclude ) );
                
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
    
    collateIncludeDirectories( uniquified, directories, m_project.Host().Directories() );
    
    for( const egxml::Package& package : m_project.Package() )
    {
        collateIncludeDirectories( uniquified, directories, package.Directories() );
    }
    
    return directories;
}

boost::filesystem::path Project::getIntermediateFolder() const
{
    boost::filesystem::path intermediateFolder = 
        m_directory / ( std::string( "build" ) );
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