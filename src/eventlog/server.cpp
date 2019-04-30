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



#include "server.hpp"


namespace IPC
{
namespace Event
{
    
    Server::Server( const PID& hostPID, const boost::filesystem::path& filePath )
        :   Log( filePath ),
            m_generator( hostPID, true )
    {
        clear();
    }
    

    void Server::write( const Event& event )
    {
        VERIFY_RTE_MSG( event.totalSize() <= LOG_FILE_SIZE, "Event too big" );
        const Iterator iterClaim = m_generator.claim( event.totalSize() );
        File::Ptr pLogFile = getFile( getFileID( iterClaim ) );
        const std::size_t szPosition = getPosition( iterClaim );
        pLogFile->write( szPosition, event );
    }


}//namespace Event
}//namespace IPC
