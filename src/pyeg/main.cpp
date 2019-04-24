
#include "eg/implementation_session.hpp"

#include "py_eg_reference.hpp"
#include "py_glm.hpp"
#include "runtime.hpp"


#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>

#include "cinder/include/glm/glm.hpp"

#include <pybind11/embed.h> // everything needed for embedding
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include <iostream>
#include <memory>
#include <array>

//export PYTHONHOME=/c/Anaconda3/
//ipython console --existing kernel-6600.json

struct Modules
{
    pybind11::module module_eg;
    std::shared_ptr< eg::PythonEGReferenceType > pEGRefType;
};
std::unique_ptr< Modules > g_pModules;


struct RootReferenceCPPType
{
    __eg_reference instance;
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


float read_x( EGInstance instance )
{
    return m_buffer[ instance ].x;
}
void write_x( EGInstance instance, pybind11::tuple args )
{
    m_buffer[ instance ].x = pybind11::cast< float >( args[ 0 ] );
}

glm::vec3 read_v3( EGInstance instance )
{
    return m_buffer[ instance ].v3;
}
void write_v3( EGInstance instance, glm::vec3 v3 )
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
    //__eg_reference egReference = { 0,0,0 };
    //return g_pModules->pEGRefType->create( egReference );
    
    RootReferenceCPPType root;
    root.instance = __eg_reference{ 0, 4, 0 };
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

struct HostFunctions : public eg::HostFunctionAccessor
{
    virtual pybind11::function getRead( EGTypeID type )
    {
        switch( type )
        {
            case 0:
            default:
                break;
        }
        pybind11::object object = g_pModules->module_eg.attr( "read_x" );
        return object;
    }
    virtual pybind11::function getWrite( EGTypeID type )
    {
        switch( type )
        {
            case 0:
            default:
                break;
        }
        pybind11::object object = g_pModules->module_eg.attr( "write_x" );
        return object;
    }
    virtual pybind11::function getStart( EGTypeID type )
    {
        switch( type )
        {
            case 0:
            default:
                break;
        }
        pybind11::object object = g_pModules->module_eg.attr( "start_thing" );
        return object;
    }
    virtual pybind11::function getStop( EGTypeID type )
    {
        switch( type )
        {
            case 0:
            default:
                break;
        }
        pybind11::object object = g_pModules->module_eg.attr( "stop_thing" );
        return object;
    }
    virtual pybind11::function getPause( EGTypeID type )
    {
        pybind11::object object = g_pModules->module_eg.attr( "stop_thing" );
        return object;
    }
    virtual pybind11::function getResume( EGTypeID type )
    {
        pybind11::object object = g_pModules->module_eg.attr( "stop_thing" );
        return object;
    }
    virtual pybind11::function getEmpty( EGTypeID type )
    {
        pybind11::object object = g_pModules->module_eg.attr( "stop_thing" );
        return object;
    }
    virtual __eg_reference getReference( const __eg_reference& dimension )
    {
        
        
        switch( dimension.type )
        {
            //can just evaluate this directly
        }
        
        
        
        
        return dimension;
    }
};

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
    
    std::shared_ptr< eg::ReadSession > pDatabase;
    {
        const boost::filesystem::path databaseFilePath = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDatabaseFile ) );
        if( !boost::filesystem::exists( databaseFilePath ) )
        {
            std::cout << "Cannot locate file: " << databaseFilePath.string() << std::endl;
            return 0;
        } 
        pDatabase = std::make_shared< eg::ReadSession >( databaseFilePath );
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

    
    
    //PyImport_AppendInittab( "eg", &PyInit_eg );
    
    {
        pybind11::scoped_interpreter guard{}; // start the interpreter and keep it alive
    
    
        try
        {
            g_pModules = std::make_unique< Modules >();
            
            g_pModules->module_eg = pybind11::module::import( "pyeg" );
            
            HostFunctions hostFunctions;
            
            eg::EGRuntime runtime( hostFunctions, pDatabase );
        
            g_pModules->pEGRefType
                = std::make_shared< eg::PythonEGReferenceType >( runtime );
            
            //pybind11::print("Starting IPython Kernel"); 
            
            pybind11::exec( strScript );
            
            g_pModules.reset();
            
        }
        catch( std::exception& e )
        {
            std::cout << e.what() << std::endl;
            g_pModules.reset();
        }
    }
}

