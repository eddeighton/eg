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


#ifndef EG_EVENT_LOG_IMPL_24_04_2019
#define EG_EVENT_LOG_IMPL_24_04_2019

#include "eventlog/eventlog_api.hpp"
    
struct HostEventLog : public eg::_event_log
{
    HostEventLog( const char* pszPID, const char* pszLogDir )
        :   m_pEventServer( ::eg::EventLogServer::create( pszPID, pszLogDir ) )
    {
    }
    ~HostEventLog()
    {
        delete m_pEventServer;
    }
    
    virtual eg::event_iterator GetEventIterator()
    {
        return m_pEventServer->head();
    }
    
    virtual bool GetEvent( eg::event_iterator& iterator, eg::_event& event )
    {
        return m_pEventServer->read( iterator, event.type, event.timestamp, event.value, event.size );
    }
    
    virtual void PutEvent( const eg::_event& event )
    {
        m_pEventServer->write( event.type, strlen( event.type ), event.timestamp, event.value, event.size );
    }
    
    bool updateAndWasEvent()
    {
        return m_pEventServer->updateHead();
    }
    
    eg::EventLogServer* m_pEventServer;
};

#endif //EG_EVENT_LOG_IMPL_24_04_2019