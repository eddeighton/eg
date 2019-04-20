
#include "implementation_session.hpp"

namespace eg
{
    ImplementationSession::ImplementationSession( const IndexedFile::FileIDtoPathMap& files )
        :   CreatingSession( files, IndexedObject::ANALYSIS_FILE )
    {
    }
    
    const abstract::Root* ImplementationSession::getTreeRoot() const
    {
        return root< abstract::Root >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const concrete::Action* ImplementationSession::getInstanceRoot() const
    {
        return root< concrete::Action >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const DerivationAnalysis& ImplementationSession::getDerivationAnalysis() const
    {
        return *one< DerivationAnalysis >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const Layout& ImplementationSession::getLayout() const
    {
        return *one< Layout >( getObjects( IndexedObject::ANALYSIS_FILE ) );
    }

    void ImplementationSession::getInvocations( std::size_t szTranslationUnitID, Invocations& invocations ) const
    {
        invocations = many_cst< InvocationSolution >( getObjects( szTranslationUnitID ) );
    }
        
    std::string generateName( char prefix, const std::vector< const abstract::Element* >& path )
    {
        std::ostringstream os;
        os << prefix;
        for( const abstract::Element* pIter : path )
        {
            if( pIter->getParent() )
                os << "_" << pIter->getIdentifier();
        }
        return os.str();
    }

    std::string generateName( char prefix, const abstract::Element* pElement, const abstract::Element* pFrom )
    {
        std::ostringstream os;
        os << prefix;
        std::vector< const abstract::Element* > path = getPath( pElement, pFrom );
        for( const abstract::Element* pIter : path )
        {
            os << "_" << pIter->getIdentifier();
        }
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
            pBuffer->m_pAction = pAction;
            
            std::vector< const abstract::Element* > path = 
                getPath( pAction->getAction() );
            
            pBuffer->name       = generateName( 'b', path );
            pBuffer->variable   = generateName( 'g', path );
            
            std::size_t szOffset = 0U;
            for( const concrete::Dimension* pDimension : dimensions )
            {
                DataMember* pDimensionInstance = nullptr; 
                const std::size_t szDimensionOffset = szOffset;
                szOffset += pDimension->getDataSize();
                
                {
                    if( const concrete::Dimension_User* pUserDim = 
                        dynamic_cast< const concrete::Dimension_User* >( pDimension ) )
                    {
                        pDimensionInstance = construct< DataMember >();
                        pDimensionInstance->name = generateName( 'm', pUserDim->getDimension(), pAction->getAction() );
                    }
                    else if( const concrete::Dimension_Generated* pGenDim =
                        dynamic_cast< const concrete::Dimension_Generated* >( pDimension ) )
                    {
                        switch( pGenDim->getDimensionType() )
                        {
                            case concrete::Dimension_Generated::eDimensionTimestamp:
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    
                                    const concrete::Dimension_User* pUserDim = pGenDim->getUserDimension();
                                    VERIFY_RTE( pUserDim );
                                    
                                    pDimensionInstance->name = 
                                        generateName( 'g', pUserDim->getDimension(), pAction->getAction() ) + "_timestamp";
                                    
                                }
                                break;
                                
                            case concrete::Dimension_Generated::eActionRunning   :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_timestamp_runnning";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionPaused    :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_timestamp_paused";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionCoroutine :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_coroutine";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionEventIter :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_event_iter";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionObject    :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_object";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionReference    :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_reference";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionAllocatorData    :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_alloc_data";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionAllocatorHead    :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    
                                    const concrete::Dimension_Generated* pDimGen = 
                                        dynamic_cast< const concrete::Dimension_Generated* >( pDimension );
                                        
                                    const concrete::Action* pAllocatedAction = pDimGen->getAction();
                                    
                                    pDimensionInstance->name = pBuffer->variable + 
                                        pAllocatedAction->getAbstractElement()->getIdentifier() + "_alloc_iter";
                                }
                                break;
                            case concrete::Dimension_Generated::eActionStopTimestamp    :
                                {
                                    pDimensionInstance = construct< DataMember >();
                                    pDimensionInstance->name = pBuffer->variable + "_timestamp_stopped";
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
                if( pDimensionInstance )
                {
                    pDimensionInstance->m_pBuffer       = pBuffer;
                    pDimensionInstance->m_pDimension    = pDimension;
                    pDimensionInstance->offset          = szDimensionOffset;
                    pBuffer->m_dimensions.push_back( pDimensionInstance );
                    dimensionMap.insert( std::make_pair( pDimension, pDimensionInstance ) );
                }
            }
            pBuffer->stride = szOffset;
            pBuffer->size   = szSize * pAction->getAction()->getSize();
            
        }
        
        for( const concrete::Element* pChild : pAction->getChildren() )
        {
            if( const concrete::Action* pChildAction = 
                dynamic_cast< const concrete::Action* >( pChild ) )
            {
                recurseInstances( buffers, dimensionMap, szSize * pAction->getAction()->getSize(), pChildAction );
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
            
            //establish dimension depedencies
            for( Layout::DimensionMap::iterator 
                i = dimensionMap.begin(),
                iEnd = dimensionMap.end(); i!=iEnd; ++i )
            {
                const concrete::Dimension* pInstance = i->first;
                if( const concrete::Dimension_Generated* pGenDim = 
                    dynamic_cast< const concrete::Dimension_Generated* >( pInstance ) )
                {
                    if( pGenDim->getDependency() )
                    {
                        DataMember* pDimension = i->second;
                        Layout::DimensionMap::const_iterator iFind = 
                            dimensionMap.find( pGenDim->getDependency() );
                        VERIFY_RTE( iFind != dimensionMap.end() );
                        pDimension->m_pDependency = iFind->second;
                    }
                }
            }
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
    
    const abstract::Root* ReadSession::getTreeRoot() const
    {
        return root< abstract::Root >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const concrete::Action* ReadSession::getInstanceRoot() const
    {
        return root< concrete::Action >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const DerivationAnalysis& ReadSession::getDerivationAnalysis() const
    {
        return *one< DerivationAnalysis >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    const Layout& ReadSession::getLayout() const
    {
        return *one< Layout >( getObjects( IndexedObject::ANALYSIS_FILE ) );
    }
    const abstract::Action* ReadSession::getAbstractAction( EGTypeID id ) const
    {
        const IndexedObject::Array& tree = getObjects( IndexedObject::MASTER_FILE );
        if( id >=0 && id < static_cast< EGTypeID >( tree.size() ) )
        {
            return dynamic_cast< const abstract::Action* >( tree[ id ] );
        }
        return nullptr;
    }

    void ReadSession::getInvocations( std::size_t szTranslationUnitID, Invocations& invocations ) const
    {
        invocations = many_cst< InvocationSolution >( getObjects( szTranslationUnitID ) );
    }
}
