
#ifndef LOADER_18_04_2019
#define LOADER_18_04_2019

#include "archive.hpp"
#include "indexed_object.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <memory>

namespace eg
{
    class ObjectFactory;

    class Loader
    {
    public:
        Loader( const boost::filesystem::path& filePath, 
            ObjectFactory& objectFactory, IndexedFile::FileIDToFileMap& files );

        void loadObject();
        
        template< class T >
        inline void load( T& value )
        {
            m_archive >> value;
        }
        
        template< class T >
        T* loadObjectRef()
        {
            T* p = nullptr;
            IndexedObject::FileID fileID = IndexedObject::NO_FILE;
            load( fileID );
            if( fileID != IndexedObject::NO_FILE )
            {
                IndexedFile* pFile = m_files[ fileID ];
                VERIFY_RTE( pFile );
                
                IndexedObject::Index szIndex = IndexedObject::NO_INDEX;
                load( szIndex );
                VERIFY_RTE( szIndex < pFile->m_objects.size() );
                
                IndexedObject* pObject = pFile->m_objects[ szIndex ];
                p = dynamic_cast< T* >( pObject );
                VERIFY_RTE( p );
            }
            return p;
        }
        
        template< class T >
        inline void loadObjectVector( std::vector< T* >& objects )
        {
            std::size_t szCount = 0U;
            load( szCount );
            for( std::size_t sz = 0U; sz < szCount; ++sz )
            {
                objects.push_back( loadObjectRef< T >() );
            }
        }
        
        template< class T1, class T2 >
        inline void loadObjectMap( std::map< T1*, T2* >& objects )
        {
            std::size_t szSize = 0;
            load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const T1* pObject1 = loadObjectRef< T1 >();
                const T2* pObject2 = loadObjectRef< T2 >();
                objects.insert( std::make_pair( pObject1, pObject2 ) );
            }
        }
    private:
        ObjectFactory& m_objectFactory;
        IndexedFile::FileIDToFileMap& m_files;
        std::unique_ptr< boost::filesystem::ifstream > m_pFileStream;
        boost::archive::binary_iarchive m_archive;
    };


}

#endif //LOADER_18_04_2019