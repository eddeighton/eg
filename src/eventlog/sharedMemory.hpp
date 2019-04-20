

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