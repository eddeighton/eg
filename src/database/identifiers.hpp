
#ifndef IDENTIFIERS_19_04_2019
#define IDENTIFIERS_19_04_2019

#include "abstract.hpp"

#include <io/indexed_object.hpp>

#include <map>

namespace eg
{


    class Identifiers : public IndexedObject
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eIdentifiers;
    protected:
        Identifiers( const IndexedObject& object );
        
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    public:
        void populate( const IndexedObject::Array& objects );
        
        const abstract::Element* isElement( const std::string& strIdentifier ) const;
        std::vector< const abstract::Element* > getGroup( const abstract::Element* pElement ) const;
        std::vector< const abstract::Element* > getGroupBack( const abstract::Element* pElement ) const;
    private:
        struct CompareIdentifiers
        {
            bool operator()( const abstract::Element* pLeft, const abstract::Element* pRight ) const
            {
                return pLeft->getIdentifier() < pRight->getIdentifier();
            }
        };
        std::map< std::string, const abstract::Element* > m_identifierMap;
        using GroupMap = std::multimap< const abstract::Element*, const abstract::Element*, CompareIdentifiers >;
        GroupMap m_identifierGroups;
        
        using GroupBackMap = std::map< const abstract::Element*, const abstract::Element*, CompareIdentifiers >;
        GroupBackMap m_identifierGroupsBack;
    };

}

#endif //IDENTIFIERS_19_04_2019