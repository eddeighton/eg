

#include "session.hpp"

#include "objects.hpp"

namespace eg
{

    Session::Session( const IndexedFile::FileIDtoPathMap& files )
    {
        ObjectFactoryImpl objectFactory;
        for( IndexedFile::FileIDtoPathMap::const_iterator i = files.begin(),
            iEnd = files.end(); i!=iEnd; ++i )
        {
            IndexedFile::load( objectFactory, m_fileMap, i->second, i->first );
        }
    }
    
    Session::Session( const boost::filesystem::path& filePath, IndexedObject::FileID fileID )
    {
        ObjectFactoryImpl objectFactory;
        IndexedFile::load( objectFactory, m_fileMap, filePath, fileID );
    }
    
    Session::~Session()
    {
        for( IndexedFile::FileIDToFileMap::iterator 
            i = m_fileMap.begin(), iEnd = m_fileMap.end();
            i!=iEnd; ++i )
        {
            for( const IndexedObject* pObject : i->second->getObjects() )
            {
                delete pObject;
            }
            delete i->second;
        }
    }
    
   
    AppendingSession::AppendingSession( const boost::filesystem::path& filePath, IndexedObject::FileID fileID )
        :   Session( filePath, fileID )
    {
        
    }
    
    void AppendingSession::store() const
    {
        IndexedFile::FileIDtoPathMap files;
        for( IndexedFile::FileIDToFileMap::const_iterator 
            i = m_fileMap.begin(),
            iEnd = m_fileMap.end(); i!=iEnd; ++i )
        {
            IndexedFile* pFile = i->second;
            files.insert( std::make_pair( pFile->getFileID(), pFile->getPath() ) );
        }
        for( IndexedFile::FileIDToFileMap::const_iterator 
            i = m_fileMap.begin(),
            iEnd = m_fileMap.end(); i!=iEnd; ++i )
        {
            IndexedFile* pFile = i->second;
            IndexedFile::store( pFile->getPath(), files, pFile->getObjects() );
        }
    }
    
    
    CreatingSession::CreatingSession( const IndexedFile::FileIDtoPathMap& files, IndexedObject::FileID fileID )
        :   Session( files ),
            m_fileID( fileID )
    {
    }
    
    CreatingSession::~CreatingSession()
    {
        for( IndexedObject* pObject : m_newObjects )
        {
            delete pObject;
        }
    }
    
    void CreatingSession::store( const boost::filesystem::path& filePath ) const
    {
        IndexedFile::FileIDtoPathMap files;
        for( IndexedFile::FileIDToFileMap::const_iterator 
            i = m_fileMap.begin(),
            iEnd = m_fileMap.end(); i!=iEnd; ++i )
        {
            IndexedFile* pFile = i->second;
            files.insert( std::make_pair( pFile->getFileID(), pFile->getPath() ) );
        }
        files.insert( std::make_pair( m_fileID, filePath ) );
        IndexedFile::store( filePath, files, m_newObjects );
    }
        


}