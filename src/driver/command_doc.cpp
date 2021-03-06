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
#include <string_view>
#include <iomanip>

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

void generateHeaders( std::ostream& os, const doc::Identifier& header, doc::Identifier& headerStack )
{
    doc::Identifier::const_iterator iStack    = headerStack.begin();
    doc::Identifier::const_iterator iHeader   = header.begin();
    
    std::size_t iHeaderDepth = 0U;
    for( ; ( iStack != headerStack.end() ) && ( iHeader != header.end() ); ++iStack, ++iHeader )
    {
        if( *iStack == *iHeader )
        {
            ++iHeaderDepth;
        }
        else
        {
            break;
        }
    }
    
    if( iHeader == header.end() )
    {
        heading( os, header.back(), header.size() - 1U );
    }
    else
    {
        iHeader = header.begin() + iHeaderDepth;
        for( ; iHeader != header.end(); ++iHeader, ++iHeaderDepth )
        {
            heading( os, *iHeader, iHeaderDepth );
        }
    }
    headerStack = header;
}

std::string lowerCase( const std::string& str )
{
    std::string strResult = str;
    std::transform( strResult.begin(), strResult.end(), strResult.begin(), 
        []( char c ) { return std::tolower( c ); } );
    return strResult;
}

void generate( const boost::filesystem::path& targetPath, const doc::UnitTest::PtrVector& group )
{
    boost::filesystem::ensureFoldersExist( targetPath );
    std::unique_ptr< boost::filesystem::ofstream > pFileStream =
        boost::filesystem::createNewFileStream( targetPath );
        
    std::ostream& os = *pFileStream;
    
    doc::Identifier headerStack;
    for( const doc::UnitTest::Ptr& pUnitTest : group )
    {
        generateHeaders( *pFileStream, pUnitTest->m_headings, headerStack );
        
        os << "\n`" << pUnitTest->m_headings.back() << " Program Source Code <https://github.com/eddeighton/eg/tree/master/tests/reference/" << 
            pUnitTest->m_directory.generic_string() << ">`_\n\n";
        
        for( const doc::UnitTest::File& file : pUnitTest->m_files )
        {
            for( const doc::UnitTest::File::Section& section : file.m_sections )
            {
                doc::Identifier header = pUnitTest->m_headings;
                std::copy( section.m_identifier.begin(), section.m_identifier.end(),
                    std::back_inserter( header ) );
                
                generateHeaders( *pFileStream, header, headerStack );
                
                *pFileStream << section.m_strMarkdown << "\n";
                
                //generate code example
                if( !section.m_code.empty() )
                {
                    bool bGeneratedHeader = false;
                    
                    std::vector< std::string > lines;
                    boost::split( lines, section.m_code, []( char c ){ return c == '\n'; } );
                    
                    for( const std::string& str : lines )
                    {
                        bool bOnlyWhiteSpace = true;
                        for( char c : str )
                        {
                            if( !std::isspace( c ) )
                            {
                                bOnlyWhiteSpace = false;
                                break;
                            }
                        }
                        if( !bOnlyWhiteSpace )
                        {
                            if( !bGeneratedHeader )
                            {
                                *pFileStream << "\nexample::\n\n";
                                bGeneratedHeader = true;
                            }
                            *pFileStream << "    " << str << '\n';
                        }
                    }
                    if( bGeneratedHeader )
                    {
                        *pFileStream << "\n";
                    }
                }
            }
        }
                
        //generate event log table
        if( !pUnitTest->m_events.empty() )
        {
            os << "Program Output:\n\n";
            
            struct Row
            {
                std::string strTimestamp, strEventType, strValue, strInstance;
            };
            using RowVector = std::vector< Row >;
            RowVector rows;
            
            auto formatInt = []( std::size_t sz ) -> std::string
            {
                std::ostringstream os;
                os << std::setw( 8 ) << std::setfill( '0' ) << sz;
                return os.str();
            };
            
            for( const doc::UnitTest::Event& event : pUnitTest->m_events )
            {
                Row row;
                row.strTimestamp = formatInt( event.timestamp );
                
                switch( event.eventType )
                {
                    case doc::UnitTest::Event::eStart            :
                        {
                            row.strEventType = "START";
                            if( !event.strValue.empty() )
                                row.strValue = event.strValue;
                            else
                                row.strValue = formatInt( event.type );
                            row.strInstance = formatInt( event.instance );
                        }
                        break;
                    case doc::UnitTest::Event::eStop             :
                        {
                            row.strEventType = "STOP";
                            if( !event.strValue.empty() )
                                row.strValue = event.strValue;
                            else
                                row.strValue = formatInt( event.type );
                            row.strInstance = formatInt( event.instance );
                        }
                        break;
                    case doc::UnitTest::Event::eLog              :
                        {
                            row.strEventType = "LOG";
                            row.strValue = event.strValue;
                        }
                        break;
                    case doc::UnitTest::Event::eError            :
                        {
                            row.strEventType = "ERROR";
                            row.strValue = event.strValue;
                        }
                        break;
                    case doc::UnitTest::Event::ePass             :
                        {
                            row.strEventType = "PASS";
                            row.strValue = event.strValue;
                        }
                        break;
                    case doc::UnitTest::Event::eFail             :
                        {
                            row.strEventType = "FAIL";
                            row.strValue = event.strValue;
                        }
                        break;
                    case doc::UnitTest::Event::eOther            :
                        {
                            row.strEventType = "OTHER";
                            row.strValue = event.strValue;
                        }
                        break;
                    case doc::UnitTest::Event::TOTAL_EVENT_TYPES :
                        THROW_RTE( "Unknown event type" );
                }
                rows.push_back( row );
            }
            
            using namespace std::string_literals;
            const Row header{ "Timestamp"s, "Type"s, "Value"s, "Instance"s };
            
            //determine column widths
            std::size_t szTimeStampsWidth   = header.strTimestamp.size(),
                        szEventTypesWidth   = header.strEventType.size(),
                        szValuesWidth       = header.strValue.size(),
                        szInstancesWidth    = header.strInstance.size();
            for( const Row& row : rows )
            {
                szTimeStampsWidth   = std::max( row.strTimestamp.size(),    szTimeStampsWidth );
                szEventTypesWidth   = std::max( row.strEventType.size(),    szEventTypesWidth );
                szValuesWidth       = std::max( row.strValue.size(),        szValuesWidth );
                szInstancesWidth    = std::max( row.strInstance.size(),     szInstancesWidth );
            }
                        
            //generate the table
            
            auto drawLine = []( std::ostream& os, char c, std::size_t sz1, std::size_t sz2, std::size_t sz3, std::size_t sz4 ) -> void
            {
                os << '+';
                for( std::size_t sz = 0U; sz != sz1; ++sz )
                    os << c;
                os << '+';
                for( std::size_t sz = 0U; sz != sz2; ++sz )
                    os << c;
                os << '+';
                for( std::size_t sz = 0U; sz != sz3; ++sz )
                    os << c;
                os << '+';
                for( std::size_t sz = 0U; sz != sz4; ++sz )
                    os << c;
                os << '+';
                os << '\n';
            };
            
            auto drawRow = []( std::ostream& os, std::size_t sz1, std::size_t sz2, std::size_t sz3, std::size_t sz4, const Row& row ) -> void
            {
                os << '|';
                os << row.strTimestamp;
                for( std::size_t sz = row.strTimestamp.size(); sz < sz1; ++sz )
                    os << ' ';
                
                os << '|';
                os << row.strEventType;
                for( std::size_t sz = row.strEventType.size(); sz < sz2; ++sz )
                    os << ' ';
                
                os << '|';
                os << row.strValue;
                for( std::size_t sz = row.strValue.size(); sz < sz3; ++sz )
                    os << ' ';
                
                os << '|';
                os << row.strInstance;
                for( std::size_t sz = row.strInstance.size(); sz < sz4; ++sz )
                    os << ' ';
                
                os << '|';
                os << '\n';
            };
            
            drawLine( os, '-',  szTimeStampsWidth ,
                                szEventTypesWidth ,
                                szValuesWidth     ,
                                szInstancesWidth  );
            
            drawRow( os,        szTimeStampsWidth ,
                                szEventTypesWidth ,
                                szValuesWidth     ,
                                szInstancesWidth  ,
                                header );
                
            drawLine( os, '=',  szTimeStampsWidth ,
                                szEventTypesWidth ,
                                szValuesWidth     ,
                                szInstancesWidth  );
                                
            for( const Row& row : rows )
            {
                drawRow( os,        szTimeStampsWidth ,
                                    szEventTypesWidth ,
                                    szValuesWidth     ,
                                    szInstancesWidth  ,
                                    row );
                    
                drawLine( os, '-',  szTimeStampsWidth ,
                                    szEventTypesWidth ,
                                    szValuesWidth     ,
                                    szInstancesWidth  );
            }
            os << "\n";
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
            //check the folder ONLY contains .rst files
            boost::filesystem::recursive_directory_iterator iterEnd;
            boost::filesystem::recursive_directory_iterator iter( targetDirectory );
            for( ; iter != iterEnd; ++iter )
            {
                const boost::filesystem::path pathTemp = *iter;
                if( boost::filesystem::is_regular_file( pathTemp ) )
                {
                    VERIFY_RTE_MSG( pathTemp.extension() == ".rst", "Target folder contains non-rst files: " << targetDirectory.string() );
                }
            }
            
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
        
        toc.push_back( boost::filesystem::relative( rstPath, targetDirectory.parent_path() )  );
        
        boost::filesystem::ensureFoldersExist( rstPath );
        generate( rstPath, group );
    }
    
    
    //generate the TOC
    const boost::filesystem::path indexTemplatePath = 
        targetDirectory / "../templates/index_template.rst";
    const boost::filesystem::path indexPath = 
        targetDirectory / "../index.rst";
    
    {
        std::string strTemplate;
        boost::filesystem::loadAsciiFile( indexTemplatePath, strTemplate );
        
        static const std::string strKey = "<FILELIST_LOCATION>";
        std::string::iterator iFind =
            std::search( strTemplate.begin(), strTemplate.end(), strKey.begin(), strKey.end() );
            
        VERIFY_RTE_MSG( iFind != strTemplate.end(), 
            "Failed to locate: " <<strKey << " in " << indexTemplatePath.string() );
        const std::size_t szPosition = std::distance( strTemplate.begin(), iFind );
        
        //rewrite the table of contents
        std::ostringstream osTOC;
        for( const boost::filesystem::path& page : toc )
        {
            osTOC << "   " << page.generic_string() << "\n";
        }
        
        const std::string_view firstPart( strTemplate.data(), szPosition );
        const std::string_view lastPart( strTemplate.data() + szPosition + strKey.size(), 
            strTemplate.size() - ( szPosition + strKey.size() ) );
        
        std::ostringstream osRender;
        osRender << firstPart << osTOC.str() << lastPart;
        
        boost::filesystem::updateFileIfChanged( indexPath, osRender.str() );
    }
    

}