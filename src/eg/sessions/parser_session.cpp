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



#include "eg_compiler/sessions/parser_session.hpp"
#include "parser/parser.hpp"

#include "eg_compiler/eg.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/interface.hpp"
#include "eg_compiler/identifiers.hpp"

#include <boost/dll/import.hpp>
#include <boost/algorithm/string.hpp>

namespace eg
{
    ParserSession::ParserSession( EG_PARSER_CALLBACK* pParserCallback,
                const boost::filesystem::path& parserDLLPath, 
                const boost::filesystem::path& currentPath, 
                std::ostream& os )
        :   CreatingSession( IndexedFile::FileIDtoPathMap{}, IndexedObject::MASTER_FILE ),
            m_pParserCallback( pParserCallback ),
            m_parserDllPath( parserDLLPath ),
            m_currentPath( currentPath ),
            m_errorOS( os )
    {
        VERIFY_RTE( m_pParserCallback );
    }
    
    void ParserSession::parseEGSourceFile( const boost::filesystem::path& egSourceFile,
                ParserSession& session, input::Root* pRoot )
    {
        static boost::shared_ptr< eg::EG_PARSER_INTERFACE > pParserInterface;
        
        if( !pParserInterface )
        {
            pParserInterface =
                boost::dll::import< eg::EG_PARSER_INTERFACE >( 
                    m_parserDllPath, "g_parserSymbol" );
        }
        
        VERIFY_RTE_MSG( pParserInterface, "Failed to locate eg parser at: " << m_parserDllPath.string() );
            
        //record the source file parsed for this root
        pRoot->m_sourceFile = egSourceFile;
        
        pParserInterface->parseEGSourceFile( m_pParserCallback, egSourceFile, m_currentPath, m_errorOS, session, pRoot );
        
    }
	
	input::Root* ParserSession::getMegaRoot( 
		input::Root* pMegaStructureRoot,
		const SourceCodeTree::RootFolder& rootFolder,
		const SourceCodeTree::ProjectNameFolder& projectNameFolder,
		std::map< boost::filesystem::path, input::Root* >& rootTree )
	{
		boost::filesystem::path::const_iterator
			i = rootFolder.begin(), iEnd = rootFolder.end(),
			j = projectNameFolder.begin(), jEnd = projectNameFolder.end();
			
		//iterate until they become different
		for( ; i!=iEnd && j!=jEnd && *i == *j; ++i, ++j ){}
		
		VERIFY_RTE_MSG( i == iEnd, 
			"Project name folder: " << projectNameFolder.string() << 
			" does NOT exist within mega structure root folder: " << rootFolder.string() );
			
		input::Root* pRoot = pMegaStructureRoot;
		{
			boost::filesystem::path treePath = rootFolder;
			int iFolderDepth = 0;
			for( ; j!=jEnd; ++j, ++iFolderDepth )
			{
				treePath = treePath / *j;
		
				input::Root* pNestedRoot = nullptr;
		
				std::map< boost::filesystem::path, input::Root* >::iterator 
					iFind = rootTree.find( treePath );
				if( iFind != rootTree.end() )
				{
					pNestedRoot = iFind->second;
				}
				else
				{
					pNestedRoot = construct< input::Root >();
					{
						pNestedRoot->m_strIdentifier = j->string();
						switch( iFolderDepth )
						{
							case 0:
								pNestedRoot->m_rootType = eCoordinator;
								break;
							case 1:
								pNestedRoot->m_rootType = eHostName;
								break;
							case 2:
								pNestedRoot->m_rootType = eProjectName;
								break;
							default:
                                THROW_RTE( "TODO" );
								pNestedRoot->m_rootType = eSubFolder;
								break;
						}
					}
					pRoot->m_elements.push_back( pNestedRoot );
					
					rootTree.insert( std::make_pair( treePath, pNestedRoot ) );
				}
				
				pRoot = pNestedRoot;
			}
		}
		
		return pRoot;
	}
	
	void ParserSession::parse( const SourceCodeTree& egSourceCodeFiles )
	{
		input::Root* pMegaStructureRoot = construct< input::Root >();
		{
			pMegaStructureRoot->m_rootType = eMegaRoot;
		}
			
        std::set< boost::filesystem::path > includePaths;
		std::map< boost::filesystem::path, input::Root* > rootTree;
		
		for( SourceCodeTree::FileMap::const_iterator 
			i = egSourceCodeFiles.files.begin(),
			iEnd = egSourceCodeFiles.files.end();
			i != iEnd; ++i )
		{
			const SourceCodeTree::ProjectNameFolder& projectNameFolder = i->first;
			const SourceCodeTree::EGSourceFile& egSourceFile = i->second;
			
			boost::filesystem::path sourceFile = projectNameFolder / egSourceFile;
			
			input::Root* pRoot = getMegaRoot( pMegaStructureRoot, egSourceCodeFiles.root, projectNameFolder, rootTree );
			
            parseEGSourceFile( sourceFile, *this, pRoot ); //parse main root
            includePaths.insert( sourceFile );
		}
		
		handleInputIncludes( includePaths );
	}
	
    void ParserSession::parse( const std::vector< boost::filesystem::path >& egSourceCodeFiles )
    {
		input::Root* pRoot = construct< input::Root >();
		{
			pRoot->m_rootType = eFileRoot;
		}
		
        std::set< boost::filesystem::path > includePaths;
        for( const boost::filesystem::path& filePath : egSourceCodeFiles )
        {
            parseEGSourceFile( filePath, *this, pRoot ); //parse main root
            includePaths.insert( filePath );
        }
        
		handleInputIncludes( includePaths );
    }
    
	void ParserSession::handleInputIncludes( std::set< boost::filesystem::path >& includePaths )
	{
        std::set< boost::filesystem::path > newIncludePaths;
        
        //greedy parse all includes 
        do
        {
            for( const boost::filesystem::path& includePath : newIncludePaths )
            {
				input::Root* pIncludeRoot = construct< input::Root >();
				{
					pIncludeRoot->m_includePath = includePath;
					pIncludeRoot->m_rootType = eFile;
				}
				
                parseEGSourceFile( includePath, *this, pIncludeRoot ); //parse include - non-main root
                includePaths.insert( includePath );
            }
            newIncludePaths.clear();
            
            std::vector< input::Include* > includes = many< input::Include >( getMaster() );
            for( input::Include* pInclude : includes )
            {
                if( pInclude->isEGInclude() )
                {
                    const boost::filesystem::path& filePath = pInclude->getIncludeFilePath();
                    if( includePaths.find( filePath ) == includePaths.end() )
                        newIncludePaths.insert( filePath );
                }
            }
        }while( !newIncludePaths.empty() );
	}
    
    interface::Element* addChild( ParserSession& session, interface::Element* pParent, input::Element* pElement, VisibilityType visibility )
    {
        interface::Element* pNewNode = nullptr;
        switch( pElement->getType() )
        {
            case eInputOpaque:         pNewNode = session.construct< interface::Opaque >(    pParent, pElement, visibility ); break;
            case eInputDimension:      pNewNode = session.construct< interface::Dimension >( pParent, pElement, visibility ); break;
            case eInputInclude:        pNewNode = session.construct< interface::Include >(   pParent, pElement, visibility ); break;
            case eInputUsing:          pNewNode = session.construct< interface::Using >(     pParent, pElement, visibility ); break;
            case eInputExport:         pNewNode = session.construct< interface::Export >(    pParent, pElement, visibility ); break;
            case eInputVisibility:     THROW_RTE( "Invalid attempt to construct interface visibility" );
            case eInputContext:
            {
                input::Context* pContext = dynamic_cast< input::Context* >( pElement );
                VERIFY_RTE( pContext );
                switch( pContext->m_contextType )
                {
                    case input::Context::eAbstract : pNewNode = session.construct< interface::Abstract > ( pParent, pElement, visibility ); break;
                    case input::Context::eEvent    : pNewNode = session.construct< interface::Event >    ( pParent, pElement, visibility ); break;
                    case input::Context::eFunction : pNewNode = session.construct< interface::Function > ( pParent, pElement, visibility ); break;
                    case input::Context::eAction   : pNewNode = session.construct< interface::Action >   ( pParent, pElement, visibility ); break;
                    case input::Context::eLink     : pNewNode = session.construct< interface::Link >     ( pParent, pElement, visibility ); break;
                    case input::Context::eObject   : pNewNode = session.construct< interface::Object >   ( pParent, pElement, visibility ); break;
                    case input::Context::eUnknown  : 
                    default:
                    {
                        THROW_RTE( "Undefined context type for: " << pParent->getFriendlyName() << "::" << pContext->getIdentifier() );
                    }
                }
                break;
            }
            case eInputRoot:           
                pNewNode = session.construct< interface::Root >( pParent, pElement, visibility ); break;
            default:
                THROW_RTE( "Unsupported type" );
                break;
        }
        VERIFY_RTE( pNewNode );
        return pNewNode;
    }
    
    void ParserSession::buildTree( const FileElementMap& fileMap, interface::Element* pParentNode, 
        input::Element* pElement,
		std::optional< boost::filesystem::path > includeDefinitionFile, 
		bool bInIncludeTree, 
        VisibilityType visibility )
    {
        switch( pElement->getType() )
        {
            case eInputOpaque    :
                {
                    //do nothing
                }
                break;
            case eInputDimension :
                {
                    //do nothing
                }
                break;
            case eInputInclude   :
                THROW_RTE( "unreachable" );
                break;
            case eInputUsing     :
                {
                    //do nothing
                }
                break;
            case eInputExport    :
                {
                    //do nothing
                }
                break;
            case eInputVisibility:
                THROW_RTE( "unreachable" );
                break;
            case eInputContext   :
            case eInputRoot      :
                {
                    input::Context* pContext = dynamic_cast< input::Context* >( pElement );
                    VERIFY_RTE( pContext );
                    
                    for( input::Element* pChildElement : pContext->getElements() )
                    {
                        if( input::Include* pInclude = dynamic_cast< input::Include* >( pChildElement ) )
                        {
                            if( pInclude->isEGInclude() )
                            {
                                FileElementMap::const_iterator iFind = 
                                    fileMap.find( pInclude->getIncludeFilePath() );
                                VERIFY_RTE_MSG( iFind != fileMap.end(), "Failed to find include file: " << 
                                    pInclude->getIncludeFilePath().string() << 
                                    " NOTE: you may have put include .eg file in root folder" );
                                input::Root* pIncludedRoot = iFind->second;
                                
                                if( pInclude->getIdentifier().empty() )
                                {
                                    //if the include has no identifier then insert the included root elements
                                    buildTree( fileMap, pParentNode, pIncludedRoot, includeDefinitionFile, true, visibility );
                                }
                                else
                                {
                                    //otherwise insert the root with the include identifier
                                    interface::Element* pChild = addChild( *this, pParentNode, pIncludedRoot, visibility );
                                    pChild->pIncludeIdentifier = pInclude;
                                    buildTree( fileMap, pChild, pIncludedRoot, includeDefinitionFile, true, visibility );
                                } 
                            }
                            else
                            {
                                addChild( *this, pParentNode, pChildElement, visibility );
                            }
                        }
                        else if( input::Context* pElementAction = dynamic_cast< input::Context* >( pChildElement ) )
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement, visibility );
                            
                            if( bInIncludeTree )
                            {
                                //if the action is defined then set the definition file to the include definition file
                                if( includeDefinitionFile )
                                {
                                    ( (interface::Context*)pChild )->setDefinitionFile( includeDefinitionFile );
                                }
                                else
                                {
                                    THROW_RTE( "Include file problem" );
                                    //if( pElementAction->getDefinitionFile() )
                                    //{
                                    //    ( (interface::Context*)pChild )->setDefinitionFile( pElementAction->getDefinitionFile() );
                                    //} 
                                }
								buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, true, visibility );
                            }
                            else
                            {
                                //if the action is defined then use its definition file for any include sub trees
                                if( pElementAction->getDefinitionFile() )
                                {
                                    ( (interface::Context*)pChild )->setDefinitionFile( pElementAction->getDefinitionFile().value() );
                                }
                                buildTree( fileMap, pChild, pChildElement, pElementAction->getSourceFile(), false, visibility );
                            }
                        }
                        else if( input::Export* pElementExport = dynamic_cast< input::Export* >( pChildElement ) )
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement, visibility );
                            buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, bInIncludeTree, visibility );
                        }
                        else if( input::Visibility* pVisibility = dynamic_cast< input::Visibility* >( pChildElement ) )
                        {
                            visibility = pVisibility->m_visibility;
                        }
                        else
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement, visibility );
                            buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, bInIncludeTree, visibility );
                        }
                    }
                }
                break;
            default:
                THROW_RTE( "Unsupported type" );
                break;
        }
    }
    
    void ParserSession::buildAbstractTree()
    {
        interface::Root* pMasterRoot = construct< interface::Root >(); //eTreeRoot
		
        std::vector< input::Root* > roots = many< input::Root >( getMaster() );
        VERIFY_RTE( !roots.empty() );
        
        input::Root* pInputMainRoot = nullptr;
        
        FileElementMap fileMap;
        for( input::Root* pRootElement : roots )
        {
			std::optional< boost::filesystem::path > includePathOpt = 
				pRootElement->getIncludePath();
			
			if( eMegaRoot == pRootElement->getRootType() || 
				eFileRoot == pRootElement->getRootType() )
			{
				VERIFY_RTE( !includePathOpt );
				VERIFY_RTE( !pInputMainRoot );
				pInputMainRoot = pRootElement;
			}
            else if( includePathOpt )
            {
                fileMap.insert( std::make_pair( includePathOpt.value(), pRootElement ) );
            }
        }
        
        VERIFY_RTE( pInputMainRoot );
        interface::Element* pInterfaceRoot = addChild( *this, pMasterRoot, pInputMainRoot, eVisPublic );
        ( (interface::Context*)pInterfaceRoot )->setDefinitionFile( pInputMainRoot->getDefinitionFile() );
        
        buildTree( fileMap, pInterfaceRoot, pInputMainRoot, pInputMainRoot->getSourceFile(), false, eVisPublic );
                
        //create the identifiers object
        Identifiers* pIdentifiers = construct< Identifiers >();
        pIdentifiers->populate( getMaster() );
    }
    
    
} //namespace eg
