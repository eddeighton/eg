


#include "client.hpp"

namespace IPC
{
namespace Event
{
    
    Client::Client( const PID& hostPID, const boost::filesystem::path& filePath )
        :   Log( filePath ),
            m_generator( hostPID, false )
    {
    }

}//namespace Event
}//namespace IPC
