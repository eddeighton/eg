
#include "indexed_object.hpp"

#include "loader.hpp"
#include "storer.hpp"

namespace eg
{  

    void IndexedObject::load( Loader& loader )
    {
        loader.load( m_type );
        loader.load( m_fileID );
        loader.load( m_index );
    }
    
    void IndexedObject::store( Storer& storer ) const
    {
        storer.store( m_type );
        storer.store( m_fileID );
        storer.store( m_index );
    }
    
    ObjectFactory::~ObjectFactory()
    {
        
    }


    IndexedFile::IndexedFile( const boost::filesystem::path& filePath, IndexedObject::FileID fileID )
        :   m_filePath( filePath ),
            m_fileID( fileID )
    {
    }
        
    void IndexedFile::beginLoad( ObjectFactory& objectFactory, FileIDToFileMap& fileMap )
    {
        m_pLoader = std::make_shared< Loader >( m_filePath, objectFactory, fileMap );
        Loader& loader = *m_pLoader;
        
        //load the file table
        {
            std::size_t szNumFiles = 0U;
            loader.load( szNumFiles );
            for( std::size_t sz = 0U; sz < szNumFiles; ++sz )
            {
                FileEntry fileEntry;
                loader.load( fileEntry.second );
                loader.load( fileEntry.first );
                m_files.push_back( fileEntry );
            }
        }
        //load the object table
        {
            std::size_t szNumObjects = 0U;
            loader.load( szNumObjects );
            m_objects.resize( szNumObjects );
            for( std::size_t sz = 0U; sz < szNumObjects; ++sz )
            {
                loader.loadObject();
            }
        }
        
        //load the files in the file table
        for( const FileEntry& fileEntry : m_files )
        {
            beginLoadingFile( objectFactory, fileMap, fileEntry.first, fileEntry.second );
        }
    }
    
    void IndexedFile::endLoad()
    {
        //actually load the objects
        if( m_pLoader )
        {
            for( IndexedObject* pObject : m_objects )
            {
                pObject->load( *m_pLoader );
            }
            m_pLoader.reset();
        }
    }
    
    void IndexedFile::beginLoadingFile( ObjectFactory& objectFactory, FileIDToFileMap& fileMap,
        const boost::filesystem::path& filePath, IndexedObject::FileID fileID )
    {
        IndexedFile::FileIDToFileMap::iterator iFind = fileMap.find( fileID );
        if( iFind == fileMap.end() )
        {
            IndexedFile* pIndexedFile = new IndexedFile( filePath, fileID );
            fileMap.insert( std::make_pair( fileID, pIndexedFile ) );
            pIndexedFile->beginLoad( objectFactory, fileMap );
        }
    }
    
    void IndexedFile::load( ObjectFactory& objectFactory, FileIDToFileMap& fileMap,
        const boost::filesystem::path& filePath, IndexedObject::FileID fileID )
    {
        beginLoadingFile( objectFactory, fileMap, filePath, fileID );
        for( FileIDToFileMap::iterator i = fileMap.begin(),
            iEnd = fileMap.end(); i!=iEnd; ++i )
        {
            i->second->endLoad();
        }
    }
    
    void IndexedFile::store( const boost::filesystem::path& filePath, 
            const FileIDtoPathMap& files, const IndexedObject::Array& objects )
    {
        Storer storer( filePath );
        
        //store the file table
        {
            storer.store( files.size() );
            for( FileIDtoPathMap::const_iterator i = files.begin(),
                iEnd = files.end(); i!=iEnd; ++i )
            {
                storer.store( i->first );
                storer.store( i->second );
            }
        }
        
        //store the other files
        //... actually no need
        
        //store the object table
        {
            storer.store( objects.size() );
            for( const IndexedObject* pObject : objects )
            {
                storer.storeObject( pObject );
            }
        }
        
        //store the objects
        {
            for( const IndexedObject* pObject : objects )
            {
                pObject->store( storer );
            }
        }
    }
        
        
}
