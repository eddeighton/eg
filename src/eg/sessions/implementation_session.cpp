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


#include "eg_compiler/sessions/implementation_session.hpp"

namespace eg
{
    ImplementationSession::ImplementationSession( const IndexedFile::FileIDtoPathMap& files )
        :   CreatingSession( files, IndexedObject::ANALYSIS_FILE )
    {
    }
    
    const interface::Root* ImplementationSession::getTreeRoot() const
    {
        return root< interface::Root >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const concrete::Action* ImplementationSession::getInstanceRoot() const
    {
        return root< concrete::Action >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const DerivationAnalysis& ImplementationSession::getDerivationAnalysis() const
    {
        return *one< DerivationAnalysis >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const LinkAnalysis& ImplementationSession::getLinkAnalysis() const
    {
        return *one< LinkAnalysis >( getObjects( IndexedObject::MASTER_FILE ) );
    }
        
    const Layout& ImplementationSession::getLayout() const
    {
        return *one< Layout >( getObjects( IndexedObject::ANALYSIS_FILE ) );
    }
        
    const TranslationUnitAnalysis& ImplementationSession::getTranslationUnitAnalysis() const
    {
        return *one< TranslationUnitAnalysis >( getObjects( IndexedObject::ANALYSIS_FILE ) );
    }
	
    void ImplementationSession::getInvocations( std::size_t szTranslationUnitID, Invocations& invocations ) const
    {
        invocations = many_cst< InvocationSolution >( getObjects( szTranslationUnitID ) );
    }
        
    std::string generateName( char prefix, const std::vector< const concrete::Element* >& path )
    {
        std::ostringstream os;
        os << prefix;
        const concrete::Element* pIterLast = nullptr;
        for( const concrete::Element* pIter : path )
        {
            if( pIter->getParent() )
                os << "_" << pIter->getAbstractElement()->getIdentifier();
            pIterLast = pIter;
        }
        VERIFY_RTE( pIterLast );
        //os << "_" << pIterLast->getIndex();
        return os.str();
    }

    std::string generateName( char prefix, const concrete::Element* pElement, const concrete::Element* pFrom )
    {
        std::ostringstream os;
        os << prefix;
        std::vector< const concrete::Element* > path = concrete::getPath( pElement, pFrom );
        const concrete::Element* pIterLast = nullptr;
        for( const concrete::Element* pIter : path )
        {
            os << "_" << pIter->getAbstractElement()->getIdentifier();
            pIterLast = pIter;
        }
        VERIFY_RTE( pIterLast );
        //os << "_" << pIterLast->getIndex();
        return os.str();
    }
     
        
    void ImplementationSession::recurseInstances( std::vector< Buffer* >& buffers, 
            Layout::DimensionMap& dimensionMap, std::size_t szSize, const concrete::Action* pAction )
    {
        std::vector< const concrete::Dimension* > dimensions;
        for( const concrete::Element* pChild : pAction->getChildren() )
        {
            if( const concrete::Dimension* pDimension = 
                dynamic_cast< const concrete::Dimension* >( pChild ) )
            {
                dimensions.push_back( pDimension );
            }
        }
        
        const std::vector< const concrete::Element* > path = concrete::getPath( pAction );
        const std::string strBaseVariable = generateName( 'g', path );
    
        //construct the data members
        std::vector< DataMember* > dataMembers;
        {
            for( const concrete::Dimension* pDimension : dimensions )
            {
                DataMember* pDataMember = nullptr; 
                {
                    if( const concrete::Dimension_User* pUserDim = 
                        dynamic_cast< const concrete::Dimension_User* >( pDimension ) )
                    {
                        pDataMember = construct< DataMember >();
                        pDataMember->m_name = generateName( 'm', pUserDim, pAction );
                    }
                    else if( const concrete::Dimension_Generated* pGenDim =
                        dynamic_cast< const concrete::Dimension_Generated* >( pDimension ) )
                    {
                        switch( pGenDim->getDimensionType() )
                        {
                            case concrete::Dimension_Generated::eActionStopCycle :
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->m_name = strBaseVariable + "_cycle";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionReference    :
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->m_name = strBaseVariable + "_reference";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionState    :
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->m_name = strBaseVariable + "_state";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionAllocator    :
                                {
                                    const concrete::Dimension_Generated* pDimGen = 
                                        dynamic_cast< const concrete::Dimension_Generated* >( pDimension );
									VERIFY_RTE( pDimGen );
                                    const concrete::Action* pDimensionAction = pDimGen->getAction();
									VERIFY_RTE( pDimensionAction );
                                    const concrete::Allocator* pAllocator = pDimensionAction->getAllocator();
									VERIFY_RTE( pAllocator );
                                    
									std::ostringstream osVarName;
                                    osVarName << strBaseVariable << "_" << pDimensionAction->getContext()->getIdentifier() << "_allocator";
                                    
                                    pDataMember = construct< DataMember >();
                                    pDataMember->m_name = osVarName.str();
                                }
                                break;
							case concrete::Dimension_Generated::eLinkReference:
								{
                                    const concrete::Dimension_Generated* pDimGen = 
                                        dynamic_cast< const concrete::Dimension_Generated* >( pDimension );
									const LinkGroup* pLinkGroup = pDimGen->getLinkGroup();
									VERIFY_RTE( pLinkGroup );
									VERIFY_RTE( !pLinkGroup->getLinkName().empty() );
                                    pDataMember = construct< DataMember >();
									std::ostringstream osVarName;
									osVarName << strBaseVariable << "_link_" << pLinkGroup->getLinkName();
                                    pDataMember->m_name = osVarName.str();
								}
                                break;
							case concrete::Dimension_Generated::eLinkReferenceCount:
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->m_name = strBaseVariable + "_link_ref_count";
                                }
                                break;
                            default:
                                THROW_RTE( "Unknown generated dimension type" );
                                break;
                        }
                    }
                    else
                    {
                        THROW_RTE( "Unknown dimension type" );
                    }
                }
                
                if( pDataMember )
                {
                    dataMembers.push_back( pDataMember );
                    pDataMember->m_pDimension = pDimension;
                    dimensionMap.insert( std::make_pair( pDimension, pDataMember ) );
                }
            }
        }
        
        Buffer* pBuffer_Simple = nullptr;
        Buffer* pBuffer_Complex = nullptr;
        
        if( !dataMembers.empty() )
        {
            if( const interface::Root* pRoot = dynamic_cast< const interface::Root* >( pAction->getContext() ) )
            {
                switch( pRoot->getRootType() )
                {
                    case eInterfaceRoot :
                    case eFile          :
                    case eFileRoot      :
                    case eProjectName   :
                        break;
                    case eMegaRoot      :
                    case eCoordinator   :
                    case eHostName      :
                    case eSubFolder     :
                        {
                            std::ostringstream osDataMembers;
                            for( DataMember* pDataMember : dataMembers )
                            {
                                osDataMembers << pDataMember->m_name << "\n";
                            }
                            THROW_RTE( "Root has datamembers with root type is: " << pRoot->getRootType() << "\n" << osDataMembers.str() );
                        }
                        break;
                    default:
                        THROW_RTE( "Unknown root type" );
                }
            }
        }
            
        for( DataMember* pDataMember : dataMembers )
        {
            if( pDataMember->m_pDimension->isSimple() )
            {
                if( !pBuffer_Simple )
                {
                    pBuffer_Simple = construct< Buffer >();
                    buffers.push_back( pBuffer_Simple );
                    pBuffer_Simple->m_pContext = pAction;
                    pBuffer_Simple->m_name      = generateName( 'b', path );
                    pBuffer_Simple->m_variable  = generateName( 'g', path );
                    pBuffer_Simple->m_simple    = true;
                    pBuffer_Simple->m_size      = szSize * pAction->getContext()->getSize();
                }
                
                pDataMember->m_pBuffer = pBuffer_Simple;
                pBuffer_Simple->m_dataMembers.push_back( pDataMember );
            }
            else
            {
                if( !pBuffer_Complex )
                {
                    pBuffer_Complex = construct< Buffer >();
                    buffers.push_back( pBuffer_Complex );
                    pBuffer_Complex->m_pContext = pAction;
                    pBuffer_Complex->m_name     = generateName( 'b', path ) + "_complex";
                    pBuffer_Complex->m_variable = generateName( 'g', path ) + "_complex";
                    pBuffer_Complex->m_simple   = false;
                    pBuffer_Complex->m_size     = szSize * pAction->getContext()->getSize();
                }
                
                pDataMember->m_pBuffer = pBuffer_Complex;
                pBuffer_Complex->m_dataMembers.push_back( pDataMember );
            }
        }
        
        for( const concrete::Element* pChild : pAction->getChildren() )
        {
            if( const concrete::Action* pChildAction = 
                dynamic_cast< const concrete::Action* >( pChild ) )
            {
                recurseInstances( buffers, dimensionMap, szSize * pAction->getContext()->getSize(), pChildAction );
            }
        }
    }
        
    void ImplementationSession::fullProgramAnalysis()
    {
        std::vector< Buffer* > buffers;
        Layout::DimensionMap dimensionMap;
            
        {
            const concrete::Action* pRoot = getInstanceRoot();
            recurseInstances( buffers, dimensionMap, 1, pRoot );
        }
        
        Layout* pLayout = construct< Layout >();
        pLayout->m_buffers = std::move( buffers );
        for( Layout::DimensionMap::iterator 
            i = dimensionMap.begin(),
            iEnd = dimensionMap.end(); i!=iEnd; ++i )
        {
            pLayout->m_dimensionMap.insert( *i );
        }
        
    }

    ReadSession::ReadSession( const boost::filesystem::path& filePath )
        :   Session( filePath, IndexedObject::ANALYSIS_FILE )
    {
    }
    
    const interface::Root* ReadSession::getTreeRoot() const
    {
        return root< interface::Root >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const concrete::Action* ReadSession::getInstanceRoot() const
    {
        return root< concrete::Action >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const DerivationAnalysis& ReadSession::getDerivationAnalysis() const
    {
        return *one< DerivationAnalysis >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const LinkAnalysis& ReadSession::getLinkAnalysis() const
    {
        return *one< LinkAnalysis >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const Identifiers& ReadSession::getIdentifiers() const
    {
        return *one< Identifiers >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const Layout& ReadSession::getLayout() const
    {
        return *one< Layout >( getObjects( IndexedObject::ANALYSIS_FILE ) );
    }
    const TranslationUnitAnalysis& ReadSession::getTranslationUnitAnalysis() const
    {
        return *one< TranslationUnitAnalysis >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const concrete::Action* ReadSession::getConcreteAction( TypeID id ) const
    {
        const IndexedObject::Array& tree = getObjects( IndexedObject::MASTER_FILE );
        if( id >=0 && id < static_cast< TypeID >( tree.size() ) )
        {
            return dynamic_cast< const concrete::Action* >( tree[ id ] );
        }
        return nullptr;
    }

    void ReadSession::getInvocations( std::size_t szTranslationUnitID, Invocations& invocations ) const
    {
        invocations = many_cst< InvocationSolution >( getObjects( szTranslationUnitID ) );
    }
}
