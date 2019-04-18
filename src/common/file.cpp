
/*
Copyright Deighton Systems Limited (c) 2016
*/

#include "common/file.hpp"

#include <list>
#include <fstream>

#include "common/assert_verify.hpp"

namespace boost
{
namespace filesystem
{

boost::filesystem::path edsCannonicalise( const boost::filesystem::path& path )
{
    std::list< std::string > components, result;
    for( boost::filesystem::path::const_iterator
        i = path.begin(),
        iEnd = path.end(); i!=iEnd; ++i )
        components.push_back( (*i).generic_string() );

    for( std::list< std::string >::const_iterator
        i = components.begin(),
        iEnd = components.end(); i!=iEnd; ++i )
    {
        if( *i == "." )
            continue;
        else if( *i == ".." )
        {
            VERIFY_RTE_MSG( !result.empty(), "Invalid path: " << path );
            VERIFY_RTE_MSG( result.back().find( ':' ) == std::string::npos, "Invalid path: " << path );
            VERIFY_RTE_MSG( result.back().find( '/' ) == std::string::npos, "Invalid path: " << path );
            result.pop_back();
        }
        else
            result.push_back( *i );
    }

    boost::filesystem::path pResult;
    for( std::list< std::string >::const_iterator
        i = result.begin(),
        iEnd = result.end(); i!=iEnd; ++i )
        pResult /= *i;

    return pResult;
}

boost::filesystem::path edsInclude(
    const boost::filesystem::path& file, const boost::filesystem::path& include )
{
    boost::filesystem::path pResult;

    //VERIFY_RTE_MSG( file.is_absolute() && include.is_absolute(), "edsInclude passed relative paths: " <<
    //    file.native() << " : " << include.native() );

    //since the file paths are absolute they must contain the same root component up to the point they diverge
    boost::filesystem::path::const_iterator
        i = file.begin(), iEnd = file.end(),
        j = include.begin(), jEnd = include.end();
    //so iterate until they become different
    for( ; i!=iEnd && j!=jEnd && *i == *j; ++i, ++j ){}

    //then for the remaining parts of the file path append ../ to the result path
    for( ; i!=iEnd;  )
    {
        if( ++i == iEnd ) break;
        pResult /= "..";
    }

    //finally append the remaining part of the include path such that
    //now the result is a relative path from the file to the include path
    for( ; j!=jEnd; ++j )
        pResult /= *j;

    return pResult;
}

void loadAsciiFile( const boost::filesystem::path& filePath, std::string& strFileData, bool bAddCR /*= true*/ )
{
    std::ifstream inputFileStream( filePath.native().c_str(), std::ios::in );
    if( !inputFileStream.good() )
    {
        THROW_RTE( "Failed to open file: " << filePath.string() );
    }
    std::string fileContents( (std::istreambuf_iterator<char>( inputFileStream )),
                               std::istreambuf_iterator<char>() );
    if( bAddCR )
        fileContents.push_back( '\n' );//add carriage return onto end just in case...
    strFileData.swap( fileContents );
}

void loadAsciiFile( const boost::filesystem::path& filePath, std::ostream& osFileData, bool bAddCR /*= true*/ )
{
    std::ifstream inputFileStream( filePath.native().c_str(), std::ios::in );
    if( !inputFileStream.good() )
    {
        THROW_RTE( "Failed to open file: " << filePath.string() );
    }
    using StreamIter = std::istreambuf_iterator< char >;
    StreamIter iter( inputFileStream ), iterEnd;
    std::copy( iter, iterEnd, std::ostream_iterator< char >( osFileData, "" ) );
    if( bAddCR )
        osFileData << '\n';
}

void ensureFoldersExist( const boost::filesystem::path& filePath )
{
    //ensure the parent path exists
    boost::filesystem::path parentPath = filePath.parent_path();
    if( !parentPath.empty() && !exists( parentPath ) && !create_directories( parentPath ) )
    {
        THROW_RTE( "Failed to create directories for: " << filePath.string() );
    }
}

std::unique_ptr< boost::filesystem::ofstream > createNewFileStream( const boost::filesystem::path& filePath )
{
    ensureFoldersExist( filePath );
    std::unique_ptr< boost::filesystem::ofstream > pFileStream( 
        new boost::filesystem::ofstream( filePath, std::ios_base::trunc | std::ios_base::out ) );
    if( !pFileStream->good() )
    {
        THROW_RTE( "Failed to create file: " << filePath.string() );
    }

    return pFileStream;
}


std::unique_ptr< boost::filesystem::ofstream > createOrLoadNewFileStream( const boost::filesystem::path& filePath )
{
    ensureFoldersExist( filePath );
    std::unique_ptr< boost::filesystem::ofstream > pFileStream( 
        new boost::filesystem::ofstream( filePath, std::ios_base::out | std::ios_base::app ) );
    if( !pFileStream->good() )
    {
        THROW_RTE( "Failed to create file: " << filePath.string() );
    }

    return pFileStream;
}

std::unique_ptr< boost::filesystem::ofstream > createBinaryOutputFileStream( const boost::filesystem::path& filePath )
{
    boost::filesystem::ensureFoldersExist( filePath );
    std::unique_ptr< boost::filesystem::ofstream > pFileStream( 
        new boost::filesystem::ofstream( filePath, 
            std::ios_base::trunc | std::ios_base::out | std::ios_base::binary ) );
    if( !pFileStream->good() )
    {
        THROW_RTE( "Failed to create file: " << filePath.string() );
    }

    return pFileStream;
}

std::unique_ptr< boost::filesystem::ifstream > createBinaryInputFileStream( const boost::filesystem::path& filePath )
{
    std::unique_ptr< boost::filesystem::ifstream > pFileStream( 
        new boost::filesystem::ifstream( filePath, 
            std::ios_base::in | std::ios_base::binary ) );
    if( !pFileStream->good() )
    {
        THROW_RTE( "Failed to load file: " << filePath.string() );
    }

    return pFileStream;
}

}
}