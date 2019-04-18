
#include "storer.hpp"

#include "common/file.hpp"

namespace eg
{

    Storer::Storer( const boost::filesystem::path& filePath )
        :   m_pFileStream( boost::filesystem::createBinaryOutputFileStream( filePath ) ),
            m_archive( *m_pFileStream )
    {
    }
    
    void Storer::storeObject( const IndexedObject* pObject )
    {
        VERIFY_RTE( pObject );
        pObject->IndexedObject::store( *this );
    }
    
    void Storer::storeObjectRef( const IndexedObject* pObject )
    {
        if( pObject )
        {
            store( pObject->getFileID() );
            store( pObject->getIndex() );
        }
        else
        {
            store( IndexedObject::NO_FILE );
        }
    }



}