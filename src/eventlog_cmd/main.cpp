


#include "eg/implementation_session.hpp"

#include "runtime/eg_common.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/tick.hpp"

#include "eventlog/client.hpp"
#include "eventlog/server.hpp"

#include <boost/program_options.hpp>

#include <windows.h>

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <thread>

struct CmdLine
{
    boost::filesystem::path eventLogPath;
    boost::filesystem::path databasePath;
    std::string strHostPID;
    std::vector< std::string > filters;
    bool bClearLog = false;
    bool bDebug = false;
    
    boost::program_options::options_description m_options;
    boost::program_options::variables_map m_variables;
    
    CmdLine()
        :   m_options( "Allowed options" )
    {
        
    }

    bool parse_args( int argc, char* argv[] )
    {
        namespace po = boost::program_options;
        try
        {
            m_options.add_options()
                ("help", "produce help message")
                ("log",     po::value< boost::filesystem::path >( &eventLogPath ),      "Event Log Path" )
                ("data",    po::value< boost::filesystem::path >( &databasePath ),      "Database Path" )
                ("filters", po::value< std::vector< std::string > >( &filters ),        "Filters ( default parameter )" )
                ("host",    po::value< std::string >( &strHostPID ),                    "Host PID" )
                ("clear",   po::value< bool >( &bClearLog )->implicit_value( true ),    "Test and clear existing log" )
                ("debug",   po::value< bool >( &bDebug )->implicit_value( true ),       "Wait at startup to attach debugger" )
            ;

            po::positional_options_description p;
            p.add( "filters", -1 );

            po::store( po::command_line_parser( argc, argv).
                        options( m_options ).
                        positional( p ).run(),
                        m_variables );
            po::notify( m_variables );
            
            if( m_variables.count("help") )
            {
                std::cout << m_options << "\n";
                return false;
            }
            
            if( eventLogPath.empty() )
            {
                std::cout << "Missing event log path" << std::endl;
                return false;
            }

        }
        catch( std::exception& e )
        {
            std::cout << "Invalid input. Type '--help' for options\n" << e.what() << "\n";
            return false;
        }
        
        return true;
    }
    
    bool keep( const char* pszEventType ) const
    {
        bool bKeep = true;
        if( !filters.empty() )
        {
            bKeep = false;
            for( const std::string& str : filters )
            {
                if( str == pszEventType )
                {
                    bKeep = true;
                    break;
                }
            }
        }
        return bKeep;
    }
};


void printEvent( const CmdLine& cmdLine, std::shared_ptr< eg::ReadSession > pDatabase, 
        const IPC::Event::Event& event, std::ostream& os )
{
    HANDLE hConsole = GetStdHandle( STD_OUTPUT_HANDLE );

    
    if( 0 == strcmp( event.getType_c_str(), "start" ) )
    {
        if( cmdLine.keep( event.getType_c_str() ) )
        {
            SetConsoleTextAttribute( hConsole, FOREGROUND_GREEN );
            
            VERIFY_RTE( sizeof( eg::reference ) == event.getValueSize() );
            const eg::reference* pRef = reinterpret_cast< const eg::reference* >( event.getValue() );
            
            if( pDatabase )
            {
                const eg::concrete::Action* pAction = pDatabase->getConcreteAction( pRef->type );
                os << "start " << pAction->getName() << " " << pRef->instance << " " << pRef->timestamp << std::endl;
            }
            else
            {
                os << "start " << pRef->type << " " << pRef->instance << " " << pRef->timestamp << std::endl;
            }
        }
    }
    else if( 0 == strcmp( event.getType_c_str(), "stop" ) )
    {
        if( cmdLine.keep( event.getType_c_str() ) )
        {
            SetConsoleTextAttribute( hConsole, FOREGROUND_BLUE );
            
            VERIFY_RTE( sizeof( eg::reference ) == event.getValueSize() );
            const eg::reference* pRef = reinterpret_cast< const eg::reference* >( event.getValue() );
            
            if( pDatabase )
            {
                const eg::concrete::Action* pAction = pDatabase->getConcreteAction( pRef->type );
                os << "stop  " << pAction->getName() << " " << pRef->instance << " " << pRef->timestamp << std::endl;
            }
            else
            {
                os << "stop  " << pRef->type << " " << pRef->instance << " " << pRef->timestamp << std::endl;
            }
        }
    }
    else if( 0 == strcmp( event.getType_c_str(), "log" ) )
    {
        if( cmdLine.keep( event.getType_c_str() ) )
        {
            SetConsoleTextAttribute( hConsole, FOREGROUND_RED );
            os << "log   " << reinterpret_cast< const char* >( event.getValue() ) << std::endl;
        }
    }
    else if( 0 == strcmp( event.getType_c_str(), "error" ) )
    {
        if( cmdLine.keep( event.getType_c_str() ) )
        {
            SetConsoleTextAttribute( hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY );
            os << "error : " << reinterpret_cast< const char* >( event.getValue() ) << std::endl;
        }
    }
    else if( 0 == strcmp( event.getType_c_str(), "pass" ) )
    {
        if( cmdLine.keep( event.getType_c_str() ) )
        {
            SetConsoleTextAttribute( hConsole, FOREGROUND_GREEN | FOREGROUND_INTENSITY );
            os << "pass  : " << reinterpret_cast< const char* >( event.getValue() ) << std::endl;
        }
    }
    else if( 0 == strcmp( event.getType_c_str(), "fail" ) )
    {
        if( cmdLine.keep( event.getType_c_str() ) )
        {
            SetConsoleTextAttribute( hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY );
            os << "fail  : " << reinterpret_cast< const char* >( event.getValue() ) << std::endl;
        }
    }
    else
    {
        if( cmdLine.keep( event.getType_c_str() ) )
        {
            SetConsoleTextAttribute( hConsole, FOREGROUND_BLUE | FOREGROUND_INTENSITY );
            os << "unknown event type: " << event.getType_c_str() << std::endl;
        }
    }
}
    
int mainImpl( int argc, char* argv[] )
{
    CmdLine cmdLine;
    if( !cmdLine.parse_args( argc, argv ) )
        return 0;
    
    if( cmdLine.bDebug )
    {
        char c;
        std::cin >> c;
    }
    
    if( cmdLine.bClearLog )
    {
        IPC::PID pid;
        IPC::Event::Server server( pid, cmdLine.eventLogPath );
    }
    
    std::shared_ptr< eg::ReadSession > pDatabase;
    if( !cmdLine.databasePath.empty() )
    {
        pDatabase = std::make_shared< eg::ReadSession >( cmdLine.databasePath );
    }
    
    if( cmdLine.strHostPID.empty() )
    {
        IPC::Event::Log eventLog( cmdLine.eventLogPath );
        
        IPC::Event::Iterator iter = 0U;
        IPC::Event::Event event;
        while( eventLog.read( iter, event ) )
        {
            printEvent( cmdLine, pDatabase, event, std::cout );
        }
        
    }
    else
    {
        std::cout << "Connecting to host: " << cmdLine.strHostPID << std::endl;
        IPC::PID pidHost = cmdLine.strHostPID;
        IPC::Event::Client eventLog( pidHost, cmdLine.eventLogPath );
        
        Timing::UpdateTick tick;
        IPC::Event::Iterator iter = 0U;
    
        while( true )
        {
            IPC::Event::Event event;
            while( eventLog.read( iter, event ) )
            {
                printEvent( cmdLine, pDatabase, event, std::cout );
            }
            
            if( tick.elapsed( 1.0f ) )
            {
                tick.update();
                //std::cout << "Event Log Head at: " << eventLog.head() << std::endl;
            }
            
            std::this_thread::sleep_for( 
                std::chrono::milliseconds( 200 ) );
        }
    }
    
    return 0;
}

int main( int argc, char* argv[] )
{
    try
    {
        return mainImpl( argc, argv );
    }
    catch( std::exception& e )
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    catch( ... )
    {
        std::cout << "Unknown exception encountered" << std::endl;
        return -1;
    }

    return 0;
}
