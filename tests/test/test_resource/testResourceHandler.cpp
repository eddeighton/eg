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

#include "testResourceHandler.hpp"

#include <fstream>
#include <string>
#include <memory>

void loadAsciiFile( const boost::filesystem::path& filePath, std::string& strFileData, bool bAddCR /*= true*/ )
{
    std::ifstream inputFileStream( filePath.native().c_str(), std::ios::in );
    std::string fileContents( (std::istreambuf_iterator<char>( inputFileStream )),
                               std::istreambuf_iterator<char>() );
    if( bAddCR )
        fileContents.push_back( '\n' );//add carriage return onto end just in case...
    strFileData.swap( fileContents );
}

eg::Resource::Ptr SimpleResourceHandler::reload( const eg::ResourcePath& path )
{
    std::shared_ptr< SimpleResource > pResource = std::make_shared< SimpleResource >();
    loadAsciiFile( path, pResource->m_str, false );
    return pResource;
}
