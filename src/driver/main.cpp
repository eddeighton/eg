//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"

#include "eg_compiler/sessions/parser_session.hpp"
#include "eg_compiler/sessions/interface_session.hpp"
#include "eg_compiler/sessions/operations_session.hpp"
#include "eg_compiler/sessions/implementation_session.hpp"
#include "eg_compiler/codegen/codegen.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include "log.hpp"

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

#pragma warning( push )
#pragma warning( disable : 4996) //iterator thing
#pragma warning( disable : 4244) //conversion to DWORD from system_clock::rep
#include "boost/process.hpp"
#pragma warning( pop )

#include <cstdlib>

#include <iostream>
#include <iomanip>
#include <bitset>


extern void command_create( bool bHelp, const std::vector< std::string >& args );
extern void command_build( bool bHelp, const std::string& strBuildCommand, const std::vector< std::string >& args );
extern void command_run( bool bHelp, const std::string& strRunCommand, const std::vector< std::string >& args );
extern void command_log( bool bHelp, const std::vector< std::string >& args );
extern void command_clean( bool bHelp, const std::vector< std::string >& args );
extern void command_info( bool bHelp, const std::vector< std::string >& args );
extern void command_doc( bool bHelp, const std::vector< std::string >& args );

enum MainCommand
{
    eCmd_Create,
    eCmd_Build,
    eCmd_Run,
    eCmd_Log,
    eCmd_Clean,
    eCmd_Info,
    eCmd_Doc,
    TOTAL_MAIN_COMMANDS
};

int main_wrapper( int argc, const char* argv[] )
{
    boost::timer::cpu_timer timer;
    
    bool bWait = false;
    
    std::bitset< TOTAL_MAIN_COMMANDS > cmds;
    MainCommand cmd = TOTAL_MAIN_COMMANDS;
    
    //commands
    std::string strBuildCommand, strRunCommand;
    
    {
        std::vector< std::string > commandArgs;
    
        namespace po = boost::program_options;
        po::variables_map vm;
        try
        {
            bool bGeneralWait = false;
            
            bool bCmdCreate = false;
            bool bCmdLog    = false;
            bool bCmdClean  = false;
            bool bCmdInfo   = false;
            bool bCmdDoc    = false;
            
            po::options_description genericOptions(" General");
            {
                genericOptions.add_options()
                
                    ("help", "Produce general or command help message")
                    ("wait", po::bool_switch( &bGeneralWait ), "Wait at startup for attaching a debugger" )
                ;
            }
            
            po::options_description commandOptions(" Commands");
            {
                
                commandOptions.add_options()
                        
                    ("create",  po::bool_switch( &bCmdCreate ),
                        "Start a new eg project" )
                        
                    ("build", po::value< std::string >( &strBuildCommand )->implicit_value("release"), 
                        "Build an eg project" )
                        
                    ("run",   po::value< std::string >( &strRunCommand )->implicit_value("default"),
                        "Run an eg project" )
                        
                    ("log",   po::bool_switch( &bCmdLog ),
                        "Run an eg project" )
                        
                    ("clean",   po::bool_switch( &bCmdClean ),
                        "Removing all build and log files" )
                        
                    ("info",   po::bool_switch( &bCmdInfo ),
                        "Report info about an eg project" )
                        
                    ("doc",    po::bool_switch( &bCmdDoc ),
                        "Generate sphinx rst documentation" )
                        
                    //("cmake",   po::bool_switch( &bCmdCMake ),
                    //    "Generate a cmake build for an eg project" )
                        
                    //("debug",   po::bool_switch( &bCmdDebug ),
                    //    "Debug an eg project using cmake based build system" )
                ;
            }
            
            if( cmds.count() > 1 )
            {
                SPDLOG_ERROR( "Invalid command combination. Type '--help' for options\n" );
                return 1;
            }
            
            po::options_description commandHiddenOptions( "" );
            {
                commandHiddenOptions.add_options()
                    ( "args", po::value< std::vector< std::string > >( &commandArgs ) )
                    ;
            }

            po::options_description visibleOptions( "Allowed options" );
            visibleOptions.add( genericOptions ).add( commandOptions );
            
            po::options_description allOptions( "all" );
            allOptions.add( genericOptions ).add( commandOptions ).add( commandHiddenOptions );

            po::positional_options_description p;
            p.add( "args", -1 );
            
            po::parsed_options parsedOptions =
                po::command_line_parser( argc, argv ).
                            options( allOptions ).
                            positional( p ).
                            allow_unregistered().
                            run();
            
            po::store( parsedOptions, vm );
            po::notify( vm );
            
            if( bGeneralWait )
            {
                SPDLOG_INFO( "Waiting for input..." );
                char c;
                std::cin >> c;
            }
            
            if( bCmdCreate )
            {
                cmds.set( eCmd_Create );
                cmd = eCmd_Create;
            }
            if( !strBuildCommand.empty() )
            {
                cmds.set( eCmd_Build );
                cmd = eCmd_Build;
            }
            if( !strRunCommand.empty() )
            {
                cmds.set( eCmd_Run );
                cmd = eCmd_Run;
            }
            if( bCmdLog )
            {
                cmds.set( eCmd_Log );
                cmd = eCmd_Log;
            }
            if( bCmdClean ) 
            {
                cmds.set( eCmd_Clean );
                cmd = eCmd_Clean;
            }
            if( bCmdInfo ) 
            {
                cmds.set( eCmd_Info );
                cmd = eCmd_Info;
            }
            if( bCmdDoc )
            {
                cmds.set( eCmd_Doc );
                cmd = eCmd_Doc;
            }
            //if( bCmdCMake )                 cmds.set( eCmd_Create );
            //if( bCmdDebug )                 cmds.set( eCmd_Create );
            
            const bool bShowHelp = vm.count("help");
            
            std::vector< std::string > commandArguments = 
                po::collect_unrecognized( parsedOptions.options, po::include_positional );
                
            switch( cmd )
            {                
                case eCmd_Create          : command_create( bShowHelp, commandArguments );                   break;
                case eCmd_Build           : command_build(  bShowHelp, strBuildCommand, commandArguments );  break;
                case eCmd_Run             : command_run(    bShowHelp, strRunCommand, commandArguments );    break;
                case eCmd_Log             : command_log(    bShowHelp, commandArguments );                   break;
                case eCmd_Clean           : command_clean(  bShowHelp, commandArguments );                   break;
                case eCmd_Info            : command_info(   bShowHelp, commandArguments );                   break;
                case eCmd_Doc             : command_doc(    bShowHelp, commandArguments );                   break;
                case TOTAL_MAIN_COMMANDS  :
                default:
                    if( vm.count( "help" ) )
                    {
                        std::ostringstream os;
                        os << visibleOptions;
                        SPDLOG_INFO( os.str() );
                    }
                    else
                    {
                        SPDLOG_ERROR( "Invalid command. Type '--help' for options\n" );
                        return 1;
                    }
            }
            return 0;
        }
        catch( boost::program_options::error& e )
        {
            SPDLOG_ERROR( "Invalid input. {}. Type '--help' for options", e.what() );
            return 1;
        }
        catch( const xml_schema::parser_exception& e )
        {
            SPDLOG_ERROR( "{} : {} : {} : {}", e.what(), e.line(), e.column(), e.text() );
            return 1;
        }
        catch( std::exception& e )
        {
            SPDLOG_ERROR( "Error: {}", e.what() );
            return 1;
        }
    }
    
    return 0;
}


int main( int argc, const char* argv[] )
{
    auto logThreadPool = eg::configureLog( boost::filesystem::current_path(), "build" );
    
    main_wrapper( argc, argv );
    
    spdlog::drop_all(); 
}