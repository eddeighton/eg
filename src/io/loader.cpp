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
