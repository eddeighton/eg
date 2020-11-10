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
    
    RuntimeTypeInterop::~RuntimeTypeInterop()
    {
        
    }
    
    RuntimeTypeInterop::Evaluation::~Evaluation()
    {
        
    }
    
    ComponentInterop::~ComponentInterop()
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
        EGRuntimeImpl( ComponentInterop& componentInterop, const char* pszDatabaseFilePath )
            :   CreatingSession( IndexedFile::FileIDtoPathMap{}, IndexedObject::NO_FILE ),
                m_componentInterop( componentInterop ),
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
            m_componentInterop.doRead( reference, dimensionType );
        }
        virtual void doWrite(   const reference& reference, TypeID dimensionType )
        {
            m_componentInterop.doWrite( reference, dimensionType );
        }
        virtual void doCall(   const reference& reference, TypeID dimensionType )
        {
            m_componentInterop.doCall( reference, dimensionType );
        }
        virtual void doStart(   const reference& reference, TypeID dimensionType )
        {
            m_componentInterop.doStart( reference, dimensionType );
        }
        virtual void doStop(    const reference& reference )
        {
            m_componentInterop.doStop( reference );
        }
        virtual void doPause(   const reference& reference )
        {
            m_componentInterop.doPause( reference );
        }
        virtual void doResume(  const reference& reference )
        {
            m_componentInterop.doResume( reference );
        }
        virtual void doDone(    const reference& reference )
        {
            m_componentInterop.doDone( reference );
        }
        virtual void doWaitAction(    const reference& reference )
        {
            m_componentInterop.doWaitAction( reference );
        }
        virtual void doWaitDimension(    const reference& reference, TypeID dimensionType )
        {
            m_componentInterop.doWaitDimension( reference, dimensionType );
        }
        virtual void doGetAction(    const reference& reference )
        {
            m_componentInterop.doGetAction( reference );
        }
        virtual void doGetDimension(    const reference& reference, TypeID dimensionType )
        {
            m_componentInterop.doGetDimension( reference, dimensionType );
        }
        virtual void doRange( const RangeDescription& range )
        {
            m_componentInterop.doRange( std::make_shared< EGRange >( range ) );
        }
		virtual void doLink( const reference& linkeeRef, TypeID linkeeDimension, const reference& linkValue )
		{
            m_componentInterop.doLink( linkeeRef, linkeeDimension, linkValue );
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
        
        virtual void printType( TypeID typeID, std::ostream& os )
        {
            const eg::IndexedObject::Array& objects =
                m_pDatabase->getObjects( IndexedObject::MASTER_FILE );
                
            if( ( typeID > 0U ) && ( typeID < objects.size() ) )
            {
                const eg::IndexedObject* pObject = objects[ typeID ];
                if( const eg::concrete::Action* pAction = dynamic_cast< const eg::concrete::Action* >( pObject ) )
                {
                    const std::vector< const eg::interface::Element* > path = 
                        eg::interface::getPath( pAction->getContext() );
                    bool bFirst = true;
                    for( const eg::interface::Element* pElement : path )
                    {
                        if( bFirst ) bFirst = false;
                        else os << '.';
                        os << pElement->getIdentifier();
                    }
                }
                else
                {
                    os << "invalid";
                }
            }
            else
            {
                os << "null";
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
        ComponentInterop& m_componentInterop;
        std::shared_ptr< eg::ReadSession > m_pDatabase;
        InvocationSolutionMap m_invocations;
    };
    
    
    EGRT_EXPORT EGRuntimePtr constructRuntime( ComponentInterop& componentInterop, const char* pszDatabaseFilePath )
    {
        static EGRuntimePtr pRuntime = std::make_shared< EGRuntimeImpl >( componentInterop, pszDatabaseFilePath );
        return pRuntime;
    }
    
    
}