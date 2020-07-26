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


#include "eventlog/eventlog_api.hpp"

#include "eventlog/server.hpp"
#include "eventlog/client.hpp"

namespace eg
{

    EventLogServer::~EventLogServer()
    {
        
    }
    
    EventLogClient::~EventLogClient()
    {
        
    }

    class EventLogServerImpl : public EventLogServer
    {
    public:
        EventLogServerImpl( const IPC::PID& hostPID, const boost::filesystem::path& filePath )
            :   m_server( hostPID, filePath )
        {
        }
    
        std::uint64_t head()
        {
            return m_server.head();
        }
        
        bool updateHead()
        {
            return m_server.updateHead();
        }
        
        void write( const char* pszType, std::size_t szTypeSize, std::uint32_t timestamp, 
                const void* pValue, std::size_t szValueSize )
        {
            m_server.write( IPC::Event::Event( pszType, szTypeSize, timestamp, pValue, szValueSize ) );
        }
        
        bool read( std::uint64_t& iterator, const char*& pszType, std::uint32_t& timestamp, 
                const void*& pValue, std::size_t& szValueSize )
        {
            IPC::Event::Event ev;
            if( m_server.read( iterator, ev ) )
            {
                pszType     = ev.getType_c_str();
                timestamp   = ev.getTimeStamp();
                pValue      = ev.getValue();
                szValueSize = ev.getValueSize();
                return true;
            }
            else
            {
                return false;
            }
        }
        
        IPC::Event::Server m_server;
    };
    
    class EventLogClientImpl : public EventLogClient
    {
    public:
        EventLogClientImpl( const IPC::PID& hostPID, const boost::filesystem::path& filePath )
            :   m_client( hostPID, filePath )
        {
            
        }
        
        std::uint64_t head()
        {
            return m_client.head();
        }
        
        bool read( std::uint64_t& iterator, const char*& pszType, std::uint32_t& timestamp, 
                const void*& pValue, std::size_t& szValueSize )
        {
            IPC::Event::Event ev;
            if( m_client.read( iterator, ev ) )
            {
                pszType     = ev.getType_c_str();
                timestamp   = ev.getTimeStamp();
                pValue      = ev.getValue();
                szValueSize = ev.getValueSize();
                return true;
            }
            else
            {
                return false;
            }
        }
        
        IPC::Event::Client m_client;
    };
    
    EventLogServer* EventLogServer::create( const char* pszPID, const char* pszFilePath )
    {
        IPC::PID pid( pszPID );
        boost::filesystem::path logPath( pszFilePath );
        return new EventLogServerImpl( pid, logPath );
    }

    EventLogServer* EventLogServer::create( const char* pszFilePath )
    {
        IPC::PID pid;
        boost::filesystem::path logPath( pszFilePath );
        return new EventLogServerImpl( pid, logPath );
    }

    EventLogClient* EventLogClient::create( const char* pszPID, const char* pszFilePath )
    {
        IPC::PID pid( pszPID );
        boost::filesystem::path logPath( pszFilePath );
        return new EventLogClientImpl( pid, logPath );
    }
}