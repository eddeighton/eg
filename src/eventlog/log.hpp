



#ifndef EVENT_LOG_02_09_2018
#define EVENT_LOG_02_09_2018

#include <cstdint>
#include <atomic>
#include <memory>

#include <boost/filesystem.hpp>

#include "iterator.hpp"
#include "event.hpp"
#include "file.hpp"

namespace IPC
{

    namespace Event
    {    
        class Log
        {
        public:
            Log( const boost::filesystem::path& filePath );
        
            const boost::filesystem::path& getDir() const { return m_filePath; }
            
            bool read( Iterator& iterator, Event& event );
            
            void clear();
        
        protected:
            File::Ptr getFile( LogFileIndex szFileID, bool bAutoDump = true );
            
        protected:
            boost::filesystem::path m_filePath;
            File::PtrVector m_logFiles;
        };
    
    }
}

#endif //EVENT_LOG_02_09_2018