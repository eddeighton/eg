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

#include "unittest_parser.hpp"

#include "eg_compiler/sessions/implementation_session.hpp"

#include "schema/project.hpp"

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"
#include "egxml/eg_schema-simpl.hxx"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <iostream>
#include <string>
#include <vector>

doc::UnitTest parseEGProject( const boost::filesystem::path& projectDirectory )
{
    doc::UnitTest unitTest{ projectDirectory };
    
    const boost::filesystem::path projectFile = 
        projectDirectory / Environment::EG_FILE_EXTENSION;
    
    if( !boost::filesystem::exists( projectFile ) )
    {
        THROW_RTE( "Could not locate " << Environment::EG_FILE_EXTENSION << " file in directory: " << projectDirectory.generic_string() );
    }
    
    XMLManager::XMLDocPtr pDocument = XMLManager::load( projectFile );
    
    Environment environment( projectDirectory );
    
    Project project( projectDirectory, environment, pDocument->Project() );
    
    eg::ReadSession session( project.getAnalysisFileName() );

    std::cout << "Found project: " << projectDirectory.string() << std::endl;
    for( const boost::filesystem::path& egSourceCodeFile : project.getEGSourceCode() )
    {
        doc::UnitTest::File file{ egSourceCodeFile };
        
        std::string strFileContents;
        boost::filesystem::loadAsciiFile( egSourceCodeFile, strFileContents );
        
        std::ostringstream osError;
        if( !doc::parseFileSections( strFileContents, file.m_sections, osError ) )
        {
            THROW_RTE( osError.str() );
        }
        else
        {
            unitTest.m_files.push_back( file );
        }
    }
    
    return unitTest;
}

void recurseFolders( const boost::filesystem::path& folder, doc::UnitTest::Vector& unitTests )
{
    bool bContainsEGFile = false;
    for( boost::filesystem::directory_iterator iter( folder );
        iter != boost::filesystem::directory_iterator(); ++iter )
    {
        const boost::filesystem::path& filePath = *iter;
        if( !boost::filesystem::is_directory( filePath ) )
        {
            if( boost::filesystem::extension( *iter ) == Environment::EG_FILE_EXTENSION )
            {
                if( filePath.stem().empty() )
                {
                    bContainsEGFile = true;
                    break;
                }
            }
        }
    }
    
    if( bContainsEGFile )
    {
        unitTests.push_back( parseEGProject( folder ) );
    }
    else
    {
        for( boost::filesystem::directory_iterator iter( folder );
            iter != boost::filesystem::directory_iterator(); ++iter )
        {
            const boost::filesystem::path& filePath = *iter;
            if( boost::filesystem::is_directory( filePath ) )
            {
                recurseFolders( filePath, unitTests );
            }
        }
    }
}

void command_doc( bool bHelp, const std::vector< std::string >& args )
{
    std::string strDirectory;
    std::string strStructureFilePath;
    bool bShowMarkDown = false;
    bool bShowCode = false;
    
    {
        namespace po = boost::program_options;
        po::options_description commandOptions(" Read Project Log");
        {
            commandOptions.add_options()
                ("dir",         po::value< std::string >( &strDirectory ),          "Project directory")
                ("structure",   po::value< std::string >( &strStructureFilePath ),  "Structure file path" )
                ("markdown",    po::bool_switch( &bShowMarkDown ),                  "Show Markdown" )
                ("code",        po::bool_switch( &bShowCode ),                      "Show Code" )
            ;
        }
        
        po::positional_options_description p;
        p.add( "filters", -1 );
        
        po::variables_map vm;
        po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
        po::notify( vm );
        
        if( bHelp )
        {
            std::cout << commandOptions << "\n";
            return;
        }
    }
    
    const boost::filesystem::path rootDirectory = 
        boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( strDirectory ) );

    if( !boost::filesystem::exists( rootDirectory ) )
    {
        THROW_RTE( "Specified directory does not exist: " << rootDirectory.generic_string() );
    }
    else if( !boost::filesystem::is_directory( rootDirectory ) )
    {
        THROW_RTE( "Specified path is not a directory: " << rootDirectory.generic_string() );
    }
    
    
    doc::UnitTest::Vector unitTests;
    
    recurseFolders( rootDirectory, unitTests );
    
    doc::print( std::cout, unitTests, bShowMarkDown, bShowCode );
}