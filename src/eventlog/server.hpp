

#ifndef SERVER_LOG_05_09_2018
#define SERVER_LOG_05_09_2018

#include "log.hpp"

namespace IPC
{

    namespace Event
    {
        
        
        class Server : public Log
        {
        public:
        
            Server( const PID& hostPID, const boost::filesystem::path& filePath );
        
            inline Iterator head() const
            {
                return m_generator.head();
            }
                        
            inline bool updateHead()
            {
                return m_generator.updateHead();
            }
            
            void write( const Event& event );
            
        private:
            Generator m_generator;
        
        };
    
    }
}

#endif //SERVER_LOG_05_09_2018