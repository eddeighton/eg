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

#include "eg/parser_session.hpp"
#include "eg/interface_session.hpp"
#include "eg/operations_session.hpp"
#include "eg/implementation_session.hpp"
#include "eg/codegen.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/Diagnostic.h"

#pragma warning( pop ) 

#include "common/assert_verify.hpp"
#include "common/file.hpp"


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


class EGDiagConsumer : public clang::DiagnosticConsumer 
{
    virtual void anchor()
    {
    }

    void HandleDiagnostic( clang::DiagnosticsEngine::Level DiagLevel,
                        const clang::Diagnostic &Info) override 
    {
        llvm::SmallString< 100 > msg;
        Info.FormatDiagnostic( msg );
        std::string str = msg.str();
        std::cout << str << std::endl;
    }
};

extern void command_create( bool bHelp, const std::vector< std::string >& args );

extern void command_build( bool bHelp, const std::string& strBuildCommand, const std::vector< std::string >& args );

extern void command_run( bool bHelp, const std::vector< std::string >& args );

extern void command_log( bool bHelp, const std::vector< std::string >& args );

void command_cmake( bool bHelp, const std::vector< std::string >& args )
{
    
}
void command_debug( bool bHelp, const std::vector< std::string >& args )
{
    
}

int main( int argc, const char* argv[] )
{
    boost::timer::cpu_timer timer;
    
    bool bWait = false;
    
    //commands
    bool bCmdCreate = false;
    //bool bCmdBuild  = false;
    std::string strBuildCommand;
    bool bCmdRun    = false;
    bool bCmdLog    = false;
    bool bCmdCMake  = false;
    bool bCmdDebug  = false;
    
    {
        std::vector< std::string > commandArgs;
    
        namespace po = boost::program_options;
        po::variables_map vm;
        try
        {
            po::options_description genericOptions(" General");
            {
                genericOptions.add_options()
                    ("help", "produce general or command help message")
                ;
            }
            
            po::options_description commandOptions(" Commands");
            {
                commandOptions.add_options()
                        
                    ("create",  po::bool_switch( &bCmdCreate ),
                        "Start a new eg project" )
                        
                    ("build", po::value< std::string >( &strBuildCommand )->implicit_value("release"), 
                        "Build an eg project with the specified build command" )
                        
                    ("run",   po::bool_switch( &bCmdRun ),
                        "Run an eg project" )
                        
                    ("log",   po::bool_switch( &bCmdLog ),
                        "Run an eg project" )
                        
                    ("cmake",   po::bool_switch( &bCmdCMake ),
                        "Generate a cmake build for an eg project" )
                        
                    ("debug",   po::bool_switch( &bCmdDebug ),
                        "Debug an eg project using cmake based build system" )
                        
                ;
            }
            
            po::options_description commandHiddenOptions( "" );
            {
                commandHiddenOptions.add_options()
                    ( "args", po::value< std::vector< std::string > >( &commandArgs ) )
                    //( "wait",   po::bool_switch( &bWait ) )
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
            
            if( bWait )
            {
                char c;
                std::cin >> c;
            }
            
            const bool bShowHelp = vm.count("help");
            
            std::vector< std::string > commandArguments = 
                po::collect_unrecognized( parsedOptions.options, po::include_positional );
            
            if( bCmdCreate )
            {
                if( !strBuildCommand.empty() || bCmdRun || bCmdLog || bCmdCMake || bCmdDebug )
                {
                    std::cout << "Invalid command combination. Type '--help' for options\n";
                    return 1;
                }
                command_create( bShowHelp, commandArguments );
                return 0;
            }
            else if( !strBuildCommand.empty() )
            {
                if( bCmdCreate || bCmdRun || bCmdLog || bCmdCMake || bCmdDebug )
                {
                    std::cout << "Invalid command combination. Type '--help' for options\n";
                    return 1;
                }
                command_build( bShowHelp, strBuildCommand, commandArguments );
                return 0;
            }
            else if( bCmdRun )
            {
                if( !strBuildCommand.empty() || bCmdCreate || bCmdLog || bCmdCMake || bCmdDebug )
                {
                    std::cout << "Invalid command combination. Type '--help' for options\n";
                    return 1;
                }
                command_run( bShowHelp, commandArguments );
                return 0;
            }
            else if( bCmdLog )
            {
                if( !strBuildCommand.empty() || bCmdCreate || bCmdRun || bCmdCMake || bCmdDebug )
                {
                    std::cout << "Invalid command combination. Type '--help' for options\n";
                    return 1;
                }
                command_log( bShowHelp, commandArguments );
                return 0;
            }
            else if( bCmdCMake )
            {
                if( !strBuildCommand.empty() || bCmdRun || bCmdLog || bCmdCreate || bCmdDebug )
                {
                    std::cout << "Invalid command combination. Type '--help' for options\n";
                    return 1;
                }
                command_cmake( bShowHelp, commandArguments );
                return 0;
            }
            else if( bCmdDebug )
            {
                if( !strBuildCommand.empty() || bCmdRun || bCmdLog || bCmdCMake || bCmdCreate )
                {
                    std::cout << "Invalid command combination. Type '--help' for options\n";
                    return 1;
                }
                command_debug( bShowHelp, commandArguments );
                return 0;
            }
            else if( vm.count( "help" ) )
            {
                std::cout << visibleOptions << "\n";
                return 0;
            }
            else
            {
                std::cout << "Invalid command. Type '--help' for options\n";
                return 1;
            }
        }
        catch( boost::program_options::error& e )
        {
            std::cout << "Invalid input. " << e.what() << "\nType '--help' for options" << std::endl;
            return 1;
        }
        catch( const xml_schema::parser_exception& e )
        {
            std::cout << "XML error: " << e.line () << ":" << e.column ()
             << ": " << e.text () << std::endl;
            return 1;
        }
        catch( std::exception& e )
        {
            std::cout << "Error: " << e.what() << std::endl;
            return 1;
        }
    }

    return 0;
}
