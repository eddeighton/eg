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


#ifndef STORER_18_04_2019
#define STORER_18_04_2019

#include "archive.hpp"
#include "indexed_object.hpp"

#include "common/assert_verify.hpp"

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/archive/binary_oarchive.hpp>

#include <memory>
#include <optional>
#include <set>
#include <map>
#include <vector>

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
        inline void storeOptional( const std::optional< T >& value )
        {
            if( value )
            {
                m_archive << true;
                store( value.value() );
            }
            else
            {
                m_archive << false;
            }
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
        
        template< class T, class TPred >
        inline void storeObjectSet( const std::set< T*, TPred >& objects )
        {
            std::size_t szCount = objects.size();
            store( szCount );
            for( std::set< T* >::const_iterator 
                i = objects.begin(), 
                iEnd = objects.end(); i!=iEnd; ++i )
            {
                storeObjectRef( *i );
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