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


#ifndef EG_OBJECTS_18_04_2019
#define EG_OBJECTS_18_04_2019

#include "io/indexed_object.hpp"

#include <cstddef>
#include <ostream>
#include <istream>

namespace eg
{
	
	enum RootType : std::size_t
	{
		eInterfaceRoot,
		eFile,
		eFileRoot,
		eMegaRoot,
		eCoordinator,
		eHostName,
		eProjectName,
		eSubFolder,
		TOTAL_ROOT_TYPES
	};
	
	static const char* g_RootTypeStrings[ TOTAL_ROOT_TYPES ] = 
	{
		"InterfaceRoot",
		"File",
		"FileRoot",
		"MegaRoot",
		"Coordinator",
		"HostName",
		"ProjectName",
		"SubFolder"
	};

    inline std::ostream& operator<<( std::ostream& os, const RootType type )
    {
		return os << g_RootTypeStrings[ type ];
    }

    inline std::istream& operator<<( std::istream& is, RootType& type )
    {
		std::string str;
		is >> str;
		
		for( std::size_t sz = 0u; sz != TOTAL_ROOT_TYPES; ++sz )
		{
			if( str == g_RootTypeStrings[ sz ] )
			{
				type = static_cast< RootType >( sz );
				return is;
			}
		}
		
		type = TOTAL_ROOT_TYPES;
		return is;
    }
			
    enum ObjectType : std::size_t
    {
        eInputOpaque,
        eInputDimension,
        eInputInclude,
        eInputUsing,
        eInputExport,
        
        eInputContext,
        eInputRoot,

        eAbstractOpaque,
        eAbstractDimension,
        eAbstractInclude,
        eAbstractUsing,
        eAbstractExport,
        
        eAbstractAbstract,
        eAbstractEvent,
        eAbstractFunction,
        eAbstractAction,
        eAbstractObject,
        eAbstractLink,
        eAbstractRoot,
        
        eInheritanceNode,
        
        eConcreteAction,
        eConcreteDimensionUser,
        eConcreteDimensionGenerated,
        
        eIdentifiers,
        eDerivationAnalysis,
        eLinkGroup,
        eLinkAnalysis,
        eTranslationUnit,
        eTranslationUnitAnalysis,
        eInvocationSolution,
        
        eDataMember,
        eBuffer,
        eLayout,

        TOTAL_OBJECT_TYPES
    };

    inline std::ostream& operator<<( std::ostream& os, const ObjectType type )
    {
        os << static_cast< std::size_t >( type );
        return os;
    }

    inline std::istream& operator<<( std::istream& is, ObjectType& type )
    {
        std::size_t sz ;
        is >> sz;
        type = static_cast< ObjectType >( sz );
        return is;
    }
    
    class ObjectFactoryImpl : public ObjectFactory
    {
    public:
        virtual IndexedObject* create( const IndexedObject& object );

        template< class T >
        static inline T* create( IndexedObject::FileID fileID, IndexedObject::Index index )
        {
            ObjectFactoryImpl factory;
            IndexedObject* pNewIndexedObject = factory.create( IndexedObject( T::Type, fileID, index ) );
            VERIFY_RTE( pNewIndexedObject );
            T* pNewObject = dynamic_cast< T* >( pNewIndexedObject );
            VERIFY_RTE( pNewObject );
            return pNewObject;
        }

        template< class T, class TElement, class TNestedElement >
        static inline T* create( IndexedObject::FileID fileID, IndexedObject::Index index, TElement* pParent, TNestedElement* pElement )
        {
            T* pNewObject = new T( IndexedObject( T::Type, fileID, index ), pParent, pElement );
            pParent->m_children.push_back( pNewObject );
            return pNewObject;
        }
        
        template< class T, class... Args >
        static inline T* create( IndexedObject::FileID fileID, IndexedObject::Index index, Args... args )
        {
            return new T( IndexedObject( T::Type, fileID, index ), args... );
        }
    };
}

#endif //EG_OBJECTS_18_04_2019