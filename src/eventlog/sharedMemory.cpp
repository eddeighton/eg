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




#include "sharedMemory.hpp"

#include <sstream>

namespace IPC
{

    
    MemoryRegion::MemoryRegion( const std::string& strName, size_t szBytes, Create create )
        : m_memory( boost::interprocess::create_only, 
                    strName.c_str(), 
                    boost::interprocess::read_write,
                    szBytes ),
          m_memoryMap(m_memory, boost::interprocess::read_write)
    {
        memset( get(), 0, szBytes );
    }
    
    MemoryRegion::MemoryRegion( const std::string& strName, size_t szBytes, Open open )
        : m_memory( boost::interprocess::open_only, 
                    strName.c_str(), 
                    boost::interprocess::read_write ),
          m_memoryMap(m_memory, boost::interprocess::read_write)
    {
    }

    std::string MemoryRegion::constructMemoryRegionName( 
        const std::string& strHostName, const std::string& strBufferName )
    {
        std::ostringstream os;
        os << strHostName << "/" << strBufferName;
        return os.str();
    }



}
