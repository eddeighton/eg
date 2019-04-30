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



#ifndef MEMORY_REGION_02_SEPT_2018
#define MEMORY_REGION_02_SEPT_2018

#include <string>

#include <boost/interprocess/sync/named_recursive_mutex.hpp>
#include <boost/interprocess/windows_shared_memory.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

namespace IPC
{
    class MemoryRegion
    {
      public:
        using Ptr = std::shared_ptr< MemoryRegion >;
        using PtrVector = std::vector<Ptr>;

        struct Create{};
        struct Open{};
        MemoryRegion( const std::string& strName, size_t szBytes, Create create);
        MemoryRegion( const std::string& strName, size_t szBytes, Open open );
        
        static std::string constructMemoryRegionName( 
            const std::string& strHostName, const std::string& strBufferName );
        
        void* get() const { return m_memoryMap.get_address(); }
    private:
        
        boost::interprocess::windows_shared_memory m_memory;
        boost::interprocess::mapped_region m_memoryMap;
    };

}

#endif //MEMORY_REGION_02_SEPT_2018