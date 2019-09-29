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



#include "session.hpp"

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
            IndexedFile::store( pFile->getPath(), pFile->getFileID(), files, pFile->getObjects() );
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
        IndexedFile::store( filePath, m_fileID, files, m_newObjects );
    }
        


}