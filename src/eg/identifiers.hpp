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
        
        using IdentifierMap = std::map< std::string, const abstract::Element* >;
        const IdentifierMap& getIdentifiersMap() const { return m_identifierMap; }
        
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