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

#include "schema/project.hpp"

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/program_options.hpp>
#include <boost/optional.hpp>

#include <iostream>
#include <string>
#include <vector>

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
        unitTests.push_back( doc::UnitTest( rootDirectory, folder ) );
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
    std::sort( unitTests.begin(), unitTests.end(),
        []( const doc::UnitTest& left, const doc::UnitTest& right )
        {
            return left.m_ordering < right.m_ordering;
        } );
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

void generate( const boost::filesystem::path& targetPath, const doc::UnitTest::PtrVector& group )
{
    boost::filesystem::ensureFoldersExist( targetPath );
    std::unique_ptr< boost::filesystem::ofstream > pFileStream =
        boost::filesystem::createNewFileStream( targetPath );
    
    doc::Identifier headerStack;
    
    heading( *pFileStream, group.front()->m_headings.front(), 0 );
        
    for( const doc::UnitTest::Ptr& pUnitTest : group )
    {
        
        
        
        
        for( const doc::UnitTest::File& file : pUnitTest->m_files )
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
                    heading( *pFileStream, section.m_identifier.back(), section.m_identifier.size() );
                }
                else
                {
                    iHeader = section.m_identifier.begin() + iDepth;
                    for( ; iHeader != section.m_identifier.end(); ++iHeader, ++iDepth )
                    {
                        heading( *pFileStream, *iHeader, iDepth + 1U );
                    }
                }
                headerStack = section.m_identifier;
                
                *pFileStream << section.m_strMarkdown << "\n";
            }
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
        if( boost::filesystem::exists( targetDirectory ) )
        {
            boost::filesystem::remove_all( targetDirectory );
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
    
    doc::UnitTest::PtrVectorVector unitTestGroups;
    {
        std::string strLast;
        for( doc::UnitTest::Vector::iterator 
                i = unitTests.begin(),
                iEnd = unitTests.end(); i!=iEnd; ++i )
        {
            if( strLast == i->m_ordering.front() )
            {
                unitTestGroups.back().push_back( &*i );
            }
            else
            {
                unitTestGroups.push_back( doc::UnitTest::PtrVector{} );
                unitTestGroups.back().push_back( &*i );
                strLast = i->m_ordering.front();
            }
        }
    }
    
    std::ostringstream os;
    doc::print( os, unitTests, bShowMarkDown, bShowCode );
    SPDLOG_INFO( "\n{}", os.str() );
 
    std::vector< boost::filesystem::path > toc;
    
    for( const doc::UnitTest::PtrVector& group : unitTestGroups )
    {
        using namespace std::string_literals;
        boost::filesystem::path rstPath = 
            targetDirectory / group.front()->m_headings.front();
        rstPath.replace_extension( ".rst" );
        VERIFY_RTE( std::find( toc.begin(), toc.end(), rstPath ) == toc.end() );
        toc.push_back( rstPath );
        
        boost::filesystem::ensureFoldersExist( rstPath );
        generate( rstPath, group );
    }
    
    
    //generate the TOC
    

}