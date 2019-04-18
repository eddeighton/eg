
#ifndef EG_OBJECTS_18_04_2019
#define EG_OBJECTS_18_04_2019

#include "io/indexed_object.hpp"

#include <cstddef>
#include <ostream>
#include <istream>

namespace eg
{
    enum ObjectType : std::size_t
    {
        eInputOpaque,
        eInputDimension,
        eInputInclude,
        eInputAction,
        eInputRoot,

        eAbstractOpaque,
        eAbstractDimension,
        eAbstractInclude,
        eAbstractAction,
        eAbstractRoot,
        
        eInheritanceNode,
        
        eInstanceAction,
        eInstanceDimensionUser,
        eInstanceDimensionGenerated,
        
        eDerivationAnalysis,
        eInvocationSolution,
        
        eDimensionInstance,
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
    };
}

#endif //EG_OBJECTS_18_04_2019