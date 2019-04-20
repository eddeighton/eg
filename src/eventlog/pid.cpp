
#include "pid.hpp"

#pragma warning( push )
#pragma warning( disable : 4996) //iterator thing
#pragma warning( disable : 4244) //conversion to DWORD from system_clock::rep
#include "boost/process.hpp"
#pragma warning( pop )

#include <sstream>

namespace IPC
{
    PID::PID()
    {
        const int iActualPID = boost::this_process::get_id();
        std::ostringstream os;
        os << "process_" << iActualPID;
        *this = os.str();
    }
    
    PID::PID( const FixedBuffer& buffer )
    {
        for( char c : buffer )
        {
            if( c == '\0' )
                break;
            push_back( c );
        }
    }

    PID::FixedBuffer PID::toFixedBuffer() const
    {
        FixedBuffer buffer;
        std::copy( begin(), end(), buffer.begin() );
        std::fill( buffer.begin() + size(), buffer.end(), '\0' );
        return buffer;
    }

    MutexName mutexNameFromProcess( const PID& pid )
    {
        std::ostringstream os;
        os << pid;
        os << "_mutex";
        return MutexName( os.str() );
    }

    
}
