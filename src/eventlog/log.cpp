


#include "log.hpp"

#include <iostream>

#include "common/file.hpp"
#include "common/assert_verify.hpp"

namespace IPC
{

namespace Event
{
       
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//Log
Log::Log( const boost::filesystem::path& filePath )
    :   m_filePath( boost::filesystem::edsCannonicalise(
                        boost::filesystem::absolute( filePath ) ) )
{
    if( !boost::filesystem::exists( m_filePath ) )
    {
        if( !boost::filesystem::create_directories( m_filePath ) )
        {
            THROW_RTE( "Failed to create directories for: " << m_filePath.string() );
            throw std::runtime_error( "Failed to create directories" );
        } 
    }
    if( !boost::filesystem::is_directory( m_filePath ) )
    {
        THROW_RTE( "Failed to create directories for: " << m_filePath.string() );
        throw std::runtime_error( "Failed to create directories" );
    }  
    
}

File::Ptr Log::getFile( LogFileIndex szFileIndex, bool bAutoDump /*= true*/ )
{
    while( szFileIndex >= m_logFiles.size() )
    {
        m_logFiles.push_back( File::Ptr() );
    }
    if( nullptr == m_logFiles[ szFileIndex ].get() )
    {
        //create the event log file
        m_logFiles[ szFileIndex ] = 
            std::make_shared< File >( m_filePath, szFileIndex );
    }
    
    //release old files
    std::size_t szOld = 0U;
    while( ( szOld + 2U ) < szFileIndex )
    {
        m_logFiles[ szOld++ ] = File::Ptr();
    }
    
    return m_logFiles[ szFileIndex ];
}

bool Log::read( Iterator& iterator, Event& event )
{
    LogFileIndex szFileID = getFileID( iterator );
    File::Ptr pLogFile = getFile( szFileID );
    const std::size_t szPosition = getPosition( iterator );
    if( pLogFile->isEvent( szPosition ) )
    {
        pLogFile->read( szPosition, event );
        iterator += event.totalSize();
        return true;
    }
    else 
    {
        ++szFileID;
        File::Ptr pLogFile = getFile( szFileID );
        if( pLogFile->isEvent( 0U ) )
        {
            pLogFile->read( 0U, event );
            iterator = getIterFromFileID( szFileID ) + event.totalSize();
            return true;
        }
    }
    return false;
}
          
void Log::clear()
{
    //enumerate all files in directory
    for( boost::filesystem::directory_entry& entry : 
        boost::filesystem::directory_iterator( m_filePath ) )
    {
        const boost::filesystem::path& filePath = entry.path();
        LogFileIndex szFileIndex = 0U;
        if( File::isLogFile( filePath, szFileIndex ) )
        {
            if( ( m_logFiles.size() > szFileIndex ) && 
                ( nullptr != m_logFiles[ szFileIndex ].get() ) )
            {
                m_logFiles[ szFileIndex ]->clear();
            }
            else
            {
                //create file to wipe clean...
                File file( m_filePath, szFileIndex );
                file.clear();
            }
        }
    }
}     
           
} //namespace Event



} //namespace IPC