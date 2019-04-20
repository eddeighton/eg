


#ifndef ITERAOR_05_SEPT_2018
#define ITERAOR_05_SEPT_2018

#include <cstdint>
#include <atomic>
#include <memory>
#include <limits>

#include "pid.hpp"
#include "sharedMemory.hpp"

namespace IPC
{
    namespace Event
    {
        using Iterator = std::uint64_t;
        using LogFileIndex = std::uint32_t;
        
        static const int LogFileIndexDigits = 
            std::numeric_limits< LogFileIndex >::digits10 + 1;
        
        static const Iterator LOG_FILE_SIZE = 1 << 26;
        
        static inline LogFileIndex getFileID( Iterator iterator ) 
        {
            const Iterator logFileIndex = iterator / LOG_FILE_SIZE;
            if( logFileIndex > std::numeric_limits< LogFileIndex >::max() )
                throw std::out_of_range( "out of range" );
            return static_cast< LogFileIndex >( logFileIndex );
        }
        static inline std::size_t getPosition( Iterator iterator ) 
        {
            return static_cast< std::size_t >( iterator % LOG_FILE_SIZE );
        }
        static inline Iterator getIterFromFileID( LogFileIndex szFileID )
        {
            return static_cast< Iterator >( szFileID ) * LOG_FILE_SIZE;
        }
        
        
        class Generator
        {
            struct Iterators
            {
                std::atomic< Iterator > m_claim;
                std::atomic< Iterator > m_head;
                
                Iterators()
                    :   m_claim( 0UL ),
                        m_head( 0UL )
                {
                    VERIFY_RTE( m_claim == 0UL );
                    VERIFY_RTE( m_head == 0UL );
                }
            };
        public:
            Generator( const PID& hostPID, bool bCreate );
                        
            Iterator head() const;
            bool updateHead();
            Iterator claim( std::size_t size );
        private:
            IPC::MemoryRegion::Ptr m_pIteratorMemory;
            Iterators* m_pIterators;
        };
        
    }
}

#endif //ITERAOR_05_SEPT_2018
