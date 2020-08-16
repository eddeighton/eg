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


#ifndef TRANSLATIONUNIT_22_04_2020
#define TRANSLATIONUNIT_22_04_2020

#include "eg.hpp"
#include "objects.hpp"
#include "concrete.hpp"
#include "sessions/session.hpp"

#include "eg/common.hpp"

#include "common/file.hpp"

#include <map>
#include <set>
#include <vector>
#include <tuple>

namespace eg
{

    extern std::string style_replace_non_alpha_numeric( const std::string& str, char r );
    //class InvocationSolution;
    class TranslationUnit : public IndexedObject
    {
        friend class ObjectFactoryImpl;
        friend class InterfaceSession;
    public:
        static const ObjectType Type = eTranslationUnit;
    protected:
        TranslationUnit( const IndexedObject& object )
            :   IndexedObject( object )
        {
    
        }
        
    public:
        using ActionSet = std::set< const interface::Context*, CompareIndexedObjects >; 
		
		struct CoordinatorHostnameDefinitionFile
		{
			const interface::Root* pCoordinator = nullptr;
			const interface::Root* pHostName = nullptr;
			std::optional< boost::filesystem::path > definitionFile;
			
			inline bool operator==( const CoordinatorHostnameDefinitionFile& cmp ) const
			{
				return ( pCoordinator 	== cmp.pCoordinator ) &&
					   ( pHostName 		== cmp.pHostName ) &&
					   ( definitionFile == cmp.definitionFile );
			}
			
			inline bool operator<( const CoordinatorHostnameDefinitionFile& cmp ) const
			{
				if( pCoordinator && cmp.pCoordinator && ( pCoordinator != cmp.pCoordinator ) ) 	
					return CompareIndexedObjects()( pCoordinator, cmp.pCoordinator );
				if( pHostName && cmp.pHostName && ( pHostName != cmp.pHostName ) )
					return CompareIndexedObjects()( pHostName, cmp.pHostName);
				return definitionFile < cmp.definitionFile;
			}
			
			inline bool isCoordinator( const std::string& strCoordinator ) const
			{
                return pCoordinator ? ( pCoordinator->getIdentifier() == strCoordinator ) : false;
			}
			
			inline bool isHost( const std::string& strHostName ) const
			{
                return pHostName ? ( pHostName->getIdentifier() == strHostName ) : false;
			}
            
            std::string getHostDefine() const
            {
                std::ostringstream os;
                if( pCoordinator && pHostName )
                {
                    os << pCoordinator->getIdentifier() << '_' << pHostName->getIdentifier();
                }
                return os.str(); 
            }
		};
		
		const CoordinatorHostnameDefinitionFile& getCoordinatorHostnameDefinitionFile() const
		{
			return m_coordinatorHostnameDefinitionFile;
		}
		
    protected:
		CoordinatorHostnameDefinitionFile m_coordinatorHostnameDefinitionFile;
        std::string m_strName;
        IndexedObject::FileID m_databaseFileID;
        ActionSet m_actions;
        
    public:
        static std::string TUNameFromEGSource( 
			const boost::filesystem::path& projectFolder, 
			const CoordinatorHostnameDefinitionFile& coordinatorHostnameDefinitionFile )
        {
			if( coordinatorHostnameDefinitionFile.definitionFile )
			{
				boost::filesystem::path relativePath = boost::filesystem::edsInclude( projectFolder, coordinatorHostnameDefinitionFile.definitionFile.value() );
				return style_replace_non_alpha_numeric( relativePath.replace_extension().generic_string(), '_' );
			}
			else
			{
				std::ostringstream os;
				if( coordinatorHostnameDefinitionFile.pCoordinator )
					os << coordinatorHostnameDefinitionFile.pCoordinator->getIdentifier() << "_";
				if( coordinatorHostnameDefinitionFile.pHostName )
					os << coordinatorHostnameDefinitionFile.pHostName->getIdentifier() << "_";
				os << "definition";
				return os.str();
			}
        }
        
        
        const std::string& getName() const { return m_strName; }
        IndexedObject::FileID getDatabaseFileID() const { return m_databaseFileID; }
        const ActionSet& getActions() const { return m_actions; }
        
        bool isAction( const interface::Context* pAction ) const
        {
            return m_actions.count( pAction ) > 0U;
        }
        //using Invocations = std::vector< const InvocationSolution* >;
        //Invocations invocations;
        
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
        
    };
    
    
    
    class TranslationUnitAnalysis : public IndexedObject
    {
        friend class ObjectFactoryImpl;
        friend class InterfaceSession;
    public:
        static const ObjectType Type = eTranslationUnitAnalysis;
    protected:
        TranslationUnitAnalysis( const IndexedObject& object )
            :   IndexedObject( object )
        {
    
        }
    public:
        using TranslationUnits = std::vector< TranslationUnit* >;
		using ActionTUMap = std::map< const interface::Context*, const TranslationUnit*, CompareIndexedObjects >;
    private:
        TranslationUnits m_translationUnits;
        ActionTUMap m_actionTUMap;
    public:
		const TranslationUnit* getActionTU( const interface::Context* pAction ) const;
		
        const TranslationUnits& getTranslationUnits() const { return m_translationUnits; }
    public:
        virtual void load( Loader& loader );
        virtual void store( Storer& storer ) const;
    };

}

#endif //TRANSLATIONUNIT_22_04_2020
