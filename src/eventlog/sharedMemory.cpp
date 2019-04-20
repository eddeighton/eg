


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
