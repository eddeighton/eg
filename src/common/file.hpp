//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

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

void updateFileIfChanged( const boost::filesystem::path& filePath, const std::string& strContents );

bool compareFiles( const boost::filesystem::path& fileOne, const boost::filesystem::path& fileTwo );
}
}


#endif
