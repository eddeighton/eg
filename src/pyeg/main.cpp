
#include "py_eg_reference.hpp"

#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <pybind11/embed.h> // everything needed for embedding

#include <iostream>
#include <memory>

//export PYTHONHOME=/c/Anaconda3/
//ipython console --existing kernel-6600.json

struct Modules
{
    pybind11::module module_eg_host;
    pybind11::module module_eg;
    std::shared_ptr< eg::PythonEGReferenceType > pEGRefType;
};
std::unique_ptr< Modules > g_pModules;


struct RootReferenceCPPType
{
    __eg_reference instance;
};





namespace pybind11 { namespace detail {
    template <> struct type_caster< RootReferenceCPPType > 
    {
    public:
        /**
         * This macro establishes the name 'inty' in
         * function signatures and declares a local variable
         * 'value' of type inty
         */
        PYBIND11_TYPE_CASTER( RootReferenceCPPType, _("eg.reference"));

        /**
         * Conversion part 1 (Python->C++): convert a PyObject into a inty
         * instance or return false upon failure. The second argument
         * indicates whether implicit conversions should be applied.
         */
        bool load( handle src, bool ) 
        {
            /* Extract PyObject from handle */
            PyObject* pSource = src.ptr();
            
            const eg::PythonEGReference* pEGReference = 
                eg::PythonEGReferenceType::getReference( pSource );
            
            value.instance = pEGReference->getEGReference();
            return true;
            
            ///* Try converting into a Python integer value */
            //PyObject *tmp = PyNumber_Long(source);
            //if (!tmp)
            //    return false;
            ///* Now try to convert into a C++ int */
            //value.long_value = PyLong_AsLong(tmp);
            //Py_DECREF(tmp);
            ///* Ensure return code was OK (to avoid out-of-range errors etc) */
            //return !(value.long_value == -1 && !PyErr_Occurred());
        }

        /**
         * Conversion part 2 (C++ -> Python): convert an inty instance into
         * a Python object. The second and third arguments are used to
         * indicate the return value policy and parent object (for
         * ``return_value_policy::reference_internal``) and are generally
         * ignored by implicit casters.
         */
        static handle cast( RootReferenceCPPType src, return_value_policy /* policy */, handle /* parent */) 
        {
            return g_pModules->pEGRefType->create( src.instance );
            
            //return PyLong_FromLong(src.long_value);
        }
    };
}} // namespace pybind11::detail











std::string testHostFunction( RootReferenceCPPType instance )
{
    std::ostringstream os;
    os << "instance is: " << instance.instance.instance;
    return os.str();
}

PYBIND11_EMBEDDED_MODULE( eg_host, module ) 
{
    // `m` is a `py::module` which is used to bind functions and classes
    module.def( "testHostFunction", testHostFunction );
}



static int numargs=0;

/* Return the number of arguments of the application command line */
static PyObject* eg_root(PyObject *self, PyObject *pArgs)
{
    //if(!PyArg_ParseTuple(args, ":numargs"))
    //    return NULL;


    //g_pModules->module_eg
    __eg_reference egReference = { 0,0,0 };
    
    return g_pModules->pEGRefType->create( egReference );
    
    //pybind11::module host = g_pModules->module_eg;
    //
    //pybind11::function theFunction = host.attr( "testHostFunction" );
    //
    //pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pArgs );
    //
    //pybind11::object result = theFunction( *args );
    //
    //return result.ptr();
    
    //return PyLong_FromLong(numargs);
}

static PyMethodDef EGMethods[] = 
{
    {"root", eg_root, METH_VARARGS, "Get the root action."},
    {NULL, NULL, 0, NULL}
};

static PyModuleDef EGModule = 
{
    PyModuleDef_HEAD_INIT, "eg", NULL, -1, EGMethods,
    NULL, NULL, NULL, NULL
};

static PyObject* PyInit_eg(void)
{
    return PyModule_Create( &EGModule );
}




int main( int argc, const char* argv[] )
{
    bool bDebug = false;
    std::string strPythonFile;
    
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
        std::cout << "Missing pythong file" << std::endl;
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

    
    PyImport_AppendInittab( "eg", &PyInit_eg );
    
    
    pybind11::scoped_interpreter guard{}; // start the interpreter and keep it alive

    
    g_pModules = std::make_unique< Modules >();
    
    g_pModules->module_eg_host  = pybind11::module::import( "eg_host" );
    g_pModules->module_eg       = pybind11::module::import( "eg" );
    g_pModules->pEGRefType      = std::make_shared< eg::PythonEGReferenceType >();
    
    //pybind11::module iPythonModule = pybind11::module::import( "IPython" );
   
    //pybind11::print("Starting IPython Kernel"); 
    //pybind11::eval( "print( \"Starting the IPython Kernel\" ) " );
    
 
    pybind11::exec( strScript );
    
    g_pModules.reset();
}

