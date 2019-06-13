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
static std::array< b_root_Camera, 1 > g_root_Camera_array;
static std::array< b_root_Camera_Orbit, 1 > g_root_Camera_Orbit_array;
static std::array< b_root_Cube, 9 > g_root_Cube_array;
static std::array< b_root_CreateCube, 1 > g_root_CreateCube_array;
b_root* g_root = g_root_array.data();
b_root_Camera* g_root_Camera = g_root_Camera_array.data();
b_root_Camera_Orbit* g_root_Camera_Orbit = g_root_Camera_Orbit_array.data();
b_root_Cube* g_root_Cube = g_root_Cube_array.data();
b_root_CreateCube* g_root_CreateCube = g_root_CreateCube_array.data();

void allocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 56, 0 } );
        g_root[ i ].g_rootCamera_alloc_iter = 0UL;
        g_root[ i ].g_rootCube_alloc_iter = 0UL;
        g_root[ i ].g_rootCreateCube_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Camera[ i ].g_root_Camera_cycle = eg::INVALID_TIMESTAMP;
        g_root_Camera[ i ].g_root_Camera_state = ::eg::action_stopped;
        g_root_Camera[ i ].g_root_Camera_reference = __eg_root< void >::__eg_Camera< void >( eg::reference { i, 62, 0 } );
        g_root_Camera[ i ].g_root_Camera_alloc_data = i;
        g_root_Camera[ i ].g_root_CameraOrbit_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Camera_Orbit[ i ].g_root_Camera_Orbit_cycle = eg::INVALID_TIMESTAMP;
        g_root_Camera_Orbit[ i ].g_root_Camera_Orbit_state = ::eg::action_stopped;
        g_root_Camera_Orbit[ i ].g_root_Camera_Orbit_reference = __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >( eg::reference { i, 82, 0 } );
        g_root_Camera_Orbit[ i ].g_root_Camera_Orbit_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 9; ++i )
    {
        g_root_Cube[ i ].g_root_Cube_cycle = eg::INVALID_TIMESTAMP;
        g_root_Cube[ i ].g_root_Cube_state = ::eg::action_stopped;
        g_root_Cube[ i ].g_root_Cube_reference = __eg_root< void >::__eg_Cube< void >( eg::reference { i, 67, 0 } );
        g_root_Cube[ i ].g_root_Cube_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_CreateCube[ i ].g_root_CreateCube_cycle = eg::INVALID_TIMESTAMP;
        g_root_CreateCube[ i ].g_root_CreateCube_state = ::eg::action_stopped;
        g_root_CreateCube[ i ].g_root_CreateCube_reference = __eg_root< void >::__eg_CreateCube< void >( eg::reference { i, 72, 0 } );
        g_root_CreateCube[ i ].g_root_CreateCube_alloc_data = i;
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
    for( eg::Instance i = 0U; i != 9; ++i )
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
}

//Action functions
extern __eg_root< void > root_starter( std::vector< std::function< void() > >& );
extern void root_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Camera< void > root_Camera_starter( eg::Instance _gid );
extern void root_Camera_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Cube< void > root_Cube_starter( eg::Instance _gid );
extern void root_Cube_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_CreateCube< void > root_CreateCube_starter( eg::Instance _gid );
extern void root_CreateCube_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > root_Camera_Orbit_starter( eg::Instance _gid );
extern void root_Camera_Orbit_stopper( eg::Instance _gid );



eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 56: return g_root[ instance ].g_root_reference.data.timestamp;
        case 62: return g_root_Camera[ instance ].g_root_Camera_reference.data.timestamp;
        case 67: return g_root_Cube[ instance ].g_root_Cube_reference.data.timestamp;
        case 72: return g_root_CreateCube[ instance ].g_root_CreateCube_reference.data.timestamp;
        case 82: return g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_reference.data.timestamp;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 56: return g_root[ instance ].g_root_state;
        case 62: return g_root_Camera[ instance ].g_root_Camera_state;
        case 67: return g_root_Cube[ instance ].g_root_Cube_state;
        case 72: return g_root_CreateCube[ instance ].g_root_CreateCube_state;
        case 82: return g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_state;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 56: return g_root[ instance ].g_root_fiber;
        case 62: return g_root_Camera[ instance ].g_root_Camera_fiber;
        case 67: return g_root_Cube[ instance ].g_root_Cube_fiber;
        case 72: return g_root_CreateCube[ instance ].g_root_CreateCube_fiber;
        case 82: return g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_fiber;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 56: return g_root[ instance ].g_root_cycle;
        case 62: return g_root_Camera[ instance ].g_root_Camera_cycle;
        case 67: return g_root_Cube[ instance ].g_root_Cube_cycle;
        case 72: return g_root_CreateCube[ instance ].g_root_CreateCube_cycle;
        case 82: return g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_cycle;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
__eg_root< void > get_root()
{
    return  __eg_root< void >( eg::reference{ 0, 56, getTimestamp( 56, 0 ) } );
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
        template <> struct type_caster< __eg_root< void >::__eg_Camera< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Camera< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Camera< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Cube< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Cube< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Cube< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_CreateCube< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_CreateCube< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_CreateCube< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
    }   //namespace detail
} // namespace pybind11

//Python Interface
int g_root_m_x_read( eg::Instance instance )
{
    return g_root[ instance ].m_x;
}
void g_root_m_x_write( eg::Instance instance, pybind11::tuple args )
{
    g_root[ instance ].m_x = pybind11::cast< int >( args[ 0 ] );
}
int g_root_m_y_read( eg::Instance instance )
{
    return g_root[ instance ].m_y;
}
void g_root_m_y_write( eg::Instance instance, pybind11::tuple args )
{
    g_root[ instance ].m_y = pybind11::cast< int >( args[ 0 ] );
}
class cinder::CameraPersp g_root_Camera_m_perspective_read( eg::Instance instance )
{
    return g_root_Camera[ instance ].m_perspective;
}
void g_root_Camera_m_perspective_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Camera[ instance ].m_perspective = pybind11::cast< class cinder::CameraPersp >( args[ 0 ] );
}
struct glm::tvec3<float, glm::highp> g_root_Camera_m_eye_read( eg::Instance instance )
{
    return g_root_Camera[ instance ].m_eye;
}
void g_root_Camera_m_eye_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Camera[ instance ].m_eye = pybind11::cast< struct glm::tvec3<float, glm::highp> >( args[ 0 ] );
}
float g_root_Camera_Orbit_m_rate_read( eg::Instance instance )
{
    return g_root_Camera_Orbit[ instance ].m_rate;
}
void g_root_Camera_Orbit_m_rate_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Camera_Orbit[ instance ].m_rate = pybind11::cast< float >( args[ 0 ] );
}
float g_root_Camera_Orbit_m_radius_read( eg::Instance instance )
{
    return g_root_Camera_Orbit[ instance ].m_radius;
}
void g_root_Camera_Orbit_m_radius_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Camera_Orbit[ instance ].m_radius = pybind11::cast< float >( args[ 0 ] );
}
struct glm::tvec3<float, glm::highp> g_root_Cube_m_position_read( eg::Instance instance )
{
    return g_root_Cube[ instance ].m_position;
}
void g_root_Cube_m_position_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Cube[ instance ].m_position = pybind11::cast< struct glm::tvec3<float, glm::highp> >( args[ 0 ] );
}
struct glm::tvec3<float, glm::highp> g_root_Cube_m_size_read( eg::Instance instance )
{
    return g_root_Cube[ instance ].m_size;
}
void g_root_Cube_m_size_write( eg::Instance instance, pybind11::tuple args )
{
    g_root_Cube[ instance ].m_size = pybind11::cast< struct glm::tvec3<float, glm::highp> >( args[ 0 ] );
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
    iter = eg::Iterator( g_root[ instance ].g_rootCamera_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootCube_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootCreateCube_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Camera_stop( eg::Instance instance )
{
    root_Camera_stopper( instance );
}
void root_Camera_pause( eg::Instance instance )
{
    if( g_root_Camera[ instance ].g_root_Camera_state == ::eg::action_running )
        g_root_Camera[ instance ].g_root_Camera_state = ::eg::action_paused;
}
void root_Camera_resume( eg::Instance instance )
{
    if( g_root_Camera[ instance ].g_root_Camera_state == ::eg::action_paused )
        g_root_Camera[ instance ].g_root_Camera_state = ::eg::action_running;
}
bool root_Camera_done( eg::Instance instance )
{
    eg::Iterator iter;
    iter = eg::Iterator( g_root_Camera[ instance ].g_root_CameraOrbit_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Cube_stop( eg::Instance instance )
{
    root_Cube_stopper( instance );
}
void root_Cube_pause( eg::Instance instance )
{
    if( g_root_Cube[ instance ].g_root_Cube_state == ::eg::action_running )
        g_root_Cube[ instance ].g_root_Cube_state = ::eg::action_paused;
}
void root_Cube_resume( eg::Instance instance )
{
    if( g_root_Cube[ instance ].g_root_Cube_state == ::eg::action_paused )
        g_root_Cube[ instance ].g_root_Cube_state = ::eg::action_running;
}
bool root_Cube_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_CreateCube_stop( eg::Instance instance )
{
    root_CreateCube_stopper( instance );
}
void root_CreateCube_pause( eg::Instance instance )
{
    if( g_root_CreateCube[ instance ].g_root_CreateCube_state == ::eg::action_running )
        g_root_CreateCube[ instance ].g_root_CreateCube_state = ::eg::action_paused;
}
void root_CreateCube_resume( eg::Instance instance )
{
    if( g_root_CreateCube[ instance ].g_root_CreateCube_state == ::eg::action_paused )
        g_root_CreateCube[ instance ].g_root_CreateCube_state = ::eg::action_running;
}
bool root_CreateCube_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Camera_Orbit_stop( eg::Instance instance )
{
    root_Camera_Orbit_stopper( instance );
}
void root_Camera_Orbit_pause( eg::Instance instance )
{
    if( g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_state == ::eg::action_running )
        g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_state = ::eg::action_paused;
}
void root_Camera_Orbit_resume( eg::Instance instance )
{
    if( g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_state == ::eg::action_paused )
        g_root_Camera_Orbit[ instance ].g_root_Camera_Orbit_state = ::eg::action_running;
}
bool root_Camera_Orbit_done( eg::Instance instance )
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
    module.def( "g_root_m_x_read", g_root_m_x_read );
    module.def( "g_root_m_x_write", g_root_m_x_write );
    module.def( "g_root_m_y_read", g_root_m_y_read );
    module.def( "g_root_m_y_write", g_root_m_y_write );
    module.def( "g_root_Camera_m_perspective_read", g_root_Camera_m_perspective_read );
    module.def( "g_root_Camera_m_perspective_write", g_root_Camera_m_perspective_write );
    module.def( "g_root_Camera_m_eye_read", g_root_Camera_m_eye_read );
    module.def( "g_root_Camera_m_eye_write", g_root_Camera_m_eye_write );
    module.def( "g_root_Camera_Orbit_m_rate_read", g_root_Camera_Orbit_m_rate_read );
    module.def( "g_root_Camera_Orbit_m_rate_write", g_root_Camera_Orbit_m_rate_write );
    module.def( "g_root_Camera_Orbit_m_radius_read", g_root_Camera_Orbit_m_radius_read );
    module.def( "g_root_Camera_Orbit_m_radius_write", g_root_Camera_Orbit_m_radius_write );
    module.def( "g_root_Cube_m_position_read", g_root_Cube_m_position_read );
    module.def( "g_root_Cube_m_position_write", g_root_Cube_m_position_write );
    module.def( "g_root_Cube_m_size_read", g_root_Cube_m_size_read );
    module.def( "g_root_Cube_m_size_write", g_root_Cube_m_size_write );
    module.def( "root_stop", root_stop);
    module.def( "root_pause", root_pause);
    module.def( "root_resume", root_resume);
    module.def( "root_done", root_done);
    module.def( "root_Camera_operator", &__eg_root< void >::__eg_Camera< void >::operator() );
    module.def( "root_Camera_stop", root_Camera_stop);
    module.def( "root_Camera_pause", root_Camera_pause);
    module.def( "root_Camera_resume", root_Camera_resume);
    module.def( "root_Camera_done", root_Camera_done);
    module.def( "root_Cube_operator", &__eg_root< void >::__eg_Cube< void >::operator() );
    module.def( "root_Cube_stop", root_Cube_stop);
    module.def( "root_Cube_pause", root_Cube_pause);
    module.def( "root_Cube_resume", root_Cube_resume);
    module.def( "root_Cube_done", root_Cube_done);
    module.def( "root_CreateCube_operator", &__eg_root< void >::__eg_CreateCube< void >::operator() );
    module.def( "root_CreateCube_stop", root_CreateCube_stop);
    module.def( "root_CreateCube_pause", root_CreateCube_pause);
    module.def( "root_CreateCube_resume", root_CreateCube_resume);
    module.def( "root_CreateCube_done", root_CreateCube_done);
    module.def( "root_Camera_Orbit_operator", &__eg_root< void >::__eg_Camera< void >::__eg_Orbit< void >::operator() );
    module.def( "root_Camera_Orbit_stop", root_Camera_Orbit_stop);
    module.def( "root_Camera_Orbit_pause", root_Camera_Orbit_pause);
    module.def( "root_Camera_Orbit_resume", root_Camera_Orbit_resume);
    module.def( "root_Camera_Orbit_done", root_Camera_Orbit_done);
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
                case 77:
                    pStack->m_result = m_module_eg.attr( "g_root_m_x_read" )( reference.instance );
                    break;
                case 79:
                    pStack->m_result = m_module_eg.attr( "g_root_m_y_read" )( reference.instance );
                    break;
                case 87:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_m_perspective_read" )( reference.instance );
                    break;
                case 89:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_m_eye_read" )( reference.instance );
                    break;
                case 92:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_rate_read" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_radius_read" )( reference.instance );
                    break;
                case 97:
                    pStack->m_result = m_module_eg.attr( "g_root_Cube_m_position_read" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "g_root_Cube_m_size_read" )( reference.instance );
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
                case 77:
                     pStack->m_result = m_module_eg.attr( "g_root_m_x_write" )( reference.instance, args );
                     break;
                case 79:
                     pStack->m_result = m_module_eg.attr( "g_root_m_y_write" )( reference.instance, args );
                     break;
                case 87:
                     pStack->m_result = m_module_eg.attr( "g_root_Camera_m_perspective_write" )( reference.instance, args );
                     break;
                case 89:
                     pStack->m_result = m_module_eg.attr( "g_root_Camera_m_eye_write" )( reference.instance, args );
                     break;
                case 92:
                     pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_rate_write" )( reference.instance, args );
                     break;
                case 94:
                     pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_radius_write" )( reference.instance, args );
                     break;
                case 97:
                     pStack->m_result = m_module_eg.attr( "g_root_Cube_m_position_write" )( reference.instance, args );
                     break;
                case 99:
                     pStack->m_result = m_module_eg.attr( "g_root_Cube_m_size_write" )( reference.instance, args );
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
                case 62:
                    {
                        __eg_root< void >::__eg_Camera< void > ref = root_Camera_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Camera_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Camera_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 67:
                    {
                        __eg_root< void >::__eg_Cube< void > ref = root_Cube_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Cube_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Cube_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 72:
                    {
                        __eg_root< void >::__eg_CreateCube< void > ref = root_CreateCube_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_CreateCube_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_CreateCube_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 82:
                    {
                        __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > ref = root_Camera_Orbit_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Camera_Orbit_stopper( ref.data.instance );
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
                case 62:
                    {
                        __eg_root< void >::__eg_Camera< void > ref = root_Camera_starter( reference.instance );
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
                                            case 0: m.attr( "root_Camera_operator" )( ref ); break;
                                            case 1: m.attr( "root_Camera_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Camera_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Camera_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 67:
                    {
                        __eg_root< void >::__eg_Cube< void > ref = root_Cube_starter( reference.instance );
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
                                            case 0: m.attr( "root_Cube_operator" )( ref ); break;
                                            case 1: m.attr( "root_Cube_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Cube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Cube_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 72:
                    {
                        __eg_root< void >::__eg_CreateCube< void > ref = root_CreateCube_starter( reference.instance );
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
                                            case 0: m.attr( "root_CreateCube_operator" )( ref ); break;
                                            case 1: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_CreateCube_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_CreateCube_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 82:
                    {
                        __eg_root< void >::__eg_Camera< void >::__eg_Orbit< void > ref = root_Camera_Orbit_starter( reference.instance );
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
                                            case 0: m.attr( "root_Camera_Orbit_operator" )( ref ); break;
                                            case 1: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Camera_Orbit_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Camera_Orbit_stopper( ref.data.instance );
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
                case 56:
                    pStack->m_result = m_module_eg.attr( "root_stop" )( reference.instance );
                    break;
                case 62:
                    pStack->m_result = m_module_eg.attr( "root_Camera_stop" )( reference.instance );
                    break;
                case 67:
                    pStack->m_result = m_module_eg.attr( "root_Cube_stop" )( reference.instance );
                    break;
                case 72:
                    pStack->m_result = m_module_eg.attr( "root_CreateCube_stop" )( reference.instance );
                    break;
                case 82:
                    pStack->m_result = m_module_eg.attr( "root_Camera_Orbit_stop" )( reference.instance );
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
                case 56:
                    pStack->m_result = m_module_eg.attr( "root_pause" )( reference.instance );
                    break;
                case 62:
                    pStack->m_result = m_module_eg.attr( "root_Camera_pause" )( reference.instance );
                    break;
                case 67:
                    pStack->m_result = m_module_eg.attr( "root_Cube_pause" )( reference.instance );
                    break;
                case 72:
                    pStack->m_result = m_module_eg.attr( "root_CreateCube_pause" )( reference.instance );
                    break;
                case 82:
                    pStack->m_result = m_module_eg.attr( "root_Camera_Orbit_pause" )( reference.instance );
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
                case 56:
                    pStack->m_result = m_module_eg.attr( "root_resume" )( reference.instance );
                    break;
                case 62:
                    pStack->m_result = m_module_eg.attr( "root_Camera_resume" )( reference.instance );
                    break;
                case 67:
                    pStack->m_result = m_module_eg.attr( "root_Cube_resume" )( reference.instance );
                    break;
                case 72:
                    pStack->m_result = m_module_eg.attr( "root_CreateCube_resume" )( reference.instance );
                    break;
                case 82:
                    pStack->m_result = m_module_eg.attr( "root_Camera_Orbit_resume" )( reference.instance );
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
                case 56:
                    pStack->m_result = m_module_eg.attr( "root_done" )( reference.instance );
                    break;
                case 62:
                    pStack->m_result = m_module_eg.attr( "root_Camera_done" )( reference.instance );
                    break;
                case 67:
                    pStack->m_result = m_module_eg.attr( "root_Cube_done" )( reference.instance );
                    break;
                case 72:
                    pStack->m_result = m_module_eg.attr( "root_CreateCube_done" )( reference.instance );
                    break;
                case 82:
                    pStack->m_result = m_module_eg.attr( "root_Camera_Orbit_done" )( reference.instance );
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
                case 77:
                    pStack->m_result = m_module_eg.attr( "g_root_m_x_read" )( reference.instance );
                    break;
                case 79:
                    pStack->m_result = m_module_eg.attr( "g_root_m_y_read" )( reference.instance );
                    break;
                case 87:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_m_perspective_read" )( reference.instance );
                    break;
                case 89:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_m_eye_read" )( reference.instance );
                    break;
                case 92:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_rate_read" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_radius_read" )( reference.instance );
                    break;
                case 97:
                    pStack->m_result = m_module_eg.attr( "g_root_Cube_m_position_read" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "g_root_Cube_m_size_read" )( reference.instance );
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
                case 77:
                    pStack->m_result = m_module_eg.attr( "g_root_m_x_read" )( reference.instance );
                    break;
                case 79:
                    pStack->m_result = m_module_eg.attr( "g_root_m_y_read" )( reference.instance );
                    break;
                case 87:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_m_perspective_read" )( reference.instance );
                    break;
                case 89:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_m_eye_read" )( reference.instance );
                    break;
                case 92:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_rate_read" )( reference.instance );
                    break;
                case 94:
                    pStack->m_result = m_module_eg.attr( "g_root_Camera_Orbit_m_radius_read" )( reference.instance );
                    break;
                case 97:
                    pStack->m_result = m_module_eg.attr( "g_root_Cube_m_position_read" )( reference.instance );
                    break;
                case 99:
                    pStack->m_result = m_module_eg.attr( "g_root_Cube_m_size_read" )( reference.instance );
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

    