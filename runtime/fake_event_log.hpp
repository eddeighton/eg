

#ifndef FAKE_EVENT_LOG
#define FAKE_EVENT_LOG


struct FakeEventLog : public eg::_event_log
{
    FakeEventLog()
    {
    }
    
    virtual eg::event_iterator GetEventIterator()
    {
        return eg::event_iterator();
    }
    
    virtual bool GetEvent( eg::event_iterator& iterator, eg::_event& event )
    {
        return false;
    }
    
    virtual void PutEvent( const eg::_event& event )
    {
    }
    
    bool updateAndWasEvent()
    {
        return false;
    }
    
};

#endif //FAKE_EVENT_LOG