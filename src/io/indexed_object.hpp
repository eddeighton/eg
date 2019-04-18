

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

        using Type = std::size_t;
        static const Type NO_TYPE = (Type)-1;
    
        using Index = std::size_t;
        static const Index NO_INDEX = (Index)-1;
        
        using FileID = std::size_t;
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
        
        virtual void print( std::ostream& os, std::string& strIndent ) const { };
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
    
    struct CompareIndexedObjects
    {
        inline bool operator()( const IndexedObject* pLeft, const IndexedObject* pRight ) const
        {
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
            
        static void load( ObjectFactory& objectFactory, FileIDToFileMap& fileMap,
            const boost::filesystem::path& filePath, IndexedObject::FileID fileID );

        static void store( const boost::filesystem::path& filePath, 
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