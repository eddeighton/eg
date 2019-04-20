


#ifndef CLIENT_LOG_05_09_2018
#define CLIENT_LOG_05_09_2018

#include "log.hpp"

namespace IPC
{

    namespace Event
    {
        
        class Client : public Log
        {
        public:
            Client( const PID& hostPID, const boost::filesystem::path& filePath );
            
            inline Iterator head() const
            {
                return m_generator.head();
            }
            
        private:
            Generator m_generator;
        };
    
    }
}

#endif //CLIENT_LOG_05_09_2018