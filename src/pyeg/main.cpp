
#include "eg/implementation_session.hpp"

#include "runtime/py_eg_reference.hpp"
#include "runtime/py_glm.hpp"
#include "eg_runtime/eg_runtime.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>

#include "cinder/include/glm/glm.hpp"

#include <pybind11/embed.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <iostream>
#include <memory>
#include <array>
#include <thread>
#include <chrono>
#include <functional>

//export PYTHONHOME=/c/Anaconda3/
//ipython console --existing kernel-6600.json

struct Modules
{
    pybind11::module module_eg;
    std::shared_ptr< eg::EGRuntime > pRuntime;
    std::shared_ptr< eg::PythonEGReferenceType > pEGRefType;
};
std::unique_ptr< Modules > g_pModules;


struct RootReferenceCPPType
{
    eg::reference instance;
};

namespace pybind11 
{ 
    namespace detail 
    {
        template <> struct type_caster< RootReferenceCPPType > 
        {
        public:
            PYBIND11_TYPE_CASTER( RootReferenceCPPType, _("pyeg.reference"));

            bool load( handle src, bool ) 
            {
                const eg::PythonEGReference* pEGReference = 
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.instance = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
            
            static handle cast( RootReferenceCPPType src, return_value_policy /* policy */, handle /* parent */) 
            {
                return g_pModules->pEGRefType->create( src.instance );
                
                //return PyLong_FromLong(src.long_value);
            }
        };
    }   //namespace detail
} // namespace pybind11








//PYBIND11_MAKE_OPAQUE( StuffArray );


struct Stuff
{
    glm::vec3 v3;
    float x = 0.0f,
        y = 0.0f,
        z = 0.0f;
    int thing = 0;
};
using StuffArray = std::array< Stuff, 16 >;

StuffArray m_buffer;


float read_x( eg::Instance instance )
{
    return m_buffer[ instance ].x;
}
void write_x( eg::Instance instance, pybind11::tuple args )
{
    m_buffer[ instance ].x = pybind11::cast< float >( args[ 0 ] );
}

glm::vec3 read_v3( eg::Instance instance )
{
    return m_buffer[ instance ].v3;
}
void write_v3( eg::Instance instance, glm::vec3 v3 )
{
    m_buffer[ instance ].v3 = v3;
}


std::string testHostFunction( RootReferenceCPPType instance )
{
    std::ostringstream os;
    os << "instance is: " << instance.instance.instance;
    return os.str();
}

RootReferenceCPPType root()
{
    //g_pModules->module_eg
    //eg::reference egReference = { 0,0,0 };
    //return g_pModules->pEGRefType->create( egReference );
    
    RootReferenceCPPType root;
    root.instance = eg::reference{ 0, 4, 0 };
    return root;
}

PYBIND11_EMBEDDED_MODULE( pyeg, module ) 
{
    PYBIND11_NUMPY_DTYPE( glm::vec3, x, y, z );
    
    // `m` is a `py::module` which is used to bind functions and classes
    module.def( "testHostFunction", testHostFunction );
    module.def( "root", root );
    
    module.def( "read_x", read_x );
    module.def( "write_x", write_x );
    
    module.def( "read_v3", read_v3 );
    module.def( "write_v3", write_v3 );
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
    virtual void doRead(    const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            switch( reference.type )
            {
                case 5:
                    {
                        pStack->m_result = m_module_eg.attr( "read_x" )( reference.instance );
                    }
                    break;
                default:
                    break;
            }
        }
        
    }   
    virtual void doWrite(   const eg::reference& reference )
    {
        if( Stack::SharedPtr pStack = m_pStack.lock() )
        {
            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
            switch( reference.type )
            {
                case 5:
                    {
                        pStack->m_result = m_module_eg.attr( "write_x" )( reference.instance, args );
                    }
                    break;
                default:
                    break;
            }
        }
    }
    virtual void doStart(   const eg::reference& reference )
    {
    }
    virtual void doStop(    const eg::reference& reference )
    {
    }
    virtual void doPause(   const eg::reference& reference )
    {
    }
    virtual void doResume(  const eg::reference& reference )
    {
    }
    virtual void doEmpty(   const eg::reference& reference )
    {
    }
    
    virtual eg::reference getReference( const eg::reference& dimension )
    {
        return dimension;
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
        else
        {
            Py_INCREF( Py_None );
            return Py_None;
        }
    }
    
private:
    pybind11::module m_module_eg;
    Stack::WeakPtr m_pStack;
    
    std::shared_ptr< eg::EGRuntime > m_pRuntime;
    
};

void runPython( const std::string& strDatabaseFile, const std::string& strScript )
{
    pybind11::scoped_interpreter guard{}; // start the interpreter and keep it alive

    try
    {
        g_pModules = std::make_unique< Modules >();
        
        g_pModules->module_eg = pybind11::module::import( "pyeg" );
        
        //g_pModules->module_eg = pybind11::module::import( "IPython" );
        //pybind11::print("Starting IPython Kernel"); 
        
        HostFunctions hostFunctions( strDatabaseFile, g_pModules->module_eg );
        
        g_pModules->pEGRefType
            = std::make_shared< eg::PythonEGReferenceType >( hostFunctions );
        
        pybind11::exec( strScript );
        
        g_pModules.reset();
        
    }
    catch( std::exception& e )
    {
        std::cout << e.what() << std::endl;
        g_pModules.reset();
    }
}

int main( int argc, const char* argv[] )
{
    bool bDebug = false;
    std::string strPythonFile;
    std::string strDatabaseFile;
    
    {
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
                ("python",      po::value< std::string >( &strPythonFile ), "Python file to run" )
                ("database",    po::value< std::string >( &strDatabaseFile ), "Python file to run" )
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
    }
    
    if( bDebug )
    {
        char c;
        std::cin >> c;
    }
    
    if( strPythonFile.empty() )
    {
        std::cout << "Missing python file" << std::endl;
        return 0;
    }
    
    if( strDatabaseFile.empty() )
    {
        std::cout << "Missing database file" << std::endl;
        return 0;
    }
    
    
    std::string strScript;
    {
        const boost::filesystem::path pythonFilePath = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strPythonFile ) );
        if( !boost::filesystem::exists( pythonFilePath ) )
        {
            std::cout << "Cannot locate file: " << pythonFilePath.string() << std::endl;
            return 0;
        } 
        boost::filesystem::loadAsciiFile( pythonFilePath, strScript );
    }

    std::thread pythonThread( std::bind( &runPython, strDatabaseFile, strScript ) );
    
    using namespace std::chrono_literals;
    std::this_thread::sleep_for( 10s );
    
    //PyImport_AppendInittab( "eg", &PyInit_eg );
    
    
}

