
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
