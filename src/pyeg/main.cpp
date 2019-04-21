


#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <pybind11/embed.h> // everything needed for embedding

#include <iostream>

//export PYTHONHOME=/c/Anaconda3/
//ipython console --existing kernel-6600.json

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

    pybind11::scoped_interpreter guard{}; // start the interpreter and keep it alive

    pybind11::module iPythonModule = pybind11::module::import( "IPython" );
   
    //pybind11::print("Starting IPython Kernel"); 
    //pybind11::eval( "print( \"Starting the IPython Kernel\" ) " );
 
    pybind11::exec( strScript );
    
}

