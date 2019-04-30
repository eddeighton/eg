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
