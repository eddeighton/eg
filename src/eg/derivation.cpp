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



#include "derivation.hpp"

namespace eg
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void DerivationAnalysis::load( Loader& loader )
    {
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const interface::Element*     pElement   = loader.loadObjectRef< const interface::Element >();
                const concrete::Element*     pInstance  = loader.loadObjectRef< concrete::Element >();
                m_instanceMap.insert( std::make_pair( pElement, pInstance ) );
            }
        }
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                const interface::Action*             pAction          = loader.loadObjectRef< const interface::Action >();
                const concrete::Inheritance_Node*   pInheritanceNode = loader.loadObjectRef< const concrete::Inheritance_Node >();
                m_inheritanceMap.insert( std::make_pair( pAction, pInheritanceNode ) );
            }
        }
    }
    void DerivationAnalysis::store( Storer& storer ) const
    {
        {
            const std::size_t szSize = m_instanceMap.size();
            storer.store( szSize );
            for( InstanceMap::const_iterator 
                i = m_instanceMap.begin(),
                iEnd = m_instanceMap.end(); i!=iEnd; ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectRef( i->second );
            }
        }
        
        {
            const std::size_t szSize = m_inheritanceMap.size();
            storer.store( szSize );
            for( InheritanceNodeMap::const_iterator 
                i = m_inheritanceMap.begin(),
                iEnd = m_inheritanceMap.end(); i!=iEnd; ++i )
            {
                storer.storeObjectRef( i->first );
                storer.storeObjectRef( i->second );
            }
        }
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void TranslationUnitAnalysis::load( Loader& loader )
    {
        {
            std::size_t szSize = 0;
            loader.load( szSize );
            for( std::size_t sz = 0; sz != szSize; ++sz )
            {
                boost::filesystem::path thePath;
                loader.load( thePath );
                
                TranslationUnit actions;
                loader.loadObjectSet( actions );
                
                m_translationUnits.insert( std::make_pair( thePath, actions ) );
            }
        }
        
    }
    void TranslationUnitAnalysis::store( Storer& storer ) const
    {
        {
            const std::size_t szSize = m_translationUnits.size();
            storer.store( szSize );
            for( TranslationUnitMap::const_iterator 
                i = m_translationUnits.begin(),
                iEnd = m_translationUnits.end(); i!=iEnd; ++i )
            {
                storer.store( i->first );
                storer.storeObjectSet( i->second );
            }
        }
    }
    

} //namespace eg