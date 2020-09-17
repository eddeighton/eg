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



#include "eg_compiler/translation_unit.hpp"

namespace eg
{
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void TranslationUnit::load( Loader& loader )
    {
        m_chd.pCoordinator = loader.loadObjectRef< interface::Root >();
        m_chd.pHostName = loader.loadObjectRef< interface::Root >();
        loader.loadOptional( m_chd.definitionFile );
        loader.load( m_strName );
        loader.load( m_databaseFileID );
        loader.loadObjectSet( m_actions );
    }
    
    void TranslationUnit::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_chd.pCoordinator );
        storer.storeObjectRef( m_chd.pHostName );
        storer.storeOptional( m_chd.definitionFile );
        storer.store( m_strName );
        storer.store( m_databaseFileID );
        storer.storeObjectSet( m_actions );
    }
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void TranslationUnitAnalysis::load( Loader& loader )
    {
        loader.loadObjectVector( m_translationUnits );
		loader.loadObjectMap( m_actionTUMap );
    }
    void TranslationUnitAnalysis::store( Storer& storer ) const
    {
        storer.storeObjectVector( m_translationUnits );
		storer.storeObjectMap( m_actionTUMap );
    }
    
	const TranslationUnit* TranslationUnitAnalysis::getActionTU( const interface::Context* pAction ) const
	{
		ActionTUMap::const_iterator iFind = m_actionTUMap.find( pAction );
		if( iFind != m_actionTUMap.end() )
			return iFind->second;
		return nullptr;
	}
    
    const TranslationUnit* TranslationUnitAnalysis::getTU( const boost::filesystem::path& sourceFile ) const
    {
        for( const TranslationUnit* pTranslationUnit : m_translationUnits )
        {
            std::optional< boost::filesystem::path > optFile = pTranslationUnit->getDefinitionFile();
            if( optFile )
            {
                if( optFile.value() == sourceFile )
                {
                    return pTranslationUnit;
                }
            }
        }
        return nullptr;
    }

} //namespace eg