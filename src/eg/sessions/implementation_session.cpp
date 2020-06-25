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
        os << "_" << pIterLast->getIndex();
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
        os << "_" << pIterLast->getIndex();
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
        
        if( !dimensions.empty() )
        {
            //create buffer
            Buffer* pBuffer = construct< Buffer >();
            buffers.push_back( pBuffer );
            pBuffer->m_pContext = pAction;
            
            std::vector< const concrete::Element* > path = concrete::getPath( pAction );
            
            pBuffer->name       = generateName( 'b', path );
            pBuffer->variable   = generateName( 'g', path );
            
            for( const concrete::Dimension* pDimension : dimensions )
            {
                DataMember* pDataMember = nullptr; 
                
                {
                    if( const concrete::Dimension_User* pUserDim = 
                        dynamic_cast< const concrete::Dimension_User* >( pDimension ) )
                    {
                        pDataMember = construct< DataMember >();
                        pDataMember->name = generateName( 'm', pUserDim, pAction );
                    }
                    else if( const concrete::Dimension_Generated* pGenDim =
                        dynamic_cast< const concrete::Dimension_Generated* >( pDimension ) )
                    {
                        switch( pGenDim->getDimensionType() )
                        {
                            case concrete::Dimension_Generated::eActionStopCycle :
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->name = pBuffer->variable + "_cycle";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionState    :
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->name = pBuffer->variable + "_state";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionReference    :
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->name = pBuffer->variable + "_reference";
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
                                    osVarName << pBuffer->variable << "_" << pDimensionAction->getContext()->getIdentifier() << "_allocator";
                                    
                                    pDataMember = construct< DataMember >();
                                    pDataMember->name = osVarName.str();
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
									osVarName << pBuffer->variable << "_link_" << pLinkGroup->getLinkName();
                                    pDataMember->name = osVarName.str();
								}
                                break;
							case concrete::Dimension_Generated::eLinkReferenceCount:
                                {
                                    pDataMember = construct< DataMember >();
                                    pDataMember->name = pBuffer->variable + "_link_ref_count";
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
                    pDataMember->m_pBuffer       = pBuffer;
                    pDataMember->m_pDimension    = pDimension;
                    pBuffer->m_dimensions.push_back( pDataMember );
                    dimensionMap.insert( std::make_pair( pDimension, pDataMember ) );
                }
            }
            pBuffer->size   = szSize * pAction->getContext()->getSize();
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
