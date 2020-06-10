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


#include "eg_runtime/eg_runtime.hpp"

#include "eg_compiler/sessions/implementation_session.hpp"
#include "eg_compiler/derivation.hpp"

#include "common/file.hpp"

#include <tuple>
#include <map>
#include <memory>

namespace eg
{
    
    HostFunctionAccessor::~HostFunctionAccessor()
    {
        
    }
    
    EGRangeDescription::~EGRangeDescription()
    {
        
    }
    
    EGRuntime::~EGRuntime()
    {
        
    }
    
    std::shared_ptr< eg::ReadSession > loadDatabase( const char* pszDatabaseFilePath )
    {
        const boost::filesystem::path databaseFilePath = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    std::string( pszDatabaseFilePath ) ) );
        if( !boost::filesystem::exists( databaseFilePath ) )
        {
            THROW_RTE( "Cannot locate file: " << databaseFilePath.string() );
        } 
        return std::make_shared< eg::ReadSession >( databaseFilePath );
    }
    
    class EGRange : public EGRangeDescription
    {
        const RangeDescription range;
    public:
        EGRange( const RangeDescription& range )
            :    range( range )
        {
        }
        
        virtual std::size_t getSize()
        {
            return range.ranges.size();
        }
        virtual TypeID getType( std::size_t szIndex )
        {
            return range.ranges[ szIndex ].runtimeType;
        }
        virtual Instance getBegin( std::size_t szIndex )
        {
            return range.ranges[ szIndex ].begin;
        }
        virtual Instance getEnd( std::size_t szIndex )
        {
            return range.ranges[ szIndex ].end;
        }
        virtual bool raw()
        {
            return range.raw;
        }
    };
    
    class EGRuntimeImpl : public EGRuntime, public RuntimeEvaluator, public CreatingSession
    {
    public:
        EGRuntimeImpl( HostFunctionAccessor& hostAccessor, const char* pszDatabaseFilePath )
            :   CreatingSession( IndexedFile::FileIDtoPathMap{}, IndexedObject::NO_FILE ),
                m_hostAccessor( hostAccessor ),
                m_pDatabase( loadDatabase( pszDatabaseFilePath ) ),
                m_invocations( *this, m_pDatabase->getDerivationAnalysis() )
        {
            
        }
        ~EGRuntimeImpl()
        {
        }
        //RuntimeEvaluator
        virtual reference dereferenceDimension( const reference& action, const TypeID& dimensionType )
        {
            return action;
        }
        virtual void doRead(    const reference& reference, TypeID dimensionType )
        {
            m_hostAccessor.doRead( reference, dimensionType );
        }
        virtual void doWrite(   const reference& reference, TypeID dimensionType )
        {
            m_hostAccessor.doWrite( reference, dimensionType );
        }
        virtual void doCall(   const reference& reference, TypeID dimensionType )
        {
            m_hostAccessor.doCall( reference, dimensionType );
        }
        virtual void doStart(   const reference& reference, TypeID dimensionType )
        {
            m_hostAccessor.doStart( reference, dimensionType );
        }
        virtual void doStop(    const reference& reference )
        {
            m_hostAccessor.doStop( reference );
        }
        virtual void doPause(   const reference& reference )
        {
            m_hostAccessor.doPause( reference );
        }
        virtual void doResume(  const reference& reference )
        {
            m_hostAccessor.doResume( reference );
        }
        virtual void doDone(    const reference& reference )
        {
            m_hostAccessor.doDone( reference );
        }
        virtual void doWaitAction(    const reference& reference )
        {
            m_hostAccessor.doWaitAction( reference );
        }
        virtual void doWaitDimension(    const reference& reference, TypeID dimensionType )
        {
            m_hostAccessor.doWaitDimension( reference, dimensionType );
        }
        virtual void doGetAction(    const reference& reference )
        {
            m_hostAccessor.doGetAction( reference );
        }
        virtual void doGetDimension(    const reference& reference, TypeID dimensionType )
        {
            m_hostAccessor.doGetDimension( reference, dimensionType );
        }
        virtual void doRange( const RangeDescription& range )
        {
            m_hostAccessor.doRange( std::make_shared< EGRange >( range ) );
        }
		virtual void doLink( const reference& linkeeRef, TypeID linkeeDimension, const reference& linkValue )
		{
            m_hostAccessor.doLink( linkeeRef, linkeeDimension, linkValue );
		}
            
        //EGRuntime
        virtual void getIdentities( std::vector< const char* >& identities )
        {
            const Identifiers::IdentifierMap& identifiers = 
                m_pDatabase->getIdentifiers().getIdentifiersMap();
            for( Identifiers::IdentifierMap::const_iterator 
                i = identifiers.begin(),
                iEnd = identifiers.end(); i!=iEnd; ++i )
            {
                identities.push_back( i->first.c_str() );
            }
            
            const OperationIDStringArray& operations = getOperationStrings();
            for( const std::string& strOp : operations )
            {
                identities.push_back( strOp.c_str() );
            }
        }
        
        virtual TypeID getTypeID( const char* pszIdentity )
        {
            const Identifiers& identifiers = m_pDatabase->getIdentifiers();
            
            const std::string strIdentity( pszIdentity );
            
            TypeID typeID = getOperationName( strIdentity );
            if( HIGHEST_OPERATION_TYPE == typeID )
            {
                if( const interface::Element* pElement = identifiers.isElement( strIdentity )  )
                {
                    typeID = - static_cast< TypeID >( pElement->getIndex() );
                }
                else
                {
                    typeID = 0;
                }
            }
            
            return typeID;
        }
        
        virtual void invoke( const reference& ref, const std::vector< TypeID >& implicitTypePath, bool bHasParameters )
        {
            const InvocationSolution::InvocationID invocationID = 
                InvocationSolution::invocationIDFromTypeIDs( m_pDatabase->getObjects( IndexedObject::MASTER_FILE ), 
                    m_pDatabase->getIdentifiers(), ref.type, implicitTypePath, bHasParameters );
                    
            const InvocationSolution* pInvocation = m_invocations.getInvocation( invocationID, implicitTypePath );
            ASSERT( pInvocation );
            
            pInvocation->evaluate( *this, ref );
        }
        
    private:
        HostFunctionAccessor& m_hostAccessor;
        std::shared_ptr< eg::ReadSession > m_pDatabase;
        InvocationSolutionMap m_invocations;
    };
    
    
    EGRT_EXPORT EGRuntimePtr constructRuntime( HostFunctionAccessor& hostAccessor, const char* pszDatabaseFilePath )
    {
        static EGRuntimePtr pRuntime = std::make_shared< EGRuntimeImpl >( hostAccessor, pszDatabaseFilePath );
        return pRuntime;
    }
    
    
}