#include "structures.hpp"
#include "pybind11/embed.h"
#include "pybind11/numpy.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"
#include "hosts/py_eg_reference.hpp"
#include "eg_runtime/eg_runtime.hpp"
#include "eventlog/eventlog_api.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>


struct HostClock
{
public:
    typedef std::chrono::steady_clock ClockType;
    typedef ClockType::time_point Tick;
    typedef ClockType::duration TickDuration;
    typedef std::chrono::duration< float, std::ratio< 1 > > FloatTickDuration;
    
    HostClock()
    {
        m_lastTick = m_startTick = ClockType::now();
        m_cycle = 1U;
        m_ct = m_dt = 0.0f;
    }
    
    inline Tick nextCycle()
    {
        const Tick nowTick = ClockType::now();
        m_dt = FloatTickDuration( nowTick - m_lastTick  ).count();
        m_ct = FloatTickDuration( nowTick - m_startTick ).count();
        m_lastTick = nowTick;
        ++m_cycle;
        return nowTick;
    }
    
    inline Tick actual()           const { return ClockType::now(); }
    inline eg::TimeStamp cycle()   const { return m_cycle; }
    inline float ct()              const { return m_ct; }
    inline float dt()              const { return m_dt; }
    
private:
    Tick m_lastTick, m_startTick;
    eg::TimeStamp m_cycle;
    float m_ct, m_dt;
} theClock;

eg::TimeStamp clock::cycle()
{
    return theClock.cycle();
}
float clock::ct()
{
    return theClock.ct();
}
float clock::dt()
{
    return theClock.dt();
}



std::unique_ptr< eg::EventLogServer > g_eventLogServer( eg::EventLogServer::create( "log" ) );

eg::event_iterator events::getIterator()
{
    return g_eventLogServer->head();
}

bool events::get( eg::event_iterator& iterator, Event& event )
{
    return false;//g_eventLogServer->read( iterator, event.type, event.timestamp, event.value, event.size );
}

void events::put( const char* type, eg::TimeStamp timestamp, const void* value, std::size_t size )
{
    g_eventLogServer->write( type, strlen( type ), timestamp, value, size );
}
    
bool updateEventLogAndWasEvent()
{
    return g_eventLogServer->updateHead();
}
    
    

std::deque< cinder::app::InputEvent > _cinder_events;
    
std::optional< cinder::app::InputEvent > Input::getEvent()
{
    std::optional< cinder::app::InputEvent > event;
    if( !_cinder_events.empty() )
    {
        event = _cinder_events.front();
        _cinder_events.pop_front();
    }
    return event;
}
    
    
//buffers
static std::array< b_root, 1 > g_root_array;
static std::array< b_root_Circle, 4096 > g_root_Circle_array;
static std::array< b_root_Circle_Shrink, 4096 > g_root_Circle_Shrink_array;
static std::array< b_root_Circle_Grow, 4096 > g_root_Circle_Grow_array;
static std::array< b_root_Circle_Move, 4096 > g_root_Circle_Move_array;
static std::array< b_root_Circle_ColourChanger, 4096 > g_root_Circle_ColourChanger_array;
b_root* g_root = g_root_array.data();
b_root_Circle* g_root_Circle = g_root_Circle_array.data();
b_root_Circle_Shrink* g_root_Circle_Shrink = g_root_Circle_Shrink_array.data();
b_root_Circle_Grow* g_root_Circle_Grow = g_root_Circle_Grow_array.data();
b_root_Circle_Move* g_root_Circle_Move = g_root_Circle_Move_array.data();
b_root_Circle_ColourChanger* g_root_Circle_ColourChanger = g_root_Circle_ColourChanger_array.data();

void allocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 75, 0 } );
        g_root[ i ].g_rootCircle_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
        g_root_Circle[ i ].g_root_Circle_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_root_Circle_state = ::eg::action_stopped;
        g_root_Circle[ i ].g_root_Circle_reference = __eg_root< void >::__eg_Circle< void >( eg::reference { i, 81, 0 } );
        g_root_Circle[ i ].g_root_Circle_alloc_data = i;
        g_root_Circle[ i ].g_root_CircleShrink_alloc_iter = 0UL;
        g_root_Circle[ i ].g_root_CircleGrow_alloc_iter = 0UL;
        g_root_Circle[ i ].g_root_CircleMove_alloc_iter = 0UL;
        g_root_Circle[ i ].g_root_CircleColourChanger_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
        g_root_Circle_Shrink[ i ].g_root_Circle_Shrink_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_Shrink[ i ].g_root_Circle_Shrink_state = ::eg::action_stopped;
        g_root_Circle_Shrink[ i ].g_root_Circle_Shrink_reference = __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >( eg::reference { i, 89, 0 } );
        g_root_Circle_Shrink[ i ].g_root_Circle_Shrink_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
        g_root_Circle_Grow[ i ].g_root_Circle_Grow_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_Grow[ i ].g_root_Circle_Grow_state = ::eg::action_stopped;
        g_root_Circle_Grow[ i ].g_root_Circle_Grow_reference = __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >( eg::reference { i, 94, 0 } );
        g_root_Circle_Grow[ i ].g_root_Circle_Grow_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
        g_root_Circle_Move[ i ].g_root_Circle_Move_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_Move[ i ].g_root_Circle_Move_state = ::eg::action_stopped;
        g_root_Circle_Move[ i ].g_root_Circle_Move_reference = __eg_root< void >::__eg_Circle< void >::__eg_Move< void >( eg::reference { i, 99, 0 } );
        g_root_Circle_Move[ i ].g_root_Circle_Move_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
        g_root_Circle_ColourChanger[ i ].g_root_Circle_ColourChanger_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_ColourChanger[ i ].g_root_Circle_ColourChanger_state = ::eg::action_stopped;
        g_root_Circle_ColourChanger[ i ].g_root_Circle_ColourChanger_reference = __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >( eg::reference { i, 104, 0 } );
        g_root_Circle_ColourChanger[ i ].g_root_Circle_ColourChanger_alloc_data = i;
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 4096; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
}

//Action functions
extern __eg_root< void > root_starter( std::vector< std::function< void() > >& );
extern void root_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Circle< void > root_Circle_starter( eg::Instance _gid );
extern void root_Circle_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > root_Circle_Shrink_starter( eg::Instance _gid );
extern void root_Circle_Shrink_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > root_Circle_Grow_starter( eg::Instance _gid );
extern void root_Circle_Grow_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Circle< void >::__eg_Move< void > root_Circle_Move_starter( eg::Instance _gid );
extern void root_Circle_Move_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > root_Circle_ColourChanger_starter( eg::Instance _gid );
extern void root_Circle_ColourChanger_stopper( eg::Instance _gid );



eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 75: return g_root[ instance ].g_root_reference.data.timestamp;
        case 81: return g_root_Circle[ instance ].g_root_Circle_reference.data.timestamp;
        case 89: return g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_reference.data.timestamp;
        case 94: return g_root_Circle_Grow[ instance ].g_root_Circle_Grow_reference.data.timestamp;
        case 99: return g_root_Circle_Move[ instance ].g_root_Circle_Move_reference.data.timestamp;
        case 104: return g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_reference.data.timestamp;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 75: return g_root[ instance ].g_root_state;
        case 81: return g_root_Circle[ instance ].g_root_Circle_state;
        case 89: return g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_state;
        case 94: return g_root_Circle_Grow[ instance ].g_root_Circle_Grow_state;
        case 99: return g_root_Circle_Move[ instance ].g_root_Circle_Move_state;
        case 104: return g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_state;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 75: return g_root[ instance ].g_root_fiber;
        case 81: return g_root_Circle[ instance ].g_root_Circle_fiber;
        case 89: return g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_fiber;
        case 94: return g_root_Circle_Grow[ instance ].g_root_Circle_Grow_fiber;
        case 99: return g_root_Circle_Move[ instance ].g_root_Circle_Move_fiber;
        case 104: return g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_fiber;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 75: return g_root[ instance ].g_root_cycle;
        case 81: return g_root_Circle[ instance ].g_root_Circle_cycle;
        case 89: return g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_cycle;
        case 94: return g_root_Circle_Grow[ instance ].g_root_Circle_Grow_cycle;
        case 99: return g_root_Circle_Move[ instance ].g_root_Circle_Move_cycle;
        case 104: return g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_cycle;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
__eg_root< void > get_root()
{
    return  __eg_root< void >( eg::reference{ 0, 75, getTimestamp( 75, 0 ) } );
}


//Python Interface Converters
std::shared_ptr< eg::PythonEGReferenceType > g_pEGRefType;
namespace pybind11
{
    namespace detail
    {
        template <> struct type_caster< __eg_root< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Circle< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Circle< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Circle< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Circle< void >::__eg_Grow< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Circle< void >::__eg_Move< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Circle< void >::__eg_Move< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Circle< void >::__eg_Move< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
    }   //namespace detail
} // namespace pybind11

//Python Interface
class std::optional<struct glm::tvec2<float, glm::highp> > g_root_m_lastPos_read( eg::Instance instance )
{
    return g_root[ instance ].m_lastPos;
}
void g_root_m_lastPos_write( eg::Instance instance, pybind11::tuple args )
{
    g_root[ instance ].m_lastPos = pybind11::cast< class std::optional<struct glm::tvec2<float, glm::highp> > >( args[ 0 ] );
}
struct glm::tvec2<float, glm::highp> g_root_Circle_m_position_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_position;
}
void g_root_Circle_m_position_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_position = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
}
class cinder::ColorT<float> g_root_Circle_m_color_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_color;
}
void g_root_Circle_m_color_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_color = pybind11::cast< class cinder::ColorT<float> >( args[ 0 ] );
}
float g_root_Circle_m_size_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_size;
}
void g_root_Circle_m_size_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_size = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_m_starttime_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_starttime;
}
void g_root_Circle_m_starttime_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_starttime = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_m_lifetime_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_lifetime;
}
void g_root_Circle_m_lifetime_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_lifetime = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_m_max_size_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_max_size;
}
void g_root_Circle_m_max_size_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_max_size = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_m_max_lifetime_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_max_lifetime;
}
void g_root_Circle_m_max_lifetime_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_max_lifetime = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_Shrink_m_start_read( eg::Instance instance )
{
    return g_root_Circle_Shrink[ instance ].m_start;
}
void g_root_Circle_Shrink_m_start_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle_Shrink[ instance ].m_start = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_Grow_m_start_read( eg::Instance instance )
{
    return g_root_Circle_Grow[ instance ].m_start;
}
void g_root_Circle_Grow_m_start_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle_Grow[ instance ].m_start = pybind11::cast< float >( args[ 0 ] );
}
struct glm::tvec2<float, glm::highp> g_root_Circle_Move_m_direction_read( eg::Instance instance )
{
    return g_root_Circle_Move[ instance ].m_direction;
}
void g_root_Circle_Move_m_direction_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle_Move[ instance ].m_direction = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
}
float g_root_Circle_Move_m_speed_read( eg::Instance instance )
{
    return g_root_Circle_Move[ instance ].m_speed;
}
void g_root_Circle_Move_m_speed_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle_Move[ instance ].m_speed = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_ColourChanger_m_offset_read( eg::Instance instance )
{
    return g_root_Circle_ColourChanger[ instance ].m_offset;
}
void g_root_Circle_ColourChanger_m_offset_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle_ColourChanger[ instance ].m_offset = pybind11::cast< float >( args[ 0 ] );
}
void root_stop( eg::Instance instance )
{
    root_stopper( instance );
}
void root_pause( eg::Instance instance )
{
    if( g_root[ instance ].g_root_state == ::eg::action_running )
        g_root[ instance ].g_root_state = ::eg::action_paused;
}
void root_resume( eg::Instance instance )
{
    if( g_root[ instance ].g_root_state == ::eg::action_paused )
        g_root[ instance ].g_root_state = ::eg::action_running;
}
bool root_done( eg::Instance instance )
{
    eg::Iterator iter;
    iter = eg::Iterator( g_root[ instance ].g_rootCircle_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Circle_stop( eg::Instance instance )
{
    root_Circle_stopper( instance );
}
void root_Circle_pause( eg::Instance instance )
{
    if( g_root_Circle[ instance ].g_root_Circle_state == ::eg::action_running )
        g_root_Circle[ instance ].g_root_Circle_state = ::eg::action_paused;
}
void root_Circle_resume( eg::Instance instance )
{
    if( g_root_Circle[ instance ].g_root_Circle_state == ::eg::action_paused )
        g_root_Circle[ instance ].g_root_Circle_state = ::eg::action_running;
}
bool root_Circle_done( eg::Instance instance )
{
    eg::Iterator iter;
    iter = eg::Iterator( g_root_Circle[ instance ].g_root_CircleShrink_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Circle[ instance ].g_root_CircleGrow_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Circle[ instance ].g_root_CircleMove_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Circle[ instance ].g_root_CircleColourChanger_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Circle_Shrink_stop( eg::Instance instance )
{
    root_Circle_Shrink_stopper( instance );
}
void root_Circle_Shrink_pause( eg::Instance instance )
{
    if( g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_state == ::eg::action_running )
        g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_state = ::eg::action_paused;
}
void root_Circle_Shrink_resume( eg::Instance instance )
{
    if( g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_state == ::eg::action_paused )
        g_root_Circle_Shrink[ instance ].g_root_Circle_Shrink_state = ::eg::action_running;
}
bool root_Circle_Shrink_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Circle_Grow_stop( eg::Instance instance )
{
    root_Circle_Grow_stopper( instance );
}
void root_Circle_Grow_pause( eg::Instance instance )
{
    if( g_root_Circle_Grow[ instance ].g_root_Circle_Grow_state == ::eg::action_running )
        g_root_Circle_Grow[ instance ].g_root_Circle_Grow_state = ::eg::action_paused;
}
void root_Circle_Grow_resume( eg::Instance instance )
{
    if( g_root_Circle_Grow[ instance ].g_root_Circle_Grow_state == ::eg::action_paused )
        g_root_Circle_Grow[ instance ].g_root_Circle_Grow_state = ::eg::action_running;
}
bool root_Circle_Grow_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Circle_Move_stop( eg::Instance instance )
{
    root_Circle_Move_stopper( instance );
}
void root_Circle_Move_pause( eg::Instance instance )
{
    if( g_root_Circle_Move[ instance ].g_root_Circle_Move_state == ::eg::action_running )
        g_root_Circle_Move[ instance ].g_root_Circle_Move_state = ::eg::action_paused;
}
void root_Circle_Move_resume( eg::Instance instance )
{
    if( g_root_Circle_Move[ instance ].g_root_Circle_Move_state == ::eg::action_paused )
        g_root_Circle_Move[ instance ].g_root_Circle_Move_state = ::eg::action_running;
}
bool root_Circle_Move_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Circle_ColourChanger_stop( eg::Instance instance )
{
    root_Circle_ColourChanger_stopper( instance );
}
void root_Circle_ColourChanger_pause( eg::Instance instance )
{
    if( g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_state == ::eg::action_running )
        g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_state = ::eg::action_paused;
}
void root_Circle_ColourChanger_resume( eg::Instance instance )
{
    if( g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_state == ::eg::action_paused )
        g_root_Circle_ColourChanger[ instance ].g_root_Circle_ColourChanger_state = ::eg::action_running;
}
bool root_Circle_ColourChanger_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}


void python_sleep_cycle()
{
    eg::sleep();
}
    

//Embedded Python Module
PYBIND11_EMBEDDED_MODULE( pyeg, module ) 
{
    module.def( "get_root", get_root );
    module.def( "sleep", python_sleep_cycle );
    module.def( "wait", eg::wait );
    module.def( "g_root_m_lastPos_read", g_root_m_lastPos_read );
    module.def( "g_root_m_lastPos_write", g_root_m_lastPos_write );
    module.def( "g_root_Circle_m_position_read", g_root_Circle_m_position_read );
    module.def( "g_root_Circle_m_position_write", g_root_Circle_m_position_write );
    module.def( "g_root_Circle_m_color_read", g_root_Circle_m_color_read );
    module.def( "g_root_Circle_m_color_write", g_root_Circle_m_color_write );
    module.def( "g_root_Circle_m_size_read", g_root_Circle_m_size_read );
    module.def( "g_root_Circle_m_size_write", g_root_Circle_m_size_write );
    module.def( "g_root_Circle_m_starttime_read", g_root_Circle_m_starttime_read );
    module.def( "g_root_Circle_m_starttime_write", g_root_Circle_m_starttime_write );
    module.def( "g_root_Circle_m_lifetime_read", g_root_Circle_m_lifetime_read );
    module.def( "g_root_Circle_m_lifetime_write", g_root_Circle_m_lifetime_write );
    module.def( "g_root_Circle_m_max_size_read", g_root_Circle_m_max_size_read );
    module.def( "g_root_Circle_m_max_size_write", g_root_Circle_m_max_size_write );
    module.def( "g_root_Circle_m_max_lifetime_read", g_root_Circle_m_max_lifetime_read );
    module.def( "g_root_Circle_m_max_lifetime_write", g_root_Circle_m_max_lifetime_write );
    module.def( "g_root_Circle_Shrink_m_start_read", g_root_Circle_Shrink_m_start_read );
    module.def( "g_root_Circle_Shrink_m_start_write", g_root_Circle_Shrink_m_start_write );
    module.def( "g_root_Circle_Grow_m_start_read", g_root_Circle_Grow_m_start_read );
    module.def( "g_root_Circle_Grow_m_start_write", g_root_Circle_Grow_m_start_write );
    module.def( "g_root_Circle_Move_m_direction_read", g_root_Circle_Move_m_direction_read );
    module.def( "g_root_Circle_Move_m_direction_write", g_root_Circle_Move_m_direction_write );
    module.def( "g_root_Circle_Move_m_speed_read", g_root_Circle_Move_m_speed_read );
    module.def( "g_root_Circle_Move_m_speed_write", g_root_Circle_Move_m_speed_write );
    module.def( "g_root_Circle_ColourChanger_m_offset_read", g_root_Circle_ColourChanger_m_offset_read );
    module.def( "g_root_Circle_ColourChanger_m_offset_write", g_root_Circle_ColourChanger_m_offset_write );
    module.def( "root_stop", root_stop);
    module.def( "root_pause", root_pause);
    module.def( "root_resume", root_resume);
    module.def( "root_done", root_done);
    module.def( "root_Circle_operator", &__eg_root< void >::__eg_Circle< void >::operator() );
    module.def( "root_Circle_stop", root_Circle_stop);
    module.def( "root_Circle_pause", root_Circle_pause);
    module.def( "root_Circle_resume", root_Circle_resume);
    module.def( "root_Circle_done", root_Circle_done);
    module.def( "root_Circle_Shrink_operator", &__eg_root< void >::__eg_Circle< void >::__eg_Shrink< void >::operator() );
    module.def( "root_Circle_Shrink_stop", root_Circle_Shrink_stop);
    module.def( "root_Circle_Shrink_pause", root_Circle_Shrink_pause);
    module.def( "root_Circle_Shrink_resume", root_Circle_Shrink_resume);
    module.def( "root_Circle_Shrink_done", root_Circle_Shrink_done);
    module.def( "root_Circle_Grow_operator", &__eg_root< void >::__eg_Circle< void >::__eg_Grow< void >::operator() );
    module.def( "root_Circle_Grow_stop", root_Circle_Grow_stop);
    module.def( "root_Circle_Grow_pause", root_Circle_Grow_pause);
    module.def( "root_Circle_Grow_resume", root_Circle_Grow_resume);
    module.def( "root_Circle_Grow_done", root_Circle_Grow_done);
    module.def( "root_Circle_Move_operator", &__eg_root< void >::__eg_Circle< void >::__eg_Move< void >::operator() );
    module.def( "root_Circle_Move_stop", root_Circle_Move_stop);
    module.def( "root_Circle_Move_pause", root_Circle_Move_pause);
    module.def( "root_Circle_Move_resume", root_Circle_Move_resume);
    module.def( "root_Circle_Move_done", root_Circle_Move_done);
    module.def( "root_Circle_ColourChanger_operator", &__eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void >::operator() );
    module.def( "root_Circle_ColourChanger_stop", root_Circle_ColourChanger_stop);
    module.def( "root_Circle_ColourChanger_pause", root_Circle_ColourChanger_pause);
    module.def( "root_Circle_ColourChanger_resume", root_Circle_ColourChanger_resume);
    module.def( "root_Circle_ColourChanger_done", root_Circle_ColourChanger_done);
}

struct HostFunctions : public eg::HostFunctionAccessor, public eg::HostEvaluator
{
private:
    struct Stack
    {
        PyObject *args;
        PyObject *kwargs;
        pybind11::object m_result;
        Stack( PyObject *args, PyObject *kwargs )
            :   args( args ), kwargs( kwargs )
        {
            Py_INCREF( args );
        }
        ~Stack()
        {
            Py_DECREF( args );
        }
        using WeakPtr = std::weak_ptr< Stack >;
        using SharedPtr = std::shared_ptr< Stack >;
    };
public:
    HostFunctions( const std::string& strDatabaseFile, pybind11::module module_eg )
        :   m_pRuntime( std::shared_ptr< eg::EGRuntime >( 
                eg::constructRuntime( *this, strDatabaseFile.c_str() ) ) ),
            m_module_eg( module_eg )
    {
    }
    //HostFunctionAccessor
    virtual eg::reference dereferenceDimension( const eg::reference& action, const eg::TypeID& dimensionType )
    {
        throw std::runtime_error( "Unreachable" );
    }
    virtual void doRead( const eg::reference& reference, eg::TypeID dimensionType )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( dimensionType )
            {
                case 86:
                    pStack->m_result = m_module_eg.attr( "g_root_m_lastPos_read" )( reference.instance );
                    break;
                case 109:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_read" )( reference.instance );
                    break;
                case 111:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_read" )( reference.instance );
                    break;
                case 113:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_read" )( reference.instance );
                    break;
                case 115:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_starttime_read" )( reference.instance );
                    break;
                case 117:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_lifetime_read" )( reference.instance );
                    break;
                case 119:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_size_read" )( reference.instance );
                    break;
                case 121:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_lifetime_read" )( reference.instance );
                    break;
                case 124:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Shrink_m_start_read" )( reference.instance );
                    break;
                case 127:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Grow_m_start_read" )( reference.instance );
                    break;
                case 130:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_direction_read" )( reference.instance );
                    break;
                case 132:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_speed_read" )( reference.instance );
                    break;
                case 135:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_ColourChanger_m_offset_read" )( reference.instance );
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doWrite( const eg::reference& reference, eg::TypeID dimensionType )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
            switch( dimensionType )
            {
                case 86:
                     pStack->m_result = m_module_eg.attr( "g_root_m_lastPos_write" )( reference.instance, args );
                     break;
                case 109:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_write" )( reference.instance, args );
                     break;
                case 111:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_write" )( reference.instance, args );
                     break;
                case 113:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_write" )( reference.instance, args );
                     break;
                case 115:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_starttime_write" )( reference.instance, args );
                     break;
                case 117:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_lifetime_write" )( reference.instance, args );
                     break;
                case 119:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_size_write" )( reference.instance, args );
                     break;
                case 121:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_lifetime_write" )( reference.instance, args );
                     break;
                case 124:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_Shrink_m_start_write" )( reference.instance, args );
                     break;
                case 127:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_Grow_m_start_write" )( reference.instance, args );
                     break;
                case 130:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_direction_write" )( reference.instance, args );
                     break;
                case 132:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_speed_write" )( reference.instance, args );
                     break;
                case 135:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_ColourChanger_m_offset_write" )( reference.instance, args );
                     break;
                default:
                    break;
            }
        }
    }
    virtual void doCall( const eg::reference& reference, eg::TypeID actionType )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
            switch( actionType )
            {
                case 81:
                    {
                        __eg_root< void >::__eg_Circle< void > ref = root_Circle_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 89:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > ref = root_Circle_Shrink_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_Shrink_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 94:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > ref = root_Circle_Grow_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_Grow_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_Grow_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 99:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_Move< void > ref = root_Circle_Move_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_Move_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_Move_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 104:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > ref = root_Circle_ColourChanger_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_ColourChanger_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doStart( const eg::reference& reference, eg::TypeID actionType )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( actionType )
            {
                case 81:
                    {
                        __eg_root< void >::__eg_Circle< void > ref = root_Circle_starter( reference.instance );
                        if( ref )
                        {
                            getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                            (
                                [ pStack, ref, m = m_module_eg ]()
                                {
                                    pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
                                    try
                                    {
                                        switch( args.size() )
                                        {
                                            case 0: m.attr( "root_Circle_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 89:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_Shrink< void > ref = root_Circle_Shrink_starter( reference.instance );
                        if( ref )
                        {
                            getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                            (
                                [ pStack, ref, m = m_module_eg ]()
                                {
                                    pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
                                    try
                                    {
                                        switch( args.size() )
                                        {
                                            case 0: m.attr( "root_Circle_Shrink_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_Shrink_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_Shrink_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 94:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_Grow< void > ref = root_Circle_Grow_starter( reference.instance );
                        if( ref )
                        {
                            getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                            (
                                [ pStack, ref, m = m_module_eg ]()
                                {
                                    pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
                                    try
                                    {
                                        switch( args.size() )
                                        {
                                            case 0: m.attr( "root_Circle_Grow_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_Grow_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_Grow_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 99:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_Move< void > ref = root_Circle_Move_starter( reference.instance );
                        if( ref )
                        {
                            getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                            (
                                [ pStack, ref, m = m_module_eg ]()
                                {
                                    pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
                                    try
                                    {
                                        switch( args.size() )
                                        {
                                            case 0: m.attr( "root_Circle_Move_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_Move_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_Move_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 104:
                    {
                        __eg_root< void >::__eg_Circle< void >::__eg_ColourChanger< void > ref = root_Circle_ColourChanger_starter( reference.instance );
                        if( ref )
                        {
                            getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                            (
                                [ pStack, ref, m = m_module_eg ]()
                                {
                                    pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
                                    try
                                    {
                                        switch( args.size() )
                                        {
                                            case 0: m.attr( "root_Circle_ColourChanger_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_ColourChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_ColourChanger_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doStop( const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( reference.type )
            {
                case 75:
                    pStack->m_result = m_module_eg.attr( "root_stop" )( reference.instance );
                    break;
                case 81:
                    pStack->m_result = m_module_eg.attr( "root_Circle_stop" )( reference.instance );
                    break;
                case 89:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Shrink_stop" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Grow_stop" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Move_stop" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColourChanger_stop" )( reference.instance );
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doPause( const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( reference.type )
            {
                case 75:
                    pStack->m_result = m_module_eg.attr( "root_pause" )( reference.instance );
                    break;
                case 81:
                    pStack->m_result = m_module_eg.attr( "root_Circle_pause" )( reference.instance );
                    break;
                case 89:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Shrink_pause" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Grow_pause" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Move_pause" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColourChanger_pause" )( reference.instance );
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doResume( const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( reference.type )
            {
                case 75:
                    pStack->m_result = m_module_eg.attr( "root_resume" )( reference.instance );
                    break;
                case 81:
                    pStack->m_result = m_module_eg.attr( "root_Circle_resume" )( reference.instance );
                    break;
                case 89:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Shrink_resume" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Grow_resume" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Move_resume" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColourChanger_resume" )( reference.instance );
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doDone( const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( reference.type )
            {
                case 75:
                    pStack->m_result = m_module_eg.attr( "root_done" )( reference.instance );
                    break;
                case 81:
                    pStack->m_result = m_module_eg.attr( "root_Circle_done" )( reference.instance );
                    break;
                case 89:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Shrink_done" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Grow_done" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Move_done" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColourChanger_done" )( reference.instance );
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doWaitAction( const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( reference ) );
        }
    }
    virtual void doWaitDimension( const eg::reference& reference, eg::TypeID dimensionType )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( dimensionType )
            {
                case 86:
                    pStack->m_result = m_module_eg.attr( "g_root_m_lastPos_read" )( reference.instance );
                    break;
                case 109:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_read" )( reference.instance );
                    break;
                case 111:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_read" )( reference.instance );
                    break;
                case 113:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_read" )( reference.instance );
                    break;
                case 115:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_starttime_read" )( reference.instance );
                    break;
                case 117:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_lifetime_read" )( reference.instance );
                    break;
                case 119:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_size_read" )( reference.instance );
                    break;
                case 121:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_lifetime_read" )( reference.instance );
                    break;
                case 124:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Shrink_m_start_read" )( reference.instance );
                    break;
                case 127:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Grow_m_start_read" )( reference.instance );
                    break;
                case 130:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_direction_read" )( reference.instance );
                    break;
                case 132:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_speed_read" )( reference.instance );
                    break;
                case 135:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_ColourChanger_m_offset_read" )( reference.instance );
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doGetAction( const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( reference ) );
        }
    }
    virtual void doGetDimension( const eg::reference& reference, eg::TypeID dimensionType )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( dimensionType )
            {
                case 86:
                    pStack->m_result = m_module_eg.attr( "g_root_m_lastPos_read" )( reference.instance );
                    break;
                case 109:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_read" )( reference.instance );
                    break;
                case 111:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_read" )( reference.instance );
                    break;
                case 113:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_read" )( reference.instance );
                    break;
                case 115:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_starttime_read" )( reference.instance );
                    break;
                case 117:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_lifetime_read" )( reference.instance );
                    break;
                case 119:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_size_read" )( reference.instance );
                    break;
                case 121:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_max_lifetime_read" )( reference.instance );
                    break;
                case 124:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Shrink_m_start_read" )( reference.instance );
                    break;
                case 127:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Grow_m_start_read" )( reference.instance );
                    break;
                case 130:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_direction_read" )( reference.instance );
                    break;
                case 132:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Move_m_speed_read" )( reference.instance );
                    break;
                case 135:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_ColourChanger_m_offset_read" )( reference.instance );
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doRange( eg::EGRangeDescriptionPtr pRange )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            pStack->m_result = pybind11::make_iterator( eg::PythonIterator( *g_pEGRefType, pRange, false ), eg::PythonIterator( *g_pEGRefType, pRange, true ) );
        }
    }
    //HostEvaluator
    virtual void getIdentities( std::vector< const char* >& identities )
    {
        m_pRuntime->getIdentities( identities );
    }
    virtual eg::TypeID getTypeID( const char* pszIdentity )
    {
        return m_pRuntime->getTypeID( pszIdentity );
    }
    virtual PyObject* invoke( const eg::reference& reference, const std::vector< eg::TypeID >& typePath, PyObject *args, PyObject *kwargs )
    {
        if( ( getState( reference.type, reference.instance ) != eg::action_stopped  ) || 
            ( getStopCycle( reference.type, reference.instance ) >= clock::cycle() ) )
        {
            Stack::SharedPtr pStack = std::make_shared< Stack >( args, kwargs );
            m_pStack = pStack;
            pybind11::args pyArgs = pybind11::reinterpret_borrow< pybind11::args >( args );
            m_pRuntime->invoke( reference, typePath, pyArgs.size() != 0 );
            if( pStack->m_result )
            {
                pybind11::handle h = pStack->m_result;
                h.inc_ref();
                return h.ptr();
            }
        }
        else
        {
            throw std::runtime_error( "Invalid reference used in invocation" );
        }
        Py_INCREF( Py_None );
        return Py_None;
    }
private:
    pybind11::module m_module_eg;
    Stack::WeakPtr m_pStack;
    std::shared_ptr< eg::EGRuntime > m_pRuntime;
};



void runPythonScript( const std::string& strPythonFile, const std::string& strDatabaseFile )
{
    try
    {
        std::string strScript;
        {
            const boost::filesystem::path pythonFilePath = 
                boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( strPythonFile ) );
            if( !boost::filesystem::exists( pythonFilePath ) )
            {
                std::cout << "Cannot locate file: " << pythonFilePath.string() << std::endl;
                return;
            } 
            boost::filesystem::loadAsciiFile( pythonFilePath, strScript );
        }

        if( !strScript.empty() )
        {
            pybind11::module pyeg_module = pybind11::module::import( "pyeg" );

            HostFunctions hostFunctions( strDatabaseFile, pyeg_module );
            
            if( !g_pEGRefType )
                g_pEGRefType = std::make_shared< eg::PythonEGReferenceType >( hostFunctions );

            pybind11::exec( strScript );
        }
    }
    catch( std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }
}

std::vector< std::function< void() > > loadPythonScripts( const std::vector< std::string >& scripts, const std::string& strDatabaseFile )
{
    std::vector< std::function< void() > > pythonFunctions;
    if( !scripts.empty() )
    {
        if( strDatabaseFile.empty() )
        {
            std::cout << "Missing database file path" << std::endl;
            return pythonFunctions;
        }
        const boost::filesystem::path databaseFilePath = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDatabaseFile ) );
        if( !boost::filesystem::exists( databaseFilePath ) )
        {
            std::cout << "Cannot locate file: " << databaseFilePath.string() << std::endl;
            return pythonFunctions;
        } 
            
        for( const std::string& strPythonScript : scripts )
        {
            pythonFunctions.push_back( std::bind( &runPythonScript, strPythonScript, strDatabaseFile ) );
        }
    }
    return pythonFunctions;
}



    
using namespace ci;
using namespace ci::app;

// We'll create a new Cinder Application by deriving from the App class.
class BasicApp : public App 
{
public:
    BasicApp();
    ~BasicApp();
    
    void update() override;
    
    // event handlers
    void mouseDown( MouseEvent event ) override;
    void mouseUp( MouseEvent event ) override;
    void mouseMove( MouseEvent event ) override;
    void mouseWheel( MouseEvent event ) override;
    void mouseDrag( MouseEvent event ) override;
    
    void keyDown( KeyEvent event ) override;
    void keyUp( KeyEvent event ) override;

    void setup();

	// Cinder will call 'draw' each time the contents of the window need to be redrawn.
	void draw() override;
    
    static std::vector< std::string > g_strPythonScripts;
    static std::string g_strDatabase;

private:
    HostClock::TickDuration sleepDuration = std::chrono::milliseconds( 10 );
    std::vector< std::function< void() > > pythonFunctions;
    pybind11::scoped_interpreter guard; // start the python interpreter
    
};

BasicApp::BasicApp()
{
    allocate_buffers();
}

BasicApp::~BasicApp()
{
    root_stopper( 0U );
    
    while( boost::this_fiber::properties< eg::fiber_props >().shouldContinue() )
    {
        theClock.nextCycle();
        eg::wait();
    }
    
    deallocate_buffers();
}

void BasicApp::setup()
{
    boost::fibers::use_scheduling_algorithm< eg::eg_algorithm >();
    
    ImGui::initialize();
        
    pythonFunctions = loadPythonScripts( g_strPythonScripts, g_strDatabase );
        
    root_starter( pythonFunctions );
    
    boost::this_fiber::properties< eg::fiber_props >().setTimeKeeper();
}

void prepareSettings( BasicApp::Settings* settings )
{
	settings->setMultiTouchEnabled( false );
}

void BasicApp::mouseDown( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseDown, event ) );
}
void BasicApp::mouseUp( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseUp, event ) );
}
void BasicApp::mouseMove( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseMove, event ) );
}
void BasicApp::mouseWheel( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseWheel, event ) );
}
void BasicApp::mouseDrag( MouseEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eMouseDrag, event ) );
}

void BasicApp::keyDown( KeyEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eKeyDown, event ) );
}
void BasicApp::keyUp( KeyEvent event )
{
    _cinder_events.push_back( cinder::app::InputEvent( cinder::app::InputEvent::eKeyUp, event ) );
}

void BasicApp::update()
{
    if( !boost::this_fiber::properties< eg::fiber_props >().shouldContinue() )
    {
        quit();
    }
}
    
void BasicApp::draw()
{
    theClock.nextCycle();
    eg::wait();
    _cinder_events.clear();
}

// This line tells Cinder to actually create and run the application.
CINDER_APP( BasicApp, RendererGl, prepareSettings )

std::vector< std::string > BasicApp::g_strPythonScripts;
std::string BasicApp::g_strDatabase;

int main( int argc, const char* argv[] )
{    
    {
        bool bDebug = false;
        namespace po = boost::program_options;
        po::variables_map vm;
        try
        {
            po::options_description desc("Allowed options");
            desc.add_options()
                ("help", "produce help message")
                
                //options
                ("debug",       po::value< bool >( &bDebug )->implicit_value( true ), 
                    "Wait at startup to allow attaching a debugger" )
                ("python",      po::value< std::vector< std::string > >( &BasicApp::g_strPythonScripts ), "Python file to run" )
                ("database",    po::value< std::string >( &BasicApp::g_strDatabase ), "Program Database" )
            ;

            po::positional_options_description p;
            p.add( "python", -1 );

            po::store( po::command_line_parser( argc, argv).
                        options( desc ).
                        positional( p ).run(),
                        vm );
            po::notify(vm);

            if (vm.count("help"))
            {
                std::cout << desc << "\n";
                return 1;
            }
        }
        catch( std::exception& )
        {
            std::cout << "Invalid input. Type '--help' for options\n";
            return 1;
        }
        //wait for input 
        if( bDebug )
        {
#ifdef _DEBUG
            Common::debug_break();
#else
            char c;
            std::cin >> c;
#endif 
        }
    }
    
    return WinMain( nullptr, nullptr, nullptr, 0 );
}

    