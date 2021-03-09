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
#include "log.hpp"

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

doc::UnitTest parseEGProject( const boost::filesystem::path& rootDirectory, const boost::filesystem::path& projectDirectory )
{
    //record relative path
    doc::UnitTest unitTest{ boost::filesystem::relative( projectDirectory, rootDirectory ) };
    
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

    SPDLOG_INFO( "Found project: {}", projectDirectory.string() );
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

void recurseFolders( const boost::filesystem::path& rootDirectory, const boost::filesystem::path& folder, doc::UnitTest::Vector& unitTests )
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
        unitTests.push_back( parseEGProject( rootDirectory, folder ) );
    }
    else
    {
        for( boost::filesystem::directory_iterator iter( folder );
            iter != boost::filesystem::directory_iterator(); ++iter )
        {
            const boost::filesystem::path& filePath = *iter;
            if( boost::filesystem::is_directory( filePath ) )
            {
                recurseFolders( rootDirectory, filePath, unitTests );
            }
        }
    }
}

void orderUnitTests( doc::UnitTest::Vector& unitTests )
{
    //TODO
}

void heading( std::ostream& os, const std::string& str, int depth )
{
    char c = '"';
    switch( depth )
    {
        case 0: c = '#'; break;
        case 1: c = '*'; break;
        case 2: c = '='; break;
        case 3: c = '-'; break;
        case 4: c = '^'; break;
        case 5: c = '#'; break;
        default:
            break;
    }
    
    for( std::size_t sz = 0U; sz != str.size(); ++sz )
        os << c;
    os << '\n' << str << '\n';
    for( std::size_t sz = 0U; sz != str.size(); ++sz )
        os << c;
    os << '\n';
}

void generate( const boost::filesystem::path& targetPath, const doc::UnitTest& unitTest )
{
    boost::filesystem::ensureFoldersExist( targetPath );
    std::unique_ptr< boost::filesystem::ofstream > pFileStream =
        boost::filesystem::createNewFileStream( targetPath );
    
    doc::Identifier headerStack;
    
    for( const doc::UnitTest::File& file : unitTest.m_files )
    {
        for( const doc::UnitTest::File::Section& section : file.m_sections )
        {
            std::size_t iDepth = 0U;
            doc::Identifier::const_iterator iStack    = headerStack.begin();
            doc::Identifier::const_iterator iHeader   = section.m_identifier.begin();
            
            for( ; ( iStack != headerStack.end() ) && ( iHeader != section.m_identifier.end() ); ++iStack, ++iHeader )
            {
                if( *iStack == *iHeader )
                {
                    ++iDepth;
                }
                else
                {
                    break;
                }
            }
            
            if( iHeader == section.m_identifier.end() )
            {
                heading( *pFileStream, section.m_identifier.back(), section.m_identifier.size() - 1U );
            }
            else
            {
                iHeader = section.m_identifier.begin() + iDepth;
                for( ; iHeader != section.m_identifier.end(); ++iHeader, ++iDepth )
                {
                    heading( *pFileStream, *iHeader, iDepth );
                }
            }
            headerStack = section.m_identifier;
            
            *pFileStream << section.m_strMarkdown << "\n";
        }
    }
    
}

void command_doc( bool bHelp, const std::vector< std::string >& args )
{
    std::string strDirectory;
    std::string strTargetDirectory;
    bool bShowMarkDown = false;
    bool bShowCode = false;
    
    {
        namespace po = boost::program_options;
        po::options_description commandOptions(" Read Project Log");
        {
            commandOptions.add_options()
                ("dir",         po::value< std::string >( &strDirectory ),          "Project directory")
                ("target",      po::value< std::string >( &strTargetDirectory ),    "Target directory to generate documentation" )
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
            std::ostringstream os;
            os << commandOptions;
            SPDLOG_INFO( "{}", os.str() );
            return;
        }
    }
    
    if( strTargetDirectory.empty() )
    {
        THROW_RTE( "Target directory not specified" );
    }
    
    const boost::filesystem::path targetDirectory = 
        boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( strTargetDirectory ) );
    {
        if( !boost::filesystem::exists( targetDirectory ) )
        {
            THROW_RTE( "Target directory does not exist: " << targetDirectory.generic_string() );
        }
        else if( !boost::filesystem::is_directory( targetDirectory ) )
        {
            THROW_RTE( "Target path is not a directory: " << targetDirectory.generic_string() );
        }
    }
    
    const boost::filesystem::path rootDirectory = 
        boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( strDirectory ) );
    {
        if( !boost::filesystem::exists( rootDirectory ) )
        {
            THROW_RTE( "Specified directory does not exist: " << rootDirectory.generic_string() );
        }
        else if( !boost::filesystem::is_directory( rootDirectory ) )
        {
            THROW_RTE( "Specified path is not a directory: " << rootDirectory.generic_string() );
        }
    }
    
    
    doc::UnitTest::Vector unitTests;
    
    recurseFolders( rootDirectory, rootDirectory, unitTests );
    
    orderUnitTests( unitTests );
    
    std::ostringstream os;
    doc::print( os, unitTests, bShowMarkDown, bShowCode );
    SPDLOG_INFO( "\n{}", os.str() );
 
    std::vector< boost::filesystem::path > toc;
    for( const doc::UnitTest& unitTest : unitTests )
    {
        using namespace std::string_literals;
        boost::filesystem::path rstPath = 
            targetDirectory / unitTest.m_directory;
        rstPath.replace_extension( ".rst" );
        
        generate( rstPath, unitTest );
        toc.push_back( rstPath );
    }
    
    //generate the TOC
    

}