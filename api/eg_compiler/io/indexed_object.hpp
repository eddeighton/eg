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



#ifndef INDEXED_FILE_18_04_2019
#define INDEXED_FILE_18_04_2019

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>

#include <cstddef>
#include <utility>
#include <vector>
#include <map>
#include <memory>

namespace eg
{
    class Loader;
    class Storer;
    
    class IndexedObject
    {
    public:
        using Array = std::vector< IndexedObject* >;

        using Type = std::int32_t;
        static const Type NO_TYPE = (Type)-1;
    
        using Index = std::int32_t;
        static const Index NO_INDEX = (Index)-1;
        
        using FileID = std::int32_t;
        static const FileID NO_FILE         = (FileID)-1;
        static const FileID MASTER_FILE     = (FileID)0U;
        static const FileID ANALYSIS_FILE   = (FileID)1U;
        static const FileID TU_FILES_BEGIN  = (FileID)2U;
        
        virtual ~IndexedObject(){};
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
        inline Type getType() const { return m_type; }
        inline FileID getFileID() const { return m_fileID; }
        inline Index getIndex() const { return m_index; }
        
        IndexedObject(){}
        
        IndexedObject( Type type, FileID fileID, Index index )
            :   m_type( type ),
                m_fileID( fileID ),
                m_index( index )
        {
            
        }
        
    private:
        Type    m_type      = NO_TYPE;
        FileID  m_fileID    = NO_FILE;
        Index   m_index     = NO_INDEX;
    };

    template< typename T >
    inline std::vector< T* > many( const IndexedObject::Array& objects )
    {
        std::vector< T* > found;
        for( IndexedObject* pObject : objects )
        {
            if( T* p = dynamic_cast< T* >( pObject ) )
            {
                found.push_back( p );
            }
        }
        return found;
    }
    
    template< typename T >
    inline T* one( const IndexedObject::Array& objects )
    {
        std::vector< T* > found = many< T >( objects );
        VERIFY_RTE( found.size() == 1U );
        return found.front();
    }
    
    template< typename T >
    inline T* oneOpt( const IndexedObject::Array& objects )
    {
        std::vector< T* > found = many< T >( objects );
        if( !found.empty() )
        {
            VERIFY_RTE( found.size() == 1U );
            return found.front();
        }
        else
        {
            return nullptr;
        }
    }
    
    template< typename T >
    inline T* root( const IndexedObject::Array& objects )
    {
        T* pFound = nullptr;
        for( IndexedObject* pObject : objects )
        {
            if( T* p = dynamic_cast< T* >( pObject ) )
            {
                if( p->getParent() == nullptr )
                {
                    VERIFY_RTE( pFound == nullptr );
                    pFound = p;
                }
            }
        }
        VERIFY_RTE( pFound );
        return pFound;
    }
        
    
    template< typename T >
    inline std::vector< const T* > many_cst( const IndexedObject::Array& objects )
    {
        std::vector< const T* > found;
        for( IndexedObject* pObject : objects )
        {
            if( const T* p = dynamic_cast< const T* >( pObject ) )
            {
                found.push_back( p );
            }
        }
        return found;
    }
    
    template< typename T >
    inline const T* one_cst( const IndexedObject::Array& objects )
    {
        std::vector< const T* > found = many< const T >( objects );
        VERIFY_RTE( found.size() == 1U );
        return found.front();
    }
    
    template< typename T >
    inline const T* oneOpt_cst( const IndexedObject::Array& objects )
    {
        std::vector< const T* > found = many< const T >( objects );
        if( !found.empty() )
        {
            VERIFY_RTE( found.size() == 1U );
            return found.front();
        }
        else
        {
            return nullptr;
        }
    }
    
    template< typename T >
    inline const T* root_cst( const IndexedObject::Array& objects )
    {
        const T* pFound = nullptr;
        for( const IndexedObject* pObject : objects )
        {
            if( const T* p = dynamic_cast< const T* >( pObject ) )
            {
                if( p->getParent() == nullptr )
                {
                    VERIFY_RTE( pFound == nullptr );
                    pFound = p;
                }
            }
        }
        VERIFY_RTE( pFound );
        return pFound;
    }
    
    struct CompareIndexedObjects
    {
        inline bool operator()( const IndexedObject* pLeft, const IndexedObject* pRight ) const
        {
			VERIFY_RTE( pLeft && pRight );
            return  ( pLeft->getFileID() != pRight->getFileID() ) ? 
                    ( pLeft->getFileID() < pRight->getFileID() ) :
                    ( pLeft->getIndex() < pRight->getIndex() );
        }
    };

    class ObjectFactory
    {
    public:
        virtual ~ObjectFactory();
        virtual IndexedObject* create( const IndexedObject& objectSpec ) = 0;
    };
    
    class IndexedFile
    {
        friend class Loader;
    public:
        using FileEntry = std::pair< boost::filesystem::path, IndexedObject::FileID >;
        using FileTable = std::vector< FileEntry >;
        using FileIDToFileMap = std::map< IndexedObject::FileID, IndexedFile* >;
        using FileIDtoPathMap = std::map< IndexedObject::FileID, boost::filesystem::path >;
        
        IndexedFile( const boost::filesystem::path& filePath, IndexedObject::FileID fileID );
            
        static IndexedObject::FileID readFileID( const boost::filesystem::path& filePath );
        
        static void load( ObjectFactory& objectFactory, FileIDToFileMap& fileMap,
            const boost::filesystem::path& filePath, IndexedObject::FileID fileID );

        static void store( const boost::filesystem::path& filePath, IndexedObject::FileID fileID, 
                const FileIDtoPathMap& files, const IndexedObject::Array& objects );
                
        const boost::filesystem::path& getPath() const { return m_filePath; }
        const IndexedObject::FileID getFileID() const { return m_fileID; }
        const FileTable& getFiles() const { return m_files; }
        const IndexedObject::Array& getObjects() const { return m_objects; }
        
        IndexedObject::Array& getObjects() { return m_objects; }
            
    private:
        static void beginLoadingFile( ObjectFactory& objectFactory, FileIDToFileMap& fileMap,
            const boost::filesystem::path& filePath, IndexedObject::FileID fileID );
        void beginLoad( ObjectFactory& objectFactory, FileIDToFileMap& fileMap );
        void endLoad();
    
    private:
        const boost::filesystem::path m_filePath;
        const IndexedObject::FileID m_fileID;
        FileTable m_files;
        IndexedObject::Array m_objects;
        
        std::shared_ptr< Loader > m_pLoader;
    };

}


#endif //INDEXED_FILE_18_04_2019