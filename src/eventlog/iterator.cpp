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
