

namespace
{

std::unique_ptr< eg::EventLogServer > g_eventLogServer( eg::EventLogServer::create( "log" ) );

}

eg::event_iterator events::getIterator()
{
    return g_eventLogServer->head();
}

bool events::get( eg::event_iterator& iterator, Event& event )
{
    const char* type;
    eg::TimeStamp timestamp;
    const void* value;
    std::size_t size;
    while( g_eventLogServer->read( iterator, type, timestamp, value, size ) )
    {
        if( 0U == strcmp( type, "stop" ) )
        {
            event.data = *reinterpret_cast< const eg::reference* >( value );
            return true;
        }
    }  
    return false;
}

void events::put( const char* type, eg::TimeStamp timestamp, const void* value, std::size_t size )
{
    g_eventLogServer->write( type, strlen( type ), timestamp, value, size );
}
    
bool events::update()
{
    return g_eventLogServer->updateHead();
}