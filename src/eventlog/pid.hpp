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


#ifndef PID_15_06_2018
#define PID_15_06_2018

#include <string>
#include <array>

#include "common/assert_verify.hpp"

namespace IPC
{
    class PID : public std::string
    {
    public:
        static const size_t MAX_SIZE = 64;
        using FixedBuffer = std::array< char, MAX_SIZE >;
        
        PID();
        PID( const FixedBuffer& buffer );
        
        template< class T >
        PID( T value )
            :   std::string( value )
        {
            VERIFY_RTE_MSG( size() < MAX_SIZE, "Invalid PID length" );
        }
        
        template< class Archive >
        void serialize( Archive & ar, const unsigned int version )
        {
            ar & *static_cast< std::string* >( this );
        }
        
        FixedBuffer toFixedBuffer() const;
    };
    
    class MutexName : public std::string
    {
    public:
        static const size_t MAX_SIZE = PID::MAX_SIZE;
        
        template< class T >
        MutexName( T value )
            :   std::string( value )
        {
            VERIFY_RTE_MSG( size() < MAX_SIZE, "Invalid mutex name length" );
        }
    };
    
    MutexName mutexNameFromProcess( const PID& pid );
    
    
}

#endif //PID_15_06_2018
