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
static std::array< b_root_StrokedCircle, 256 > g_root_StrokedCircle_array;
static std::array< b_root_StrokedCircle_Default, 256 > g_root_StrokedCircle_Default_array;
static std::array< b_root_StrokedCircle_ColorChanger, 256 > g_root_StrokedCircle_ColorChanger_array;
static std::array< b_root_StrokedCircle_Morph, 256 > g_root_StrokedCircle_Morph_array;
static std::array< b_root_Circle, 256 > g_root_Circle_array;
static std::array< b_root_Circle_Default, 256 > g_root_Circle_Default_array;
static std::array< b_root_Circle_ColorChanger, 256 > g_root_Circle_ColorChanger_array;
static std::array< b_root_Circle_Morph, 256 > g_root_Circle_Morph_array;
static std::array< b_root_Spiral, 1 > g_root_Spiral_array;
static std::array< b_root_Attract, 1 > g_root_Attract_array;
static std::array< b_root_Randomise, 1 > g_root_Randomise_array;
static std::array< b_root_imgui, 1 > g_root_imgui_array;
b_root* g_root = g_root_array.data();
b_root_StrokedCircle* g_root_StrokedCircle = g_root_StrokedCircle_array.data();
b_root_StrokedCircle_Default* g_root_StrokedCircle_Default = g_root_StrokedCircle_Default_array.data();
b_root_StrokedCircle_ColorChanger* g_root_StrokedCircle_ColorChanger = g_root_StrokedCircle_ColorChanger_array.data();
b_root_StrokedCircle_Morph* g_root_StrokedCircle_Morph = g_root_StrokedCircle_Morph_array.data();
b_root_Circle* g_root_Circle = g_root_Circle_array.data();
b_root_Circle_Default* g_root_Circle_Default = g_root_Circle_Default_array.data();
b_root_Circle_ColorChanger* g_root_Circle_ColorChanger = g_root_Circle_ColorChanger_array.data();
b_root_Circle_Morph* g_root_Circle_Morph = g_root_Circle_Morph_array.data();
b_root_Spiral* g_root_Spiral = g_root_Spiral_array.data();
b_root_Attract* g_root_Attract = g_root_Attract_array.data();
b_root_Randomise* g_root_Randomise = g_root_Randomise_array.data();
b_root_imgui* g_root_imgui = g_root_imgui_array.data();

void allocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 88, 0 } );
        g_root[ i ].g_rootStrokedCircle_alloc_iter = 0UL;
        g_root[ i ].g_rootCircle_alloc_iter = 0UL;
        g_root[ i ].g_rootSpiral_alloc_iter = 0UL;
        g_root[ i ].g_rootAttract_alloc_iter = 0UL;
        g_root[ i ].g_rootRandomise_alloc_iter = 0UL;
        g_root[ i ].g_rootimgui_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_state = ::eg::action_stopped;
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_reference = __eg_root< void >::__eg_StrokedCircle< void >( eg::reference { i, 94, 0 } );
        g_root_StrokedCircle[ i ].g_root_StrokedCircle_alloc_data = i;
        g_root_StrokedCircle[ i ].g_root_StrokedCircleDefault_alloc_iter = 0UL;
        g_root_StrokedCircle[ i ].g_root_StrokedCircleColorChanger_alloc_iter = 0UL;
        g_root_StrokedCircle[ i ].g_root_StrokedCircleMorph_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_state = ::eg::action_stopped;
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_reference = __eg_root< void >::__eg_Shape< void >::__eg_Default< void >( eg::reference { i, 126, 0 } );
        g_root_StrokedCircle_Default[ i ].g_root_StrokedCircle_Default_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_state = ::eg::action_stopped;
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_reference = __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >( eg::reference { i, 131, 0 } );
        g_root_StrokedCircle_ColorChanger[ i ].g_root_StrokedCircle_ColorChanger_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_cycle = eg::INVALID_TIMESTAMP;
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_state = ::eg::action_stopped;
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_reference = __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >( eg::reference { i, 136, 0 } );
        g_root_StrokedCircle_Morph[ i ].g_root_StrokedCircle_Morph_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle[ i ].g_root_Circle_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle[ i ].g_root_Circle_state = ::eg::action_stopped;
        g_root_Circle[ i ].g_root_Circle_reference = __eg_root< void >::__eg_Circle< void >( eg::reference { i, 99, 0 } );
        g_root_Circle[ i ].g_root_Circle_alloc_data = i;
        g_root_Circle[ i ].g_root_CircleDefault_alloc_iter = 0UL;
        g_root_Circle[ i ].g_root_CircleColorChanger_alloc_iter = 0UL;
        g_root_Circle[ i ].g_root_CircleMorph_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle_Default[ i ].g_root_Circle_Default_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_Default[ i ].g_root_Circle_Default_state = ::eg::action_stopped;
        g_root_Circle_Default[ i ].g_root_Circle_Default_reference = __eg_root< void >::__eg_Shape< void >::__eg_Default< void >( eg::reference { i, 158, 0 } );
        g_root_Circle_Default[ i ].g_root_Circle_Default_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_state = ::eg::action_stopped;
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_reference = __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >( eg::reference { i, 163, 0 } );
        g_root_Circle_ColorChanger[ i ].g_root_Circle_ColorChanger_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_cycle = eg::INVALID_TIMESTAMP;
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_state = ::eg::action_stopped;
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_reference = __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >( eg::reference { i, 168, 0 } );
        g_root_Circle_Morph[ i ].g_root_Circle_Morph_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Spiral[ i ].g_root_Spiral_cycle = eg::INVALID_TIMESTAMP;
        g_root_Spiral[ i ].g_root_Spiral_state = ::eg::action_stopped;
        g_root_Spiral[ i ].g_root_Spiral_reference = __eg_root< void >::__eg_Spiral< void >( eg::reference { i, 104, 0 } );
        g_root_Spiral[ i ].g_root_Spiral_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Attract[ i ].g_root_Attract_cycle = eg::INVALID_TIMESTAMP;
        g_root_Attract[ i ].g_root_Attract_state = ::eg::action_stopped;
        g_root_Attract[ i ].g_root_Attract_reference = __eg_root< void >::__eg_Attract< void >( eg::reference { i, 109, 0 } );
        g_root_Attract[ i ].g_root_Attract_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Randomise[ i ].g_root_Randomise_cycle = eg::INVALID_TIMESTAMP;
        g_root_Randomise[ i ].g_root_Randomise_state = ::eg::action_stopped;
        g_root_Randomise[ i ].g_root_Randomise_reference = __eg_root< void >::__eg_Randomise< void >( eg::reference { i, 114, 0 } );
        g_root_Randomise[ i ].g_root_Randomise_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_imgui[ i ].g_root_imgui_cycle = eg::INVALID_TIMESTAMP;
        g_root_imgui[ i ].g_root_imgui_state = ::eg::action_stopped;
        g_root_imgui[ i ].g_root_imgui_reference = __eg_root< void >::__eg_imgui< void >( eg::reference { i, 119, 0 } );
        g_root_imgui[ i ].g_root_imgui_alloc_data = i;
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 256; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
}

//Action functions
extern __eg_root< void > root_starter( std::vector< std::function< void() > >& );
extern void root_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_StrokedCircle< void > root_StrokedCircle_starter( eg::Instance _gid );
extern void root_StrokedCircle_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Circle< void > root_Circle_starter( eg::Instance _gid );
extern void root_Circle_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Spiral< void > root_Spiral_starter( eg::Instance _gid );
extern void root_Spiral_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Attract< void > root_Attract_starter( eg::Instance _gid );
extern void root_Attract_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Randomise< void > root_Randomise_starter( eg::Instance _gid );
extern void root_Randomise_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_imgui< void > root_imgui_starter( eg::Instance _gid );
extern void root_imgui_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Shape< void >::__eg_Default< void > root_StrokedCircle_Default_starter( eg::Instance _gid );
extern void root_StrokedCircle_Default_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > root_StrokedCircle_ColorChanger_starter( eg::Instance _gid );
extern void root_StrokedCircle_ColorChanger_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > root_StrokedCircle_Morph_starter( eg::Instance _gid );
extern void root_StrokedCircle_Morph_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Shape< void >::__eg_Default< void > root_Circle_Default_starter( eg::Instance _gid );
extern void root_Circle_Default_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > root_Circle_ColorChanger_starter( eg::Instance _gid );
extern void root_Circle_ColorChanger_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > root_Circle_Morph_starter( eg::Instance _gid );
extern void root_Circle_Morph_stopper( eg::Instance _gid );



eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 88: return g_root[ instance ].g_root_reference.data.timestamp;
        case 94: return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_reference.data.timestamp;
        case 99: return g_root_Circle[ instance ].g_root_Circle_reference.data.timestamp;
        case 104: return g_root_Spiral[ instance ].g_root_Spiral_reference.data.timestamp;
        case 109: return g_root_Attract[ instance ].g_root_Attract_reference.data.timestamp;
        case 114: return g_root_Randomise[ instance ].g_root_Randomise_reference.data.timestamp;
        case 119: return g_root_imgui[ instance ].g_root_imgui_reference.data.timestamp;
        case 126: return g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_reference.data.timestamp;
        case 131: return g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_reference.data.timestamp;
        case 136: return g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_reference.data.timestamp;
        case 158: return g_root_Circle_Default[ instance ].g_root_Circle_Default_reference.data.timestamp;
        case 163: return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_reference.data.timestamp;
        case 168: return g_root_Circle_Morph[ instance ].g_root_Circle_Morph_reference.data.timestamp;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 88: return g_root[ instance ].g_root_state;
        case 94: return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_state;
        case 99: return g_root_Circle[ instance ].g_root_Circle_state;
        case 104: return g_root_Spiral[ instance ].g_root_Spiral_state;
        case 109: return g_root_Attract[ instance ].g_root_Attract_state;
        case 114: return g_root_Randomise[ instance ].g_root_Randomise_state;
        case 119: return g_root_imgui[ instance ].g_root_imgui_state;
        case 126: return g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_state;
        case 131: return g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_state;
        case 136: return g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_state;
        case 158: return g_root_Circle_Default[ instance ].g_root_Circle_Default_state;
        case 163: return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_state;
        case 168: return g_root_Circle_Morph[ instance ].g_root_Circle_Morph_state;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 88: return g_root[ instance ].g_root_fiber;
        case 94: return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_fiber;
        case 99: return g_root_Circle[ instance ].g_root_Circle_fiber;
        case 104: return g_root_Spiral[ instance ].g_root_Spiral_fiber;
        case 109: return g_root_Attract[ instance ].g_root_Attract_fiber;
        case 114: return g_root_Randomise[ instance ].g_root_Randomise_fiber;
        case 119: return g_root_imgui[ instance ].g_root_imgui_fiber;
        case 126: return g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_fiber;
        case 131: return g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_fiber;
        case 136: return g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_fiber;
        case 158: return g_root_Circle_Default[ instance ].g_root_Circle_Default_fiber;
        case 163: return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_fiber;
        case 168: return g_root_Circle_Morph[ instance ].g_root_Circle_Morph_fiber;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 88: return g_root[ instance ].g_root_cycle;
        case 94: return g_root_StrokedCircle[ instance ].g_root_StrokedCircle_cycle;
        case 99: return g_root_Circle[ instance ].g_root_Circle_cycle;
        case 104: return g_root_Spiral[ instance ].g_root_Spiral_cycle;
        case 109: return g_root_Attract[ instance ].g_root_Attract_cycle;
        case 114: return g_root_Randomise[ instance ].g_root_Randomise_cycle;
        case 119: return g_root_imgui[ instance ].g_root_imgui_cycle;
        case 126: return g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_cycle;
        case 131: return g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_cycle;
        case 136: return g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_cycle;
        case 158: return g_root_Circle_Default[ instance ].g_root_Circle_Default_cycle;
        case 163: return g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_cycle;
        case 168: return g_root_Circle_Morph[ instance ].g_root_Circle_Morph_cycle;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
__eg_root< void > get_root()
{
    return  __eg_root< void >( eg::reference{ 0, 88, getTimestamp( 88, 0 ) } );
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
        template <> struct type_caster< __eg_root< void >::__eg_Shape< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Shape< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Shape< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Shape< void >::__eg_Default< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Shape< void >::__eg_Default< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Shape< void >::__eg_Default< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Shape< void >::__eg_Morph< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_StrokedCircle< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_StrokedCircle< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_StrokedCircle< void > src, return_value_policy /* policy */, handle /* parent */)
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
        template <> struct type_caster< __eg_root< void >::__eg_Spiral< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Spiral< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Spiral< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Attract< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Attract< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Attract< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Randomise< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Randomise< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Randomise< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_imgui< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_imgui< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_imgui< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
    }   //namespace detail
} // namespace pybind11

//Python Interface
struct glm::tvec2<float, glm::highp> g_root_StrokedCircle_m_position_read( eg::Instance instance )
{
    return g_root_StrokedCircle[ instance ].m_position;
}
void g_root_StrokedCircle_m_position_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_StrokedCircle[ instance ].m_position = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
}
struct glm::tvec2<float, glm::highp> g_root_StrokedCircle_m_positionActual_read( eg::Instance instance )
{
    return g_root_StrokedCircle[ instance ].m_positionActual;
}
void g_root_StrokedCircle_m_positionActual_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_StrokedCircle[ instance ].m_positionActual = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
}
class cinder::ColorT<float> g_root_StrokedCircle_m_color_read( eg::Instance instance )
{
    return g_root_StrokedCircle[ instance ].m_color;
}
void g_root_StrokedCircle_m_color_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_StrokedCircle[ instance ].m_color = pybind11::cast< class cinder::ColorT<float> >( args[ 0 ] );
}
float g_root_StrokedCircle_m_size_read( eg::Instance instance )
{
    return g_root_StrokedCircle[ instance ].m_size;
}
void g_root_StrokedCircle_m_size_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_StrokedCircle[ instance ].m_size = pybind11::cast< float >( args[ 0 ] );
}
float g_root_StrokedCircle_ColorChanger_m_f_read( eg::Instance instance )
{
    return g_root_StrokedCircle_ColorChanger[ instance ].m_f;
}
void g_root_StrokedCircle_ColorChanger_m_f_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_StrokedCircle_ColorChanger[ instance ].m_f = pybind11::cast< float >( args[ 0 ] );
}
float g_root_StrokedCircle_Morph_m_speed_read( eg::Instance instance )
{
    return g_root_StrokedCircle_Morph[ instance ].m_speed;
}
void g_root_StrokedCircle_Morph_m_speed_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_StrokedCircle_Morph[ instance ].m_speed = pybind11::cast< float >( args[ 0 ] );
}
struct glm::tvec2<float, glm::highp> g_root_Circle_m_position_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_position;
}
void g_root_Circle_m_position_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_position = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
}
struct glm::tvec2<float, glm::highp> g_root_Circle_m_positionActual_read( eg::Instance instance )
{
    return g_root_Circle[ instance ].m_positionActual;
}
void g_root_Circle_m_positionActual_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle[ instance ].m_positionActual = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
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
float g_root_Circle_ColorChanger_m_f_read( eg::Instance instance )
{
    return g_root_Circle_ColorChanger[ instance ].m_f;
}
void g_root_Circle_ColorChanger_m_f_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle_ColorChanger[ instance ].m_f = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Circle_Morph_m_speed_read( eg::Instance instance )
{
    return g_root_Circle_Morph[ instance ].m_speed;
}
void g_root_Circle_Morph_m_speed_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Circle_Morph[ instance ].m_speed = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Spiral_m_curve_read( eg::Instance instance )
{
    return g_root_Spiral[ instance ].m_curve;
}
void g_root_Spiral_m_curve_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Spiral[ instance ].m_curve = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Spiral_m_radius_read( eg::Instance instance )
{
    return g_root_Spiral[ instance ].m_radius;
}
void g_root_Spiral_m_radius_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Spiral[ instance ].m_radius = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Spiral_m_relative_read( eg::Instance instance )
{
    return g_root_Spiral[ instance ].m_relative;
}
void g_root_Spiral_m_relative_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Spiral[ instance ].m_relative = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Spiral_m_speed_read( eg::Instance instance )
{
    return g_root_Spiral[ instance ].m_speed;
}
void g_root_Spiral_m_speed_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Spiral[ instance ].m_speed = pybind11::cast< float >( args[ 0 ] );
}
int g_root_Randomise_m_range_read( eg::Instance instance )
{
    return g_root_Randomise[ instance ].m_range;
}
void g_root_Randomise_m_range_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Randomise[ instance ].m_range = pybind11::cast< int >( args[ 0 ] );
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
    iter = eg::Iterator( g_root[ instance ].g_rootStrokedCircle_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootCircle_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootSpiral_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootAttract_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootRandomise_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootimgui_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_StrokedCircle_stop( eg::Instance instance )
{
    root_StrokedCircle_stopper( instance );
}
void root_StrokedCircle_pause( eg::Instance instance )
{
    if( g_root_StrokedCircle[ instance ].g_root_StrokedCircle_state == ::eg::action_running )
        g_root_StrokedCircle[ instance ].g_root_StrokedCircle_state = ::eg::action_paused;
}
void root_StrokedCircle_resume( eg::Instance instance )
{
    if( g_root_StrokedCircle[ instance ].g_root_StrokedCircle_state == ::eg::action_paused )
        g_root_StrokedCircle[ instance ].g_root_StrokedCircle_state = ::eg::action_running;
}
bool root_StrokedCircle_done( eg::Instance instance )
{
    eg::Iterator iter;
    iter = eg::Iterator( g_root_StrokedCircle[ instance ].g_root_StrokedCircleDefault_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_StrokedCircle[ instance ].g_root_StrokedCircleColorChanger_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_StrokedCircle[ instance ].g_root_StrokedCircleMorph_alloc_iter.load() );
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
    iter = eg::Iterator( g_root_Circle[ instance ].g_root_CircleMorph_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Circle[ instance ].g_root_CircleDefault_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Circle[ instance ].g_root_CircleColorChanger_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Spiral_stop( eg::Instance instance )
{
    root_Spiral_stopper( instance );
}
void root_Spiral_pause( eg::Instance instance )
{
    if( g_root_Spiral[ instance ].g_root_Spiral_state == ::eg::action_running )
        g_root_Spiral[ instance ].g_root_Spiral_state = ::eg::action_paused;
}
void root_Spiral_resume( eg::Instance instance )
{
    if( g_root_Spiral[ instance ].g_root_Spiral_state == ::eg::action_paused )
        g_root_Spiral[ instance ].g_root_Spiral_state = ::eg::action_running;
}
bool root_Spiral_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Attract_stop( eg::Instance instance )
{
    root_Attract_stopper( instance );
}
void root_Attract_pause( eg::Instance instance )
{
    if( g_root_Attract[ instance ].g_root_Attract_state == ::eg::action_running )
        g_root_Attract[ instance ].g_root_Attract_state = ::eg::action_paused;
}
void root_Attract_resume( eg::Instance instance )
{
    if( g_root_Attract[ instance ].g_root_Attract_state == ::eg::action_paused )
        g_root_Attract[ instance ].g_root_Attract_state = ::eg::action_running;
}
bool root_Attract_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Randomise_stop( eg::Instance instance )
{
    root_Randomise_stopper( instance );
}
void root_Randomise_pause( eg::Instance instance )
{
    if( g_root_Randomise[ instance ].g_root_Randomise_state == ::eg::action_running )
        g_root_Randomise[ instance ].g_root_Randomise_state = ::eg::action_paused;
}
void root_Randomise_resume( eg::Instance instance )
{
    if( g_root_Randomise[ instance ].g_root_Randomise_state == ::eg::action_paused )
        g_root_Randomise[ instance ].g_root_Randomise_state = ::eg::action_running;
}
bool root_Randomise_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_imgui_stop( eg::Instance instance )
{
    root_imgui_stopper( instance );
}
void root_imgui_pause( eg::Instance instance )
{
    if( g_root_imgui[ instance ].g_root_imgui_state == ::eg::action_running )
        g_root_imgui[ instance ].g_root_imgui_state = ::eg::action_paused;
}
void root_imgui_resume( eg::Instance instance )
{
    if( g_root_imgui[ instance ].g_root_imgui_state == ::eg::action_paused )
        g_root_imgui[ instance ].g_root_imgui_state = ::eg::action_running;
}
bool root_imgui_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_StrokedCircle_Default_stop( eg::Instance instance )
{
    root_StrokedCircle_Default_stopper( instance );
}
void root_StrokedCircle_Default_pause( eg::Instance instance )
{
    if( g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_state == ::eg::action_running )
        g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_state = ::eg::action_paused;
}
void root_StrokedCircle_Default_resume( eg::Instance instance )
{
    if( g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_state == ::eg::action_paused )
        g_root_StrokedCircle_Default[ instance ].g_root_StrokedCircle_Default_state = ::eg::action_running;
}
bool root_StrokedCircle_Default_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_StrokedCircle_ColorChanger_stop( eg::Instance instance )
{
    root_StrokedCircle_ColorChanger_stopper( instance );
}
void root_StrokedCircle_ColorChanger_pause( eg::Instance instance )
{
    if( g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_state == ::eg::action_running )
        g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_state = ::eg::action_paused;
}
void root_StrokedCircle_ColorChanger_resume( eg::Instance instance )
{
    if( g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_state == ::eg::action_paused )
        g_root_StrokedCircle_ColorChanger[ instance ].g_root_StrokedCircle_ColorChanger_state = ::eg::action_running;
}
bool root_StrokedCircle_ColorChanger_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_StrokedCircle_Morph_stop( eg::Instance instance )
{
    root_StrokedCircle_Morph_stopper( instance );
}
void root_StrokedCircle_Morph_pause( eg::Instance instance )
{
    if( g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_state == ::eg::action_running )
        g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_state = ::eg::action_paused;
}
void root_StrokedCircle_Morph_resume( eg::Instance instance )
{
    if( g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_state == ::eg::action_paused )
        g_root_StrokedCircle_Morph[ instance ].g_root_StrokedCircle_Morph_state = ::eg::action_running;
}
bool root_StrokedCircle_Morph_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Circle_Default_stop( eg::Instance instance )
{
    root_Circle_Default_stopper( instance );
}
void root_Circle_Default_pause( eg::Instance instance )
{
    if( g_root_Circle_Default[ instance ].g_root_Circle_Default_state == ::eg::action_running )
        g_root_Circle_Default[ instance ].g_root_Circle_Default_state = ::eg::action_paused;
}
void root_Circle_Default_resume( eg::Instance instance )
{
    if( g_root_Circle_Default[ instance ].g_root_Circle_Default_state == ::eg::action_paused )
        g_root_Circle_Default[ instance ].g_root_Circle_Default_state = ::eg::action_running;
}
bool root_Circle_Default_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Circle_ColorChanger_stop( eg::Instance instance )
{
    root_Circle_ColorChanger_stopper( instance );
}
void root_Circle_ColorChanger_pause( eg::Instance instance )
{
    if( g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_state == ::eg::action_running )
        g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_state = ::eg::action_paused;
}
void root_Circle_ColorChanger_resume( eg::Instance instance )
{
    if( g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_state == ::eg::action_paused )
        g_root_Circle_ColorChanger[ instance ].g_root_Circle_ColorChanger_state = ::eg::action_running;
}
bool root_Circle_ColorChanger_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Circle_Morph_stop( eg::Instance instance )
{
    root_Circle_Morph_stopper( instance );
}
void root_Circle_Morph_pause( eg::Instance instance )
{
    if( g_root_Circle_Morph[ instance ].g_root_Circle_Morph_state == ::eg::action_running )
        g_root_Circle_Morph[ instance ].g_root_Circle_Morph_state = ::eg::action_paused;
}
void root_Circle_Morph_resume( eg::Instance instance )
{
    if( g_root_Circle_Morph[ instance ].g_root_Circle_Morph_state == ::eg::action_paused )
        g_root_Circle_Morph[ instance ].g_root_Circle_Morph_state = ::eg::action_running;
}
bool root_Circle_Morph_done( eg::Instance instance )
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
    module.def( "g_root_StrokedCircle_m_position_read", g_root_StrokedCircle_m_position_read );
    module.def( "g_root_StrokedCircle_m_position_write", g_root_StrokedCircle_m_position_write );
    module.def( "g_root_StrokedCircle_m_positionActual_read", g_root_StrokedCircle_m_positionActual_read );
    module.def( "g_root_StrokedCircle_m_positionActual_write", g_root_StrokedCircle_m_positionActual_write );
    module.def( "g_root_StrokedCircle_m_color_read", g_root_StrokedCircle_m_color_read );
    module.def( "g_root_StrokedCircle_m_color_write", g_root_StrokedCircle_m_color_write );
    module.def( "g_root_StrokedCircle_m_size_read", g_root_StrokedCircle_m_size_read );
    module.def( "g_root_StrokedCircle_m_size_write", g_root_StrokedCircle_m_size_write );
    module.def( "g_root_StrokedCircle_ColorChanger_m_f_read", g_root_StrokedCircle_ColorChanger_m_f_read );
    module.def( "g_root_StrokedCircle_ColorChanger_m_f_write", g_root_StrokedCircle_ColorChanger_m_f_write );
    module.def( "g_root_StrokedCircle_Morph_m_speed_read", g_root_StrokedCircle_Morph_m_speed_read );
    module.def( "g_root_StrokedCircle_Morph_m_speed_write", g_root_StrokedCircle_Morph_m_speed_write );
    module.def( "g_root_Circle_m_position_read", g_root_Circle_m_position_read );
    module.def( "g_root_Circle_m_position_write", g_root_Circle_m_position_write );
    module.def( "g_root_Circle_m_positionActual_read", g_root_Circle_m_positionActual_read );
    module.def( "g_root_Circle_m_positionActual_write", g_root_Circle_m_positionActual_write );
    module.def( "g_root_Circle_m_color_read", g_root_Circle_m_color_read );
    module.def( "g_root_Circle_m_color_write", g_root_Circle_m_color_write );
    module.def( "g_root_Circle_m_size_read", g_root_Circle_m_size_read );
    module.def( "g_root_Circle_m_size_write", g_root_Circle_m_size_write );
    module.def( "g_root_Circle_ColorChanger_m_f_read", g_root_Circle_ColorChanger_m_f_read );
    module.def( "g_root_Circle_ColorChanger_m_f_write", g_root_Circle_ColorChanger_m_f_write );
    module.def( "g_root_Circle_Morph_m_speed_read", g_root_Circle_Morph_m_speed_read );
    module.def( "g_root_Circle_Morph_m_speed_write", g_root_Circle_Morph_m_speed_write );
    module.def( "g_root_Spiral_m_curve_read", g_root_Spiral_m_curve_read );
    module.def( "g_root_Spiral_m_curve_write", g_root_Spiral_m_curve_write );
    module.def( "g_root_Spiral_m_radius_read", g_root_Spiral_m_radius_read );
    module.def( "g_root_Spiral_m_radius_write", g_root_Spiral_m_radius_write );
    module.def( "g_root_Spiral_m_relative_read", g_root_Spiral_m_relative_read );
    module.def( "g_root_Spiral_m_relative_write", g_root_Spiral_m_relative_write );
    module.def( "g_root_Spiral_m_speed_read", g_root_Spiral_m_speed_read );
    module.def( "g_root_Spiral_m_speed_write", g_root_Spiral_m_speed_write );
    module.def( "g_root_Randomise_m_range_read", g_root_Randomise_m_range_read );
    module.def( "g_root_Randomise_m_range_write", g_root_Randomise_m_range_write );
    module.def( "root_stop", root_stop);
    module.def( "root_pause", root_pause);
    module.def( "root_resume", root_resume);
    module.def( "root_done", root_done);
    module.def( "root_StrokedCircle_operator", &__eg_root< void >::__eg_StrokedCircle< void >::operator() );
    module.def( "root_StrokedCircle_stop", root_StrokedCircle_stop);
    module.def( "root_StrokedCircle_pause", root_StrokedCircle_pause);
    module.def( "root_StrokedCircle_resume", root_StrokedCircle_resume);
    module.def( "root_StrokedCircle_done", root_StrokedCircle_done);
    module.def( "root_Circle_operator", &__eg_root< void >::__eg_Circle< void >::operator() );
    module.def( "root_Circle_stop", root_Circle_stop);
    module.def( "root_Circle_pause", root_Circle_pause);
    module.def( "root_Circle_resume", root_Circle_resume);
    module.def( "root_Circle_done", root_Circle_done);
    module.def( "root_Spiral_operator", &__eg_root< void >::__eg_Spiral< void >::operator() );
    module.def( "root_Spiral_stop", root_Spiral_stop);
    module.def( "root_Spiral_pause", root_Spiral_pause);
    module.def( "root_Spiral_resume", root_Spiral_resume);
    module.def( "root_Spiral_done", root_Spiral_done);
    module.def( "root_Attract_operator", &__eg_root< void >::__eg_Attract< void >::operator() );
    module.def( "root_Attract_stop", root_Attract_stop);
    module.def( "root_Attract_pause", root_Attract_pause);
    module.def( "root_Attract_resume", root_Attract_resume);
    module.def( "root_Attract_done", root_Attract_done);
    module.def( "root_Randomise_operator", &__eg_root< void >::__eg_Randomise< void >::operator() );
    module.def( "root_Randomise_stop", root_Randomise_stop);
    module.def( "root_Randomise_pause", root_Randomise_pause);
    module.def( "root_Randomise_resume", root_Randomise_resume);
    module.def( "root_Randomise_done", root_Randomise_done);
    module.def( "root_imgui_operator", &__eg_root< void >::__eg_imgui< void >::operator() );
    module.def( "root_imgui_stop", root_imgui_stop);
    module.def( "root_imgui_pause", root_imgui_pause);
    module.def( "root_imgui_resume", root_imgui_resume);
    module.def( "root_imgui_done", root_imgui_done);
    module.def( "root_StrokedCircle_Default_operator", &__eg_root< void >::__eg_Shape< void >::__eg_Default< void >::operator() );
    module.def( "root_StrokedCircle_Default_stop", root_StrokedCircle_Default_stop);
    module.def( "root_StrokedCircle_Default_pause", root_StrokedCircle_Default_pause);
    module.def( "root_StrokedCircle_Default_resume", root_StrokedCircle_Default_resume);
    module.def( "root_StrokedCircle_Default_done", root_StrokedCircle_Default_done);
    module.def( "root_StrokedCircle_ColorChanger_operator", &__eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::operator() );
    module.def( "root_StrokedCircle_ColorChanger_stop", root_StrokedCircle_ColorChanger_stop);
    module.def( "root_StrokedCircle_ColorChanger_pause", root_StrokedCircle_ColorChanger_pause);
    module.def( "root_StrokedCircle_ColorChanger_resume", root_StrokedCircle_ColorChanger_resume);
    module.def( "root_StrokedCircle_ColorChanger_done", root_StrokedCircle_ColorChanger_done);
    module.def( "root_StrokedCircle_Morph_operator", &__eg_root< void >::__eg_Shape< void >::__eg_Morph< void >::operator() );
    module.def( "root_StrokedCircle_Morph_stop", root_StrokedCircle_Morph_stop);
    module.def( "root_StrokedCircle_Morph_pause", root_StrokedCircle_Morph_pause);
    module.def( "root_StrokedCircle_Morph_resume", root_StrokedCircle_Morph_resume);
    module.def( "root_StrokedCircle_Morph_done", root_StrokedCircle_Morph_done);
    module.def( "root_Circle_Default_operator", &__eg_root< void >::__eg_Shape< void >::__eg_Default< void >::operator() );
    module.def( "root_Circle_Default_stop", root_Circle_Default_stop);
    module.def( "root_Circle_Default_pause", root_Circle_Default_pause);
    module.def( "root_Circle_Default_resume", root_Circle_Default_resume);
    module.def( "root_Circle_Default_done", root_Circle_Default_done);
    module.def( "root_Circle_ColorChanger_operator", &__eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void >::operator() );
    module.def( "root_Circle_ColorChanger_stop", root_Circle_ColorChanger_stop);
    module.def( "root_Circle_ColorChanger_pause", root_Circle_ColorChanger_pause);
    module.def( "root_Circle_ColorChanger_resume", root_Circle_ColorChanger_resume);
    module.def( "root_Circle_ColorChanger_done", root_Circle_ColorChanger_done);
    module.def( "root_Circle_Morph_operator", &__eg_root< void >::__eg_Shape< void >::__eg_Morph< void >::operator() );
    module.def( "root_Circle_Morph_stop", root_Circle_Morph_stop);
    module.def( "root_Circle_Morph_pause", root_Circle_Morph_pause);
    module.def( "root_Circle_Morph_resume", root_Circle_Morph_resume);
    module.def( "root_Circle_Morph_done", root_Circle_Morph_done);
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
                case 141:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_position_read" )( reference.instance );
                    break;
                case 143:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_positionActual_read" )( reference.instance );
                    break;
                case 145:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_color_read" )( reference.instance );
                    break;
                case 147:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_size_read" )( reference.instance );
                    break;
                case 151:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_ColorChanger_m_f_read" )( reference.instance );
                    break;
                case 154:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_Morph_m_speed_read" )( reference.instance );
                    break;
                case 173:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_read" )( reference.instance );
                    break;
                case 175:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_positionActual_read" )( reference.instance );
                    break;
                case 177:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_read" )( reference.instance );
                    break;
                case 179:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_read" )( reference.instance );
                    break;
                case 183:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_ColorChanger_m_f_read" )( reference.instance );
                    break;
                case 186:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Morph_m_speed_read" )( reference.instance );
                    break;
                case 189:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_curve_read" )( reference.instance );
                    break;
                case 191:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_radius_read" )( reference.instance );
                    break;
                case 193:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_relative_read" )( reference.instance );
                    break;
                case 195:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_speed_read" )( reference.instance );
                    break;
                case 199:
                    pStack->m_result = m_module_eg.attr( "g_root_Randomise_m_range_read" )( reference.instance );
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
                case 141:
                     pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_position_write" )( reference.instance, args );
                     break;
                case 143:
                     pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_positionActual_write" )( reference.instance, args );
                     break;
                case 145:
                     pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_color_write" )( reference.instance, args );
                     break;
                case 147:
                     pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_size_write" )( reference.instance, args );
                     break;
                case 151:
                     pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_ColorChanger_m_f_write" )( reference.instance, args );
                     break;
                case 154:
                     pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_Morph_m_speed_write" )( reference.instance, args );
                     break;
                case 173:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_write" )( reference.instance, args );
                     break;
                case 175:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_positionActual_write" )( reference.instance, args );
                     break;
                case 177:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_write" )( reference.instance, args );
                     break;
                case 179:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_write" )( reference.instance, args );
                     break;
                case 183:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_ColorChanger_m_f_write" )( reference.instance, args );
                     break;
                case 186:
                     pStack->m_result = m_module_eg.attr( "g_root_Circle_Morph_m_speed_write" )( reference.instance, args );
                     break;
                case 189:
                     pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_curve_write" )( reference.instance, args );
                     break;
                case 191:
                     pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_radius_write" )( reference.instance, args );
                     break;
                case 193:
                     pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_relative_write" )( reference.instance, args );
                     break;
                case 195:
                     pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_speed_write" )( reference.instance, args );
                     break;
                case 199:
                     pStack->m_result = m_module_eg.attr( "g_root_Randomise_m_range_write" )( reference.instance, args );
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
                case 94:
                    {
                        __eg_root< void >::__eg_StrokedCircle< void > ref = root_StrokedCircle_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_StrokedCircle_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_StrokedCircle_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 99:
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
                case 104:
                    {
                        __eg_root< void >::__eg_Spiral< void > ref = root_Spiral_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Spiral_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Spiral_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 109:
                    {
                        __eg_root< void >::__eg_Attract< void > ref = root_Attract_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Attract_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Attract_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 114:
                    {
                        __eg_root< void >::__eg_Randomise< void > ref = root_Randomise_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Randomise_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Randomise_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 119:
                    {
                        __eg_root< void >::__eg_imgui< void > ref = root_imgui_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_imgui_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_imgui_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 126:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Default< void > ref = root_StrokedCircle_Default_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_StrokedCircle_Default_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 131:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > ref = root_StrokedCircle_ColorChanger_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_StrokedCircle_ColorChanger_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 136:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > ref = root_StrokedCircle_Morph_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_StrokedCircle_Morph_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 158:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Default< void > ref = root_Circle_Default_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_Default_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_Default_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 163:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > ref = root_Circle_ColorChanger_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_ColorChanger_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 168:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > ref = root_Circle_Morph_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Circle_Morph_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Circle_Morph_stopper( ref.data.instance );
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
                case 94:
                    {
                        __eg_root< void >::__eg_StrokedCircle< void > ref = root_StrokedCircle_starter( reference.instance );
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
                                            case 0: m.attr( "root_StrokedCircle_operator" )( ref ); break;
                                            case 1: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_StrokedCircle_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_StrokedCircle_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 99:
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
                case 104:
                    {
                        __eg_root< void >::__eg_Spiral< void > ref = root_Spiral_starter( reference.instance );
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
                                            case 0: m.attr( "root_Spiral_operator" )( ref ); break;
                                            case 1: m.attr( "root_Spiral_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Spiral_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Spiral_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 109:
                    {
                        __eg_root< void >::__eg_Attract< void > ref = root_Attract_starter( reference.instance );
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
                                            case 0: m.attr( "root_Attract_operator" )( ref ); break;
                                            case 1: m.attr( "root_Attract_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Attract_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Attract_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 114:
                    {
                        __eg_root< void >::__eg_Randomise< void > ref = root_Randomise_starter( reference.instance );
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
                                            case 0: m.attr( "root_Randomise_operator" )( ref ); break;
                                            case 1: m.attr( "root_Randomise_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Randomise_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Randomise_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 119:
                    {
                        __eg_root< void >::__eg_imgui< void > ref = root_imgui_starter( reference.instance );
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
                                            case 0: m.attr( "root_imgui_operator" )( ref ); break;
                                            case 1: m.attr( "root_imgui_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_imgui_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_imgui_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 126:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Default< void > ref = root_StrokedCircle_Default_starter( reference.instance );
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
                                            case 0: m.attr( "root_StrokedCircle_Default_operator" )( ref ); break;
                                            case 1: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_StrokedCircle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_StrokedCircle_Default_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 131:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > ref = root_StrokedCircle_ColorChanger_starter( reference.instance );
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
                                            case 0: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref ); break;
                                            case 1: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_StrokedCircle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_StrokedCircle_ColorChanger_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 136:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > ref = root_StrokedCircle_Morph_starter( reference.instance );
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
                                            case 0: m.attr( "root_StrokedCircle_Morph_operator" )( ref ); break;
                                            case 1: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_StrokedCircle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_StrokedCircle_Morph_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 158:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Default< void > ref = root_Circle_Default_starter( reference.instance );
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
                                            case 0: m.attr( "root_Circle_Default_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_Default_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_Default_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 163:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_ColorChanger< void > ref = root_Circle_ColorChanger_starter( reference.instance );
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
                                            case 0: m.attr( "root_Circle_ColorChanger_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_ColorChanger_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_ColorChanger_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 168:
                    {
                        __eg_root< void >::__eg_Shape< void >::__eg_Morph< void > ref = root_Circle_Morph_starter( reference.instance );
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
                                            case 0: m.attr( "root_Circle_Morph_operator" )( ref ); break;
                                            case 1: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Circle_Morph_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Circle_Morph_stopper( ref.data.instance );
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
                case 88:
                    pStack->m_result = m_module_eg.attr( "root_stop" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_stop" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_stop" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Spiral_stop" )( reference.instance );
                    break;
                case 109:
                    pStack->m_result = m_module_eg.attr( "root_Attract_stop" )( reference.instance );
                    break;
                case 114:
                    pStack->m_result = m_module_eg.attr( "root_Randomise_stop" )( reference.instance );
                    break;
                case 119:
                    pStack->m_result = m_module_eg.attr( "root_imgui_stop" )( reference.instance );
                    break;
                case 126:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Default_stop" )( reference.instance );
                    break;
                case 131:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_ColorChanger_stop" )( reference.instance );
                    break;
                case 136:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Morph_stop" )( reference.instance );
                    break;
                case 158:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Default_stop" )( reference.instance );
                    break;
                case 163:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColorChanger_stop" )( reference.instance );
                    break;
                case 168:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Morph_stop" )( reference.instance );
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
                case 88:
                    pStack->m_result = m_module_eg.attr( "root_pause" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_pause" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_pause" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Spiral_pause" )( reference.instance );
                    break;
                case 109:
                    pStack->m_result = m_module_eg.attr( "root_Attract_pause" )( reference.instance );
                    break;
                case 114:
                    pStack->m_result = m_module_eg.attr( "root_Randomise_pause" )( reference.instance );
                    break;
                case 119:
                    pStack->m_result = m_module_eg.attr( "root_imgui_pause" )( reference.instance );
                    break;
                case 126:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Default_pause" )( reference.instance );
                    break;
                case 131:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_ColorChanger_pause" )( reference.instance );
                    break;
                case 136:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Morph_pause" )( reference.instance );
                    break;
                case 158:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Default_pause" )( reference.instance );
                    break;
                case 163:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColorChanger_pause" )( reference.instance );
                    break;
                case 168:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Morph_pause" )( reference.instance );
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
                case 88:
                    pStack->m_result = m_module_eg.attr( "root_resume" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_resume" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_resume" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Spiral_resume" )( reference.instance );
                    break;
                case 109:
                    pStack->m_result = m_module_eg.attr( "root_Attract_resume" )( reference.instance );
                    break;
                case 114:
                    pStack->m_result = m_module_eg.attr( "root_Randomise_resume" )( reference.instance );
                    break;
                case 119:
                    pStack->m_result = m_module_eg.attr( "root_imgui_resume" )( reference.instance );
                    break;
                case 126:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Default_resume" )( reference.instance );
                    break;
                case 131:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_ColorChanger_resume" )( reference.instance );
                    break;
                case 136:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Morph_resume" )( reference.instance );
                    break;
                case 158:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Default_resume" )( reference.instance );
                    break;
                case 163:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColorChanger_resume" )( reference.instance );
                    break;
                case 168:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Morph_resume" )( reference.instance );
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
                case 88:
                    pStack->m_result = m_module_eg.attr( "root_done" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_done" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "root_Circle_done" )( reference.instance );
                    break;
                case 104:
                    pStack->m_result = m_module_eg.attr( "root_Spiral_done" )( reference.instance );
                    break;
                case 109:
                    pStack->m_result = m_module_eg.attr( "root_Attract_done" )( reference.instance );
                    break;
                case 114:
                    pStack->m_result = m_module_eg.attr( "root_Randomise_done" )( reference.instance );
                    break;
                case 119:
                    pStack->m_result = m_module_eg.attr( "root_imgui_done" )( reference.instance );
                    break;
                case 126:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Default_done" )( reference.instance );
                    break;
                case 131:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_ColorChanger_done" )( reference.instance );
                    break;
                case 136:
                    pStack->m_result = m_module_eg.attr( "root_StrokedCircle_Morph_done" )( reference.instance );
                    break;
                case 158:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Default_done" )( reference.instance );
                    break;
                case 163:
                    pStack->m_result = m_module_eg.attr( "root_Circle_ColorChanger_done" )( reference.instance );
                    break;
                case 168:
                    pStack->m_result = m_module_eg.attr( "root_Circle_Morph_done" )( reference.instance );
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
                case 141:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_position_read" )( reference.instance );
                    break;
                case 143:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_positionActual_read" )( reference.instance );
                    break;
                case 145:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_color_read" )( reference.instance );
                    break;
                case 147:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_size_read" )( reference.instance );
                    break;
                case 151:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_ColorChanger_m_f_read" )( reference.instance );
                    break;
                case 154:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_Morph_m_speed_read" )( reference.instance );
                    break;
                case 173:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_read" )( reference.instance );
                    break;
                case 175:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_positionActual_read" )( reference.instance );
                    break;
                case 177:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_read" )( reference.instance );
                    break;
                case 179:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_read" )( reference.instance );
                    break;
                case 183:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_ColorChanger_m_f_read" )( reference.instance );
                    break;
                case 186:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Morph_m_speed_read" )( reference.instance );
                    break;
                case 189:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_curve_read" )( reference.instance );
                    break;
                case 191:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_radius_read" )( reference.instance );
                    break;
                case 193:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_relative_read" )( reference.instance );
                    break;
                case 195:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_speed_read" )( reference.instance );
                    break;
                case 199:
                    pStack->m_result = m_module_eg.attr( "g_root_Randomise_m_range_read" )( reference.instance );
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
                case 141:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_position_read" )( reference.instance );
                    break;
                case 143:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_positionActual_read" )( reference.instance );
                    break;
                case 145:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_color_read" )( reference.instance );
                    break;
                case 147:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_m_size_read" )( reference.instance );
                    break;
                case 151:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_ColorChanger_m_f_read" )( reference.instance );
                    break;
                case 154:
                    pStack->m_result = m_module_eg.attr( "g_root_StrokedCircle_Morph_m_speed_read" )( reference.instance );
                    break;
                case 173:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_position_read" )( reference.instance );
                    break;
                case 175:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_positionActual_read" )( reference.instance );
                    break;
                case 177:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_color_read" )( reference.instance );
                    break;
                case 179:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_m_size_read" )( reference.instance );
                    break;
                case 183:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_ColorChanger_m_f_read" )( reference.instance );
                    break;
                case 186:
                    pStack->m_result = m_module_eg.attr( "g_root_Circle_Morph_m_speed_read" )( reference.instance );
                    break;
                case 189:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_curve_read" )( reference.instance );
                    break;
                case 191:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_radius_read" )( reference.instance );
                    break;
                case 193:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_relative_read" )( reference.instance );
                    break;
                case 195:
                    pStack->m_result = m_module_eg.attr( "g_root_Spiral_m_speed_read" )( reference.instance );
                    break;
                case 199:
                    pStack->m_result = m_module_eg.attr( "g_root_Randomise_m_range_read" )( reference.instance );
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

    