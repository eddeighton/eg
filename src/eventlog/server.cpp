

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
