#include "structures.hpp"
#include "host_clock.hpp"
#include "host_event_log.hpp"
#include "pybind11/embed.h"
#include "pybind11/numpy.h"
#include "pybind11/stl.h"
#include "pybind11/stl_bind.h"
#include "py_eg_reference.hpp"
#include "eg_runtime/eg_runtime.hpp"
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

//buffers
static std::array< b_root, 1 > g_root;
void allocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 9, 0 } );
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 1; ++i )
    {
    }
}

//initialiser
extern void initialise( eg::DependencyProvider* pDependencyProvider );

//Action functions
extern __eg_root< void > root_starter( std::vector< std::function< void() > >& );
extern void root_stopper( eg::Instance _gid );

eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 9: return g_root[ instance ].g_root_reference.data.timestamp;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::ActionState getState( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 9: return g_root[ instance ].g_root_state;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
boost::fibers::fiber& getFiber( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 9: return g_root[ instance ].g_root_fiber;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance )
{
    switch( typeID )
    {
        case 9: return g_root[ instance ].g_root_cycle;
        default: throw std::runtime_error( "Invalid action instance" );
    }
}
__eg_root< void > get_root()
{
    return  __eg_root< void >( eg::reference{ 0, 9, getTimestamp( 9, 0 ) } );
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
    }   //namespace detail
} // namespace pybind11

//Python Interface
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
    module.def( "root_stop", root_stop);
    module.def( "root_pause", root_pause);
    module.def( "root_resume", root_resume);
    module.def( "root_done", root_done);
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
                case 9:
                    pStack->m_result = m_module_eg.attr( "root_stop" )( reference.instance );
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
                case 9:
                    pStack->m_result = m_module_eg.attr( "root_pause" )( reference.instance );
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
                case 9:
                    pStack->m_result = m_module_eg.attr( "root_resume" )( reference.instance );
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
                case 9:
                    pStack->m_result = m_module_eg.attr( "root_done" )( reference.instance );
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


//Dependency Provider Implementation
struct BasicHost_EGDependencyProvider : public eg::DependencyProvider
{
     eg::_clock* m_pClock;
     eg::_event_log* m_pEventLog;

     BasicHost_EGDependencyProvider( eg::_clock* pClock, eg::_event_log* pEventLog )
         :   m_pClock( pClock ),
             m_pEventLog( pEventLog )
     {

     }

    virtual void* getBuffer( const char* pszName )
    {
        if( 0U == strcmp( pszName, "g_root" ) ) return g_root.data();
        return nullptr;
    }
    virtual void* getInterface( const char* pszName )
    {
        if( 0U == strcmp( pszName, "_clock" ) ) return m_pClock;
        if( 0U == strcmp( pszName, "_event_log" ) ) return m_pEventLog;
        return nullptr;
    }
};


std::string strEventLogFolder = "log";


    
int main( int argc, const char* argv[] )
{
    std::string strDatabaseFile;
    std::vector< std::string > scripts;
    int iMilliseconds = 16;
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
                ("database",    po::value< std::string >( &strDatabaseFile ), "Program Database" )
                ("python",      po::value< std::vector< std::string > >( &scripts ), "Python scripts" )
                ("rate",        po::value< int >( &iMilliseconds ), "Simulation rate in milliseconds" )
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
    
    
    try
    {
        const HostClock::TickDuration sleepDuration = 
            std::chrono::milliseconds( iMilliseconds );
        
        //allocate everything
        allocate_buffers();
        
        //setup the dependencies
        HostClock theClock;
        HostEventLog theEventLog( "basic", strEventLogFolder.c_str() );
        
        BasicHost_EGDependencyProvider dependencies( &theClock, &theEventLog );
        initialise( &dependencies );
        
        //be sure to initialise the clock before the scheduler
        boost::fibers::use_scheduling_algorithm< eg::eg_algorithm >();
    
        pybind11::scoped_interpreter guard{}; // start the python interpreter
        
        //start the root
        std::vector< std::function< void() > > pythonFunctions = 
            loadPythonScripts( scripts, strDatabaseFile );
        
        root_starter( pythonFunctions );
        
        boost::this_fiber::properties< eg::fiber_props >().setTimeKeeper();
        
        HostClock::Tick cycleStart = theClock.actual();
        while( boost::this_fiber::properties< eg::fiber_props >().shouldContinue() )
        {
            const HostClock::TickDuration elapsed = theClock.actual() - cycleStart;
            if( elapsed < sleepDuration )
            {
                boost::this_fiber::sleep_for( sleepDuration - elapsed );
            }
            theClock.nextCycle();
            eg::wait();
        }
        
        deallocate_buffers();
    }
    catch( std::exception& e )
    {
        std::cerr << "Error: " << e.what() << std::endl;
        deallocate_buffers();
    }
    
    return 0;
    
}
    
    
