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
        m_coordinatorHostnameDefinitionFile.pCoordinator = loader.loadObjectRef< interface::Root >();
        m_coordinatorHostnameDefinitionFile.pHostName = loader.loadObjectRef< interface::Root >();
        loader.loadOptional( m_coordinatorHostnameDefinitionFile.definitionFile );
        loader.load( m_strName );
        loader.load( m_databaseFileID );
        loader.loadObjectSet( m_actions );
    }
    
    void TranslationUnit::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_coordinatorHostnameDefinitionFile.pCoordinator );
        storer.storeObjectRef( m_coordinatorHostnameDefinitionFile.pHostName );
        storer.storeOptional( m_coordinatorHostnameDefinitionFile.definitionFile );
        storer.store( m_strName );
        storer.store( m_databaseFileID );
        storer.storeObjectSet( m_actions );
    }
    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    void TranslationUnitAnalysis::load( Loader& loader )
    {
        loader.loadObjectVector( m_translationUnits );
    }
    void TranslationUnitAnalysis::store( Storer& storer ) const
    {
        storer.storeObjectVector( m_translationUnits );
    }
    

} //namespace eg