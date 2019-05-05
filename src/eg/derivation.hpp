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


#ifndef DERIVATION_18_04_2019
#define DERIVATION_18_04_2019

#include "eg.hpp"
#include "objects.hpp"
#include "concrete.hpp"
#include "session.hpp"

#include "runtime/eg_common.hpp"

#include <map>
#include <set>
#include <vector>
#include <tuple>

namespace eg
{
    template< class T >
    inline std::vector< T > uniquify_without_reorder( const std::vector< T >& ids )
    {
        /*
        not this...
        std::sort( ids.begin(), ids.end() );
        auto last = std::unique( ids.begin(), ids.end() );
        ids.erase( last, ids.end() );
        */
        
        std::vector< T > result;
        std::set< T > uniq;
        for( const T& value : ids )
        {
            if( uniq.count( value ) == 0 )
            {
                result.push_back( value );
                uniq.insert( value );
            }
        }
        return result;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    class DerivationAnalysis : public IndexedObject
    {
        friend class ObjectFactoryImpl;
    public:
        static const ObjectType Type = eDerivationAnalysis;
    protected:
        DerivationAnalysis( const IndexedObject& object )
            :   IndexedObject( object )
        {
    
        }
        
    public:
    
        using InstanceMap = std::multimap< const abstract::Element*, const concrete::Element* >;
        InstanceMap m_instanceMap;
        
        using InheritanceNodeMap = std::multimap< const abstract::Action*, const concrete::Inheritance_Node* >;
        InheritanceNodeMap m_inheritanceMap;
        
        void getInstances( const abstract::Element* pElement, std::vector< const concrete::Element* >& instances, bool bDeriving ) const
        {
            const abstract::Action* pAction = dynamic_cast< const abstract::Action* >( pElement );
            if( bDeriving && pAction )
            {
                InheritanceNodeMap::const_iterator iLower = m_inheritanceMap.lower_bound( pAction );
                InheritanceNodeMap::const_iterator iUpper = m_inheritanceMap.upper_bound( pAction );
                for( ; iLower != iUpper; ++iLower )
                    instances.push_back( iLower->second->getRootConcreteAction() );
            }
            else
            {
                InstanceMap::const_iterator iLower = m_instanceMap.lower_bound( pElement );
                InstanceMap::const_iterator iUpper = m_instanceMap.upper_bound( pElement );
                for( ; iLower != iUpper; ++iLower )
                    instances.push_back( iLower->second );
            }
        }
        
        void getInheritanceNodes( const abstract::Action* pAction, std::vector< const concrete::Inheritance_Node* >& inheritanceNodes ) const
        {
            InheritanceNodeMap::const_iterator iLower = m_inheritanceMap.lower_bound( pAction );
            InheritanceNodeMap::const_iterator iUpper = m_inheritanceMap.upper_bound( pAction );
            for( ; iLower != iUpper; ++iLower )
                inheritanceNodes.push_back( iLower->second );
        }
        
        using InstancePair = std::pair< const concrete::Element*, const concrete::Element* >;
        using CommonRootMap = std::map< InstancePair, const concrete::Element* >;
        CommonRootMap m_commonRoots;
        
        
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    };
    
}

#endif //DERIVATION_18_04_2019