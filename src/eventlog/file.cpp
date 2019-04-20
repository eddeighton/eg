


#include "file.hpp"

#include <iomanip>
#include <sstream>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_uint.hpp>
#include <boost/spirit/include/qi_lit.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>

#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include "iterator.hpp"


namespace IPC
{
namespace Event
{
    
static const std::string EVENT_LOG_FILE_PREFIX = "event_";
static const std::string EVENT_LOG_FILE_EXTENSION = ".log";

std::string toLogFileName( LogFileIndex index )
{
    std::ostringstream osNumber;
    osNumber << std::setfill( '0' ) << std::setw( LogFileIndexDigits ) << index;
        
    std::ostringstream os;
    os << EVENT_LOG_FILE_PREFIX << 
        osNumber.str() << 
        EVENT_LOG_FILE_EXTENSION;
    return os.str();
}

bool fromLogFileName( const std::string& strFileName, LogFileIndex& szIndex )
{
    using LogFileIndexParser =
        boost::spirit::qi::uint_parser< LogFileIndex, 10, 
            LogFileIndexDigits, 
            LogFileIndexDigits >;
    
    LogFileIndexParser logFileIndexParser;
    const bool bResult = boost::spirit::qi::parse
    ( 
        strFileName.begin(), strFileName.end(),
        boost::spirit::qi::string( EVENT_LOG_FILE_PREFIX ) >> 
        logFileIndexParser[ boost::phoenix::ref( szIndex ) = boost::spirit::qi::_1 ] >>
        boost::spirit::qi::string( EVENT_LOG_FILE_EXTENSION )
    );
        
    return bResult;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//File
void File::checkLogFileExists( const boost::filesystem::path& filePath )
{
    if( !boost::filesystem::exists( filePath ) )
    {
        boost::filesystem::ensureFoldersExist( filePath );
        std::filebuf fileBuf;
        fileBuf.open( filePath.string(), std::ios_base::in | std::ios_base::out
                | std::ios_base::trunc | std::ios_base::binary );
        fileBuf.pubseekoff( LOG_FILE_SIZE - 1, std::ios_base::beg );
        fileBuf.sputc(0);
    }
    else
    {
        //check size...
    }
}
    
boost::filesystem::path File::constructLogFile( 
    const boost::filesystem::path& logFilePath )
{
    checkLogFileExists( logFilePath );
    return logFilePath;
}

boost::filesystem::path File::constructPath( 
    const boost::filesystem::path& dir, LogFileIndex index )
{
    return dir / toLogFileName( index );
}

File::File( const boost::filesystem::path& dir, LogFileIndex index )
    :   m_filePath( constructLogFile( constructPath( dir, index ) ) ),
        m_fileMapping( m_filePath.string().c_str(), boost::interprocess::read_write ),
        m_region( m_fileMapping, boost::interprocess::read_write )
{
    m_region.advise( boost::interprocess::mapped_region::advice_sequential );
}
   
File::~File()
{
    m_region.flush( 0U, m_region.get_size() );
    //boost::interprocess::file_mapping::remove( m_filePath.string().c_str() );
}

bool File::isLogFile( const boost::filesystem::path& filePath, LogFileIndex& index )
{
    return fromLogFileName( filePath.filename().string(), index );
}
            
bool File::willFit( std::size_t position, const Event& event ) const
{
    return ( position + event.totalSize() ) < m_region.get_size();
}
 
bool File::isEvent( std::size_t position ) const
{
    VERIFY_RTE_MSG( position < m_region.get_size(), "Event does not fit in memory region" );
    return Event::isEvent( ((char* )m_region.get_address() ) + position );
}
                
void File::read( std::size_t position, Event& event ) const
{
    VERIFY_RTE_MSG( position < m_region.get_size(), "Event does not fit in memory region" );
    event.read( ((char* )m_region.get_address() ) + position );
}

void File::write( std::size_t position, const Event& event )
{
    VERIFY_RTE_MSG( willFit( position, event ), "Event does not fit in memory region" );
    event.write( ((char* )m_region.get_address() ) + position );
}
    
void File::clear()
{
    memset( m_region.get_address(), 0U, m_region.get_size() );
}
        
        
        
} //namespace Event
} //namespace IPC
