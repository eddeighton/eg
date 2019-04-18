
#include "loader.hpp"

#include "common/file.hpp"

namespace eg
{

    Loader::Loader( const boost::filesystem::path& filePath, 
        ObjectFactory& objectFactory, IndexedFile::FileIDToFileMap& files )
        :   m_objectFactory( objectFactory ),
            m_files( files ),
            m_pFileStream( boost::filesystem::createBinaryInputFileStream( filePath ) ),
            m_archive( *m_pFileStream )
    {
    }
        
    void Loader::loadObject()
    {
        IndexedObject object;
        object.load( *this );
        
        IndexedObject::FileID fileID = object.getFileID();
        IndexedFile* pFile = m_files[ fileID ];
        VERIFY_RTE( pFile );
        
        std::size_t szIndex = object.getIndex();
        VERIFY_RTE( szIndex < pFile->m_objects.size() );
        IndexedObject* pObject = pFile->m_objects[ szIndex ];
        VERIFY_RTE( !pObject );
        
        pObject = m_objectFactory.create( object );
        
        pFile->m_objects[ szIndex ] = pObject;
    }


}
