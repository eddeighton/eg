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
static std::array< b_root_Square, 1 > g_root_Square_array;
static std::array< b_root_imgui, 4 > g_root_imgui_array;
b_root* g_root = g_root_array.data();
b_root_Square* g_root_Square = g_root_Square_array.data();
b_root_imgui* g_root_imgui = g_root_imgui_array.data();

void allocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 28, 0 } );
        g_root[ i ].g_rootSquare_alloc_iter = 0UL;
        g_root[ i ].g_rootimgui_alloc_iter = 0UL;
    }
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Square[ i ].g_root_Square_cycle = eg::INVALID_TIMESTAMP;
        g_root_Square[ i ].g_root_Square_state = ::eg::action_stopped;
        g_root_Square[ i ].g_root_Square_reference = __eg_root< void >::__eg_Square< void >( eg::reference { i, 34, 0 } );
        g_root_Square[ i ].g_root_Square_alloc_data = i;
    }
    for( eg::Instance i = 0U; i != 4; ++i )
    {
        g_root_imgui[ i ].g_root_imgui_cycle = eg::INVALID_TIMESTAMP;
        g_root_imgui[ i ].g_root_imgui_state = ::eg::action_stopped;
        g_root_imgui[ i ].g_root_imgui_reference = __eg_root< void >::__eg_imgui< void >( eg::reference { i, 39, 0 } );
        g_root_imgui[ i ].g_root_imgui_alloc_data = i;
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 4; ++i )
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
extern __eg_root< void >::__eg_Square< void > root_Square_starter( eg::Instance _gid );
extern void root_Square_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_imgui< void > root_imgui_starter( eg::Instance _gid );
extern void root_imgui_stopper( eg::Instance _gid );



eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 28: return g_root[ instance ].g_root_reference.data.timestamp;
        case 34: return g_root_Square[ instance ].g_root_Square_reference.data.timestamp;
        case 39: return g_root_imgui[ instance ].g_root_imgui_reference.data.timestamp;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 28: return g_root[ instance ].g_root_state;
        case 34: return g_root_Square[ instance ].g_root_Square_state;
        case 39: return g_root_imgui[ instance ].g_root_imgui_state;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 28: return g_root[ instance ].g_root_fiber;
        case 34: return g_root_Square[ instance ].g_root_Square_fiber;
        case 39: return g_root_imgui[ instance ].g_root_imgui_fiber;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 28: return g_root[ instance ].g_root_cycle;
        case 34: return g_root_Square[ instance ].g_root_Square_cycle;
        case 39: return g_root_imgui[ instance ].g_root_imgui_cycle;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
__eg_root< void > get_root()
{
    return  __eg_root< void >( eg::reference{ 0, 28, getTimestamp( 28, 0 ) } );
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
        template <> struct type_caster< __eg_root< void >::__eg_Square< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Square< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Square< void > src, return_value_policy /* policy */, handle /* parent */)
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
float g_root_m_size_read( eg::Instance instance )
{
    return g_root[ instance ].m_size;
}
void g_root_m_size_write( eg::Instance instance, pybind11::tuple args )
{
    g_root[ instance ].m_size = pybind11::cast< float >( args[ 0 ] );
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
    iter = eg::Iterator( g_root[ instance ].g_rootSquare_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root[ instance ].g_rootimgui_alloc_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Square_stop( eg::Instance instance )
{
    root_Square_stopper( instance );
}
void root_Square_pause( eg::Instance instance )
{
    if( g_root_Square[ instance ].g_root_Square_state == ::eg::action_running )
        g_root_Square[ instance ].g_root_Square_state = ::eg::action_paused;
}
void root_Square_resume( eg::Instance instance )
{
    if( g_root_Square[ instance ].g_root_Square_state == ::eg::action_paused )
        g_root_Square[ instance ].g_root_Square_state = ::eg::action_running;
}
bool root_Square_done( eg::Instance instance )
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
    module.def( "g_root_m_size_read", g_root_m_size_read );
    module.def( "g_root_m_size_write", g_root_m_size_write );
    module.def( "root_stop", root_stop);
    module.def( "root_pause", root_pause);
    module.def( "root_resume", root_resume);
    module.def( "root_done", root_done);
    module.def( "root_Square_operator", &__eg_root< void >::__eg_Square< void >::operator() );
    module.def( "root_Square_stop", root_Square_stop);
    module.def( "root_Square_pause", root_Square_pause);
    module.def( "root_Square_resume", root_Square_resume);
    module.def( "root_Square_done", root_Square_done);
    module.def( "root_imgui_operator", &__eg_root< void >::__eg_imgui< void >::operator() );
    module.def( "root_imgui_stop", root_imgui_stop);
    module.def( "root_imgui_pause", root_imgui_pause);
    module.def( "root_imgui_resume", root_imgui_resume);
    module.def( "root_imgui_done", root_imgui_done);
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
                case 44:
                    pStack->m_result = m_module_eg.attr( "g_root_m_size_read" )( reference.instance );
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
                case 44:
                     pStack->m_result = m_module_eg.attr( "g_root_m_size_write" )( reference.instance, args );
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
                case 34:
                    {
                        __eg_root< void >::__eg_Square< void > ref = root_Square_starter( reference.instance );
                        if( ref )
                        {
                            switch( args.size() )
                            {
                                case 0: m_module_eg.attr( "root_Square_operator" )( ref ); break;
                                case 1: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ] ); break;
                                case 2: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                case 3: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                case 4: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                case 5: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                case 6: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                case 7: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                case 8: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                case 9: m_module_eg.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                 default: break;
                            }
                            root_Square_stopper( ref.data.instance );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 39:
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
                case 34:
                    {
                        __eg_root< void >::__eg_Square< void > ref = root_Square_starter( reference.instance );
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
                                            case 0: m.attr( "root_Square_operator" )( ref ); break;
                                            case 1: m.attr( "root_Square_operator" )( ref, args[ 0 ] ); break;
                                            case 2: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ] ); break;
                                            case 3: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ] ); break;
                                            case 4: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ] ); break;
                                            case 5: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ] ); break;
                                            case 6: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ] ); break;
                                            case 7: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ] ); break;
                                            case 8: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ] ); break;
                                            case 9: m.attr( "root_Square_operator" )( ref, args[ 0 ], args[ 1 ], args[ 2 ], args[ 3 ], args[ 4 ], args[ 5 ], args[ 6 ], args[ 7 ], args[ 8 ] ); break;
                                             default: break;
                                        }
                                    }
                                    catch( eg::termination_exception )
                                    {
                                    }
                                    root_Square_stopper( ref.data.instance );
                                }
                            );
                            getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                        }
                        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                    }
                    break;
                case 39:
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
                case 28:
                    pStack->m_result = m_module_eg.attr( "root_stop" )( reference.instance );
                    break;
                case 34:
                    pStack->m_result = m_module_eg.attr( "root_Square_stop" )( reference.instance );
                    break;
                case 39:
                    pStack->m_result = m_module_eg.attr( "root_imgui_stop" )( reference.instance );
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
                case 28:
                    pStack->m_result = m_module_eg.attr( "root_pause" )( reference.instance );
                    break;
                case 34:
                    pStack->m_result = m_module_eg.attr( "root_Square_pause" )( reference.instance );
                    break;
                case 39:
                    pStack->m_result = m_module_eg.attr( "root_imgui_pause" )( reference.instance );
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
                case 28:
                    pStack->m_result = m_module_eg.attr( "root_resume" )( reference.instance );
                    break;
                case 34:
                    pStack->m_result = m_module_eg.attr( "root_Square_resume" )( reference.instance );
                    break;
                case 39:
                    pStack->m_result = m_module_eg.attr( "root_imgui_resume" )( reference.instance );
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
                case 28:
                    pStack->m_result = m_module_eg.attr( "root_done" )( reference.instance );
                    break;
                case 34:
                    pStack->m_result = m_module_eg.attr( "root_Square_done" )( reference.instance );
                    break;
                case 39:
                    pStack->m_result = m_module_eg.attr( "root_imgui_done" )( reference.instance );
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
                case 44:
                    pStack->m_result = m_module_eg.attr( "g_root_m_size_read" )( reference.instance );
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
                case 44:
                    pStack->m_result = m_module_eg.attr( "g_root_m_size_read" )( reference.instance );
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

    