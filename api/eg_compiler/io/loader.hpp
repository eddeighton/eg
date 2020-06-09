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


#ifndef LOADER_18_04_2019
#define LOADER_18_04_2019

#include "archive.hpp"
#include "indexed_object.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/binary_iarchive.hpp>

#include <memory>
#include <optional>
#include <set>
#include <map>
#include <vector>

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
        inline void loadOptional( std::optional< T >& value )
        {
            bool bNull = false;
            m_archive >> bNull;
            if( bNull )
            {
                T valueTemp;
                load( valueTemp );
                value = valueTemp;
            }
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
                VERIFY_RTE_MSG( p, "Failed to load indexed object" );
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
        
        template< class T, class TPred >
        inline void loadObjectSet( std::set< T*, TPred >& objects )
        {
            std::size_t szCount = 0U;
            load( szCount );
            for( std::size_t sz = 0U; sz < szCount; ++sz )
            {
                objects.insert( objects.end(), loadObjectRef< T >() );
            }
        }
        
        template< class T >
        inline void loadObjectVectorVector( std::vector< std::vector< T* > >& objects )
        {
            std::size_t szCount = 0U;
            load( szCount );
            for( std::size_t sz = 0U; sz < szCount; ++sz )
            {
                std::vector< T* > nested;
                loadObjectVector( nested );
                objects.push_back( nested );
            }
        }
        
        template< class T1, class T2, class TPred >
        inline void loadObjectMap( std::map< T1*, T2*, TPred >& objects )
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
        
        template< class T1, class T2 >
        inline void loadKeyObjectMap( std::map< T1, T2* >& objects )
        {
            std::size_t szSize = 0;
            load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                T1 strKey;
				load< T1 >( strKey );
                const T2* pObject2 = loadObjectRef< T2 >();
                objects.insert( std::make_pair( strKey, pObject2 ) );
            }
        }
        
        template< class T1, class T2, class TPred >
        inline void loadObjectMap( std::multimap< T1*, T2*, TPred >& objects )
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