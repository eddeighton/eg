


#ifndef FILE_05_SEPT_2018
#define FILE_05_SEPT_2018

#include <cstdint>

#include <boost/filesystem.hpp>

#include "sharedMemory.hpp"

#include "iterator.hpp"
#include "event.hpp"

namespace IPC
{
    namespace Event
    {
        
        std::string toLogFileName( LogFileIndex index );
        bool fromLogFileName( const std::string& strFileName, LogFileIndex& szIndex );

        class File
        {
        public:
            using Ptr = std::shared_ptr< File >;
            using PtrVector = std::vector< Ptr >;
        private:
            static void checkLogFileExists( 
                const boost::filesystem::path& filePath );
            static boost::filesystem::path constructLogFile( 
                const boost::filesystem::path& filePath );
            static boost::filesystem::path constructPath( 
                const boost::filesystem::path& dir, LogFileIndex index );
            
        public:
            File( const boost::filesystem::path& dir, LogFileIndex index );
            ~File();
            
            static bool isLogFile( const boost::filesystem::path& filePath, LogFileIndex& index );
            
            bool willFit( std::size_t position, const Event& event ) const;
            bool isEvent( std::size_t position ) const;
            void read( std::size_t position, Event& event ) const;
            void write( std::size_t position, const Event& event );
            void clear();
            
        private:
            boost::filesystem::path m_filePath;
            boost::interprocess::file_mapping m_fileMapping;
            boost::interprocess::mapped_region m_region;
        };
        
        
    }
}

#endif //FILE_05_SEPT_2018
