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
