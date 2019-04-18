/*
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef PATHUTILS_01_05_2013
#define PATHUTILS_01_05_2013

#include <memory>
#include <ostream>
#include <string>

#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>

namespace boost
{
namespace filesystem
{

boost::filesystem::path edsCannonicalise( const boost::filesystem::path& path );

boost::filesystem::path edsInclude(
    const boost::filesystem::path& file, const boost::filesystem::path& include );

void loadAsciiFile( const boost::filesystem::path& filePath, std::string& strFileData, bool bAddCR = true );

void loadAsciiFile( const boost::filesystem::path& filePath, std::ostream& osFileData, bool bAddCR = true );

void ensureFoldersExist( const boost::filesystem::path& filePath );

std::unique_ptr< boost::filesystem::ofstream > createNewFileStream( const boost::filesystem::path& filePath );

std::unique_ptr< boost::filesystem::ofstream > createOrLoadNewFileStream( const boost::filesystem::path& filePath );

std::unique_ptr< boost::filesystem::ofstream > createBinaryOutputFileStream( const boost::filesystem::path& filePath );

std::unique_ptr< boost::filesystem::ifstream > createBinaryInputFileStream( const boost::filesystem::path& filePath );

}
}


#endif
