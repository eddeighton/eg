
#ifndef STORER_18_04_2019
#define STORER_18_04_2019

#include "archive.hpp"
#include "indexed_object.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <memory>

namespace eg
{

    class Storer
    {
    public:
        Storer( const boost::filesystem::path& filePath );
        
        void storeObject( const IndexedObject* pObject );
        void storeObjectRef( const IndexedObject* pObject );
        
        template< class T >
        inline void store( const T& value )
        {
            m_archive << value;
        }
        
        template< class T >
        inline void storeObjectVector( const std::vector< T* >& objects )
        {
            std::size_t szCount = objects.size();
            store( szCount );
            for( std::size_t sz = 0U; sz < szCount; ++sz )
            {
                storeObjectRef( objects[ sz ] );
            }
        }
        
        template< class T >
        inline void storeObjectVectorVector( const std::vector< std::vector< T* > >& objects )
        {
            std::size_t szCount = objects.size();
            store( szCount );
            for( std::size_t sz = 0U; sz < szCount; ++sz )
            {
                storeObjectVector( objects[ sz ] );
            }
        }
        
        template< class T1, class T2, class TPred >
        inline void storeObjectMap( const std::map< T1*, T2*, TPred >& objects )
        {
            std::size_t szSize = objects.size();
            store( szSize );
            for( std::map< T1*, T2* >::const_iterator 
                i = objects.begin(), 
                iEnd = objects.end(); i!=iEnd; ++i )
            {
                storeObjectRef( i->first );
                storeObjectRef( i->second );
            }
        }
        
        template< class T1, class T2, class TPred >
        inline void storeObjectMap( const std::multimap< T1*, T2*, TPred >& objects )
        {
            std::size_t szSize = objects.size();
            store( szSize );
            for( std::multimap< T1*, T2* >::const_iterator 
                i = objects.begin(), 
                iEnd = objects.end(); i!=iEnd; ++i )
            {
                storeObjectRef( i->first );
                storeObjectRef( i->second );
            }
        }
        
    private:
        std::unique_ptr< boost::filesystem::ofstream > m_pFileStream;
        boost::archive::binary_oarchive m_archive;
    };
    
    

}

#endif //STORER_18_04_2019