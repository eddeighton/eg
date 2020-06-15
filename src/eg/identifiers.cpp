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


#include "eg_compiler/identifiers.hpp"

namespace eg
{

    Identifiers::Identifiers( const IndexedObject& object )
        :   IndexedObject( object )
    {

    }
    
    void Identifiers::populate( const IndexedObject::Array& objects )
    {
        std::multimap< std::string, const interface::Element* > forwardDeclMap;
        for( IndexedObject* pObject : objects )
        {
            switch( pObject->getType() )
            {   
                case eAbstractOpaque    :
                case eAbstractInclude   :
                case eAbstractUsing     :
                case eAbstractExport    :
                    break;
                case eAbstractDimension :
                case eAbstractAbstract  :
                case eAbstractEvent     :
                case eAbstractFunction  :
                case eAbstractAction    :
                case eAbstractObject    :
                case eAbstractLink      :
                case eAbstractRoot      :
                    {
                        const interface::Element* pElement = dynamic_cast< const interface::Element* >( pObject );
                        VERIFY_RTE( pElement );
                        forwardDeclMap.insert( 
                            std::make_pair( pElement->getIdentifier(), pElement ) );
                    }
                    break;
                
                case eInputOpaque:
                case eInputDimension:
                case eInputInclude:
                case eInputUsing:
                case eInputExport:
                
                case eInputContext:
                case eInputRoot:
                
                case eInheritanceNode:
                
                case eConcreteAction:
                case eConcreteDimensionUser:
                case eConcreteDimensionGenerated:
                
                case eIdentifiers:
                case eDerivationAnalysis:
                case eLinkGroup:
                case eLinkAnalysis:
                case eTranslationUnit:
                case eTranslationUnitAnalysis:
                case eInvocationSolution:
                
                case eDataMember:
                case eBuffer:
                case eLayout:
                    break;
                default:
                    THROW_RTE( "Unknown abstract type" );
            }
        }
        
        for( std::multimap< std::string, const interface::Element* >::iterator 
            i = forwardDeclMap.begin(),
            iEnd = forwardDeclMap.end();
            i!=iEnd;  )
        {
            std::multimap< std::string, const interface::Element* >::iterator 
                iUpper = forwardDeclMap.upper_bound( i->first );
                
            std::set< const interface::Element* > elements;
            const interface::Element* pFirst = nullptr;
            for( ; i!=iUpper; ++i )
            {
                elements.insert( i->second );
                if( pFirst == nullptr )
                    pFirst = i->second;
            }
            
            ASSERT( !elements.empty() );
            for( const interface::Element* pElement : elements )
            {
                m_identifierMap.insert( std::make_pair( pFirst->getIdentifier(), pFirst ) );
                m_identifierGroups.insert( std::make_pair( pFirst, pElement ) );
                m_identifierGroupsBack.insert( std::make_pair( pElement, pFirst ) );
            }
        }
    }

    void Identifiers::load( Loader& loader )
    {
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            std::string str;
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                loader.load( str );
                const interface::Element* pObject = loader.loadObjectRef< const interface::Element >();
                m_identifierMap.insert( std::make_pair( str, pObject ) );
            }
        }
        
        
        loader.loadObjectMap( m_identifierGroups );
        loader.loadObjectMap( m_identifierGroupsBack );
    }
    
    void Identifiers::store( Storer& storer ) const
    {
        {
            std::size_t szSize = m_identifierMap.size();
            storer.store( szSize );
            for( std::map< std::string, const interface::Element* >::const_iterator 
                i = m_identifierMap.begin(), 
                iEnd = m_identifierMap.end(); i!=iEnd; ++i )
            {
                storer.store( i->first );
                storer.storeObjectRef( i->second );
            }
        }
            
        storer.storeObjectMap( m_identifierGroups );
        storer.storeObjectMap( m_identifierGroupsBack );
    }

    const interface::Element* Identifiers::isElement( const std::string& strIdentifier ) const
    {
        std::map< std::string, const interface::Element* >::const_iterator 
            iFind = m_identifierMap.find( strIdentifier );
        if( iFind != m_identifierMap.end() )
        {
            return iFind->second;
        }
        else
        {
            return nullptr;
        }
    }
    
    std::vector< const interface::Element* > Identifiers::getGroup( const interface::Element* pElement ) const
    {
        std::vector< const interface::Element* > result;
        GroupMap::const_iterator iLower = m_identifierGroups.lower_bound( pElement );
        GroupMap::const_iterator iUpper = m_identifierGroups.upper_bound( pElement );
        for( ; iLower != iUpper; ++iLower )
            result.push_back( iLower->second );
        return result;
    }
    
    std::vector< const interface::Element* > Identifiers::getGroupBack( const interface::Element* pElement ) const
    {
        GroupBackMap::const_iterator iFind = m_identifierGroupsBack.find( pElement );
        if( iFind != m_identifierGroupsBack.end() )
        {
            return getGroup( iFind->second );
        }
        else
        {
            return std::vector< const interface::Element* >{};
        }
    }
}