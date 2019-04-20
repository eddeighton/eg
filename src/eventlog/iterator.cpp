


#include "iterator.hpp"

#include "common/assert_verify.hpp"

namespace IPC
{
namespace Event
{

    std::string constructEventLogName( const PID& hostPID )
    {
        return IPC::MemoryRegion::constructMemoryRegionName( hostPID, "eventlog" );
    }
    
    Generator::Generator( const PID& hostPID, bool bCreate )
    {
        if( bCreate )
        {
            m_pIteratorMemory = std::make_shared< MemoryRegion >(
                constructEventLogName( hostPID ), sizeof( Iterators ), MemoryRegion::Create() );
                
            m_pIterators = new( m_pIteratorMemory->get() ) Iterators;
        }
        else
        {
            m_pIteratorMemory = std::make_shared< MemoryRegion >(
                constructEventLogName( hostPID ), sizeof( Iterators ), MemoryRegion::Open() );
            m_pIterators = reinterpret_cast< Iterators* >( m_pIteratorMemory->get() );
        }
    }
    
    Iterator Generator::head() const
    {
        return m_pIterators->m_head.load( std::memory_order_relaxed );
    }   
    
    bool Generator::updateHead()
    {
        Iterator claim = 
            m_pIterators->m_claim.load( std::memory_order_relaxed );
        Iterator oldHead =
            m_pIterators->m_head.exchange( claim );
        return claim != oldHead;
    }

    Iterator Generator::claim( std::size_t size )
    {
        //write a new event
        for( Iterator iCurrent = 
            m_pIterators->m_claim.load( std::memory_order_relaxed );; )
        {
            Iterator iBase = iCurrent;
            Iterator iDesired = iBase + size;
            
            //if would span files
            if( getFileID( iDesired ) != getFileID( iBase ) )
            {
                iBase = getIterFromFileID( getFileID( iDesired ) );
                iDesired = iBase + size;
                ASSERT( getFileID( iDesired ) != getFileID( iBase ) );
            }
            
            if( m_pIterators->m_claim.compare_exchange_weak( iCurrent, iDesired ) )
            {
                //memory is claimed
                return iBase;
            }
        }
    }
    
}//namespace Event
}//namespace IPC
