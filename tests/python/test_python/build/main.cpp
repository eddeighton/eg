#include "structures.hpp"

extern std::vector< std::function< void() > > loadPythonScripts( const std::vector< std::string >& scripts, const std::string& strDatabaseFile );
std::vector< std::function< void() > > loadScripts( const std::vector< std::string >& scripts, const std::string& strDatabase )
{
    return loadPythonScripts( scripts, strDatabase );
}
    
//buffers
static std::unique_ptr< std::array< b_root, 1 > > g_root_allocation;
b_root* g_root = nullptr;
static std::unique_ptr< std::array< b_root_Test, 1 > > g_root_Test_allocation;
b_root_Test* g_root_Test = nullptr;
static std::unique_ptr< std::array< b_root_Python, 2 > > g_root_Python_allocation;
b_root_Python* g_root_Python = nullptr;
void allocate_buffers()
{
    g_root_allocation = std::make_unique< std::array< b_root, 1 > >();
    g_root = g_root_allocation->data();
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root[ i ].g_root_cycle = eg::INVALID_TIMESTAMP;
        g_root[ i ].g_root_state = ::eg::action_stopped;
        g_root[ i ].g_root_reference = __eg_root< void >( eg::reference { i, 25, 0 } );
        g_root[ i ].g_root_ring_index = i;
        g_root[ i ].g_rootTest_ring_iter = 0UL;
        g_root[ i ].g_rootPython_ring_iter = 0UL;
    }
    g_root_Test_allocation = std::make_unique< std::array< b_root_Test, 1 > >();
    g_root_Test = g_root_Test_allocation->data();
    for( eg::Instance i = 0U; i != 1; ++i )
    {
        g_root_Test[ i ].g_root_Test_cycle = eg::INVALID_TIMESTAMP;
        g_root_Test[ i ].g_root_Test_state = ::eg::action_stopped;
        g_root_Test[ i ].g_root_Test_reference = __eg_root< void >::__eg_Test< void >( eg::reference { i, 32, 0 } );
        g_root_Test[ i ].g_root_Test_ring_index = i;
        g_root_Test[ i ].g_root_Test_ring = i;
    }
    g_root_Python_allocation = std::make_unique< std::array< b_root_Python, 2 > >();
    g_root_Python = g_root_Python_allocation->data();
    for( eg::Instance i = 0U; i != 2; ++i )
    {
        g_root_Python[ i ].g_root_Python_cycle = eg::INVALID_TIMESTAMP;
        g_root_Python[ i ].g_root_Python_state = ::eg::action_stopped;
        g_root_Python[ i ].g_root_Python_reference = __eg_root< void >::__eg_Python< void >( eg::reference { i, 38, 0 } );
        g_root_Python[ i ].g_root_Python_ring_index = i;
        ::eg::DimensionTraits< int >::initialise( g_root_Python[ i ].m_bContinue);
        g_root_Python[ i ].g_root_Python_ring = i;
    }
}

void deallocate_buffers()
{
    for( eg::Instance i = 0U; i != 2; ++i )
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


    
int main( int argc, const char* argv[] )
{
    std::string strDatabaseFile;
    std::vector< std::string > scripts;
    int iMilliseconds = 0;
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
                ("script",      po::value< std::vector< std::string > >( &scripts ), "Scripts" )
                ("rate",        po::value< int >( &iMilliseconds ), "Simulation rate in milliseconds" )
            ;

            po::positional_options_description p;
            p.add( "script", -1 );

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
        const float sleepDuration_sec = 
            std::chrono::duration< float, std::ratio< 1 > >(
                std::chrono::milliseconds( iMilliseconds ) ).count();
        
        //allocate everything
        allocate_buffers();
        
        //be sure to initialise the clock before the scheduler
        boost::fibers::use_scheduling_algorithm< eg::eg_algorithm >();
    
        //start the root
        std::vector< std::function< void() > > scriptFunctions = 
            loadScripts( scripts, strDatabaseFile );
        
        root_starter( scriptFunctions );
        
        boost::this_fiber::properties< eg::fiber_props >().setTimeKeeper();
        
        float cycleStart = clock::ct();
        while( boost::this_fiber::properties< eg::fiber_props >().shouldContinue() )
        {
            const float elapsed = clock::ct() - cycleStart;
            if( elapsed < sleepDuration_sec )
            {
                auto floatDuration      = std::chrono::duration< float, std::ratio< 1 > >( sleepDuration_sec - elapsed );
                auto intMilliseconds    = std::chrono::duration_cast< std::chrono::milliseconds >( floatDuration );
                std::this_thread::sleep_for( intMilliseconds );
            }
            eg::wait();
            
            clock::next();
            cycleStart = clock::ct();
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
    
    
