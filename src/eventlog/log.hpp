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