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




#include "schema/project.hpp"

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"
#include "egxml/eg_schema-simpl.hxx"

#include "installationxml/installation_schema.hxx"
#include "installationxml/installation_schema-pimpl.hxx"
#include "installationxml/installation_schema-simpl.hxx"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <iostream>

void command_run( bool bHelp, const std::string& strRunCommand, const std::vector< std::string >& args )
{
    std::string strDirectory;
    std::string strCommand = strRunCommand;
    bool bWait = false;
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Create Project Command");
    {
        commandOptions.add_options()
            ("dir",     po::value< std::string >( &strDirectory ), "Project directory")
            ("wait",    po::bool_switch( &bWait ), "Wait at startup to attach debugger" )
        ;
    }
    
    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).run(), vm );
    po::notify( vm );
    
    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        
        const boost::filesystem::path projectDirectory = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDirectory ) );

        if( !boost::filesystem::exists( projectDirectory ) )
        {
            THROW_RTE( "Specified directory does not exist: " << projectDirectory.generic_string() );
        }
        else if( !boost::filesystem::is_directory( projectDirectory ) )
        {
            THROW_RTE( "Specified path is not a directory: " << projectDirectory.generic_string() );
        }
        
        const boost::filesystem::path projectFile = projectDirectory / Environment::EG_FILE_EXTENSION;
        
        if( !boost::filesystem::exists( projectFile ) )
        {
            THROW_RTE( "Could not locate " << Environment::EG_FILE_EXTENSION << " file in directory: " << projectDirectory.generic_string() );
        }
        
        XMLManager::XMLDocPtr pDocument = XMLManager::load( projectFile );
        
        Environment environment( projectDirectory );
        
        const egxml::Project& project = pDocument->Project();
        
        boost::optional< const egxml::Run& > foundCommand;
            
        for( const egxml::Run& runCmd : project.Run() )
        {
            if( runCmd.Name() == strCommand )
            {
                foundCommand = runCmd;
                break;
            }
        }
        
        if( foundCommand )
        {
            std::ostringstream osCmd;
            
            osCmd << environment.expand( foundCommand.get().Command() );
            for( const auto& arg : foundCommand.get().Argument() )
            {
                osCmd << " " << environment.expand( arg );
            }
            
            if( bWait ) 
            {
                osCmd << " --debug";
            }

            {
                boost::process::environment childEnv = environment.getEnvironment();
                
                const int iResult = boost::process::system( osCmd.str(), childEnv ); //, 
                //cinder does not call PostQuitMessage so you can the last windows message as return code...
                //if( iResult )
                //{
                //    THROW_RTE( "Error invoking eg program " << iResult );
                //}
            }
        }
        else
        {
            THROW_RTE( "Failed to locate command: " << strCommand );
        }
            
        
    }
}