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


#ifndef SESSION_18_04_2019
#define SESSION_18_04_2019

#include "../objects.hpp"

#include "io/indexed_object.hpp"

namespace eg
{

    class Session
    {
    protected:
        Session( const IndexedFile::FileIDtoPathMap& files );
        Session( const boost::filesystem::path& filePath, IndexedObject::FileID fileID );
        virtual ~Session();
        
    public:
        virtual const IndexedObject::Array& getObjects( IndexedObject::FileID fileID ) const
        {
            IndexedFile::FileIDToFileMap::const_iterator iFind = m_fileMap.find( fileID );
            VERIFY_RTE( iFind != m_fileMap.end() );
            return iFind->second->getObjects();
        }
        
        const IndexedObject::Array& getMaster() const 
        { 
            return getObjects( IndexedObject::MASTER_FILE );
        }
        
    protected:
        IndexedFile::FileIDToFileMap m_fileMap;
    };
    
    class AppendingSession : public Session
    {
    public:
        AppendingSession( const boost::filesystem::path& filePath, IndexedObject::FileID fileID );
        
        void store() const;
        
        IndexedObject::Array& getAppendingObjects() const
        {
            IndexedFile::FileIDToFileMap::const_iterator iFind = m_fileMap.find( IndexedObject::MASTER_FILE );
            VERIFY_RTE( iFind != m_fileMap.end() );
            IndexedFile* pFile = iFind->second;
            return pFile->getObjects();
        }
    public:
        template< typename T, typename... Args >
        inline T* construct( Args... args )
        {
            typename IndexedFile::FileIDToFileMap::const_iterator iFind = m_fileMap.find( IndexedObject::MASTER_FILE );
            VERIFY_RTE( iFind != m_fileMap.end() );
            IndexedFile* pFile = iFind->second;
            T* pNewObject = ObjectFactoryImpl::create< T >( 
                IndexedObject::MASTER_FILE, pFile->getObjects().size(), args... );
            VERIFY_RTE( pNewObject );
            pFile->getObjects().push_back( pNewObject );
            return pNewObject;
        }
    };
    
    class CreatingSession : public Session
    {
    protected:
        CreatingSession( const IndexedFile::FileIDtoPathMap& files, IndexedObject::FileID fileID );
        ~CreatingSession();
        
    public:
        void store( const boost::filesystem::path& filePath ) const;
        
        template< typename T, typename... Args >
        inline T* construct( Args... args )
        {
            T* pNewObject = ObjectFactoryImpl::create< T >( m_fileID, m_newObjects.size(), args... );
            VERIFY_RTE( pNewObject );
            m_newObjects.push_back( pNewObject );
            return pNewObject;
        }
        const IndexedObject::Array& getNewObjects() const { return m_newObjects; }
        
        const IndexedObject::Array& getObjects( IndexedObject::FileID fileID ) const
        {
            if( fileID == m_fileID ) 
                return m_newObjects;
            else
                return Session::getObjects( fileID );
        }
    protected:
        IndexedObject::FileID m_fileID;
        IndexedObject::Array m_newObjects;
    };


}

#endif //SESSION_18_04_2019