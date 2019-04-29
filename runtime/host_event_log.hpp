
#ifndef EG_EVENT_LOG_IMPL_24_04_2019
#define EG_EVENT_LOG_IMPL_24_04_2019

struct HostEventLog : public eg::_event_log
{
    HostEventLog( const IPC::PID& hostPID, const boost::filesystem::path& filePath )
        :   m_eventLog( hostPID, filePath )
    {
    }
    
    
    virtual eg::event_iterator GetEventIterator()
    {
        return m_eventLog.head();
    }
    
    virtual bool GetEvent( eg::event_iterator& iterator, eg::_event& event )
    {
        IPC::Event::Event readEvent;
        if( m_eventLog.read( iterator, readEvent ) )
        {
            event.timestamp     = readEvent.getTimeStamp();
            event.type          = readEvent.getType_c_str();
            event.value         = readEvent.getValue();
            event.size          = readEvent.getValueSize();
            return true;
        }
        else
        {
            return false;
        }
    }
    
    virtual void PutEvent( const eg::_event& event )
    {
        m_eventLog.write( IPC::Event::Event( event.type, 
            strlen( event.type ), event.timestamp, event.value, event.size ) );
    }
    
    bool updateAndWasEvent()
    {
        return m_eventLog.updateHead();
    }
    
    IPC::Event::Server m_eventLog;
};

#endif //EG_EVENT_LOG_IMPL_24_04_2019