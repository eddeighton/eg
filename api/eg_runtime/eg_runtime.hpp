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


#ifndef RUNTIME_API_26_04_2019
#define RUNTIME_API_26_04_2019

#ifdef EG_RUNTIME_SHARED_MODULE
    #define EGRT_EXPORT __declspec( dllexport )
#else
    #define EGRT_EXPORT __declspec( dllimport )
#endif

#include "eg/common.hpp"

#include <vector>
#include <memory>

//EG Runtime Interface
namespace eg
{
    struct EGRT_EXPORT RuntimeTypeInterop
    {
        virtual ~RuntimeTypeInterop();
        
        struct EGRT_EXPORT Evaluation
        {
            using Ptr = std::shared_ptr< Evaluation >;
            virtual ~Evaluation();
        
            virtual bool isResult() const = 0;
            virtual void* getResult() const = 0;
        };
        
        virtual Evaluation::Ptr begin( void* pArgs, void* pKWArgs ) = 0;
        
        virtual eg::TimeStamp   getTimestamp( eg::TypeID type, eg::Instance instance ) = 0;
        virtual eg::ActionState getState( eg::TypeID type, eg::Instance instance ) = 0;
        virtual eg::TimeStamp   getStopCycle( eg::TypeID type, eg::Instance instance ) = 0;
        virtual eg::TimeStamp   getClockCycle() = 0;
    };
    
    struct EGRT_EXPORT EGRangeDescription
    {
        virtual ~EGRangeDescription();
        
        virtual std::size_t getSize() = 0;
        virtual TypeID getType( std::size_t szIndex ) = 0;
        virtual Instance getBegin( std::size_t szIndex ) = 0;
        virtual Instance getEnd( std::size_t szIndex ) = 0;
        virtual bool raw() = 0;
    };
    
    using EGRangeDescriptionPtr = std::shared_ptr< EGRangeDescription >;
    
    struct EGRT_EXPORT ComponentInterop
    {
        virtual ~ComponentInterop();
        
        virtual reference dereferenceDimension( const reference& action, const TypeID& dimensionType ) = 0;
        virtual void doRead(    const reference& reference, TypeID dimensionType ) = 0;
        virtual void doWrite(   const reference& reference, TypeID dimensionType ) = 0;
        virtual void doCall(   const reference& reference, TypeID dimensionType ) = 0;
        virtual void doStart(   const reference& reference, TypeID dimensionType ) = 0;
        virtual void doStop(    const reference& reference ) = 0;
        virtual void doPause(   const reference& reference ) = 0;
        virtual void doResume(  const reference& reference ) = 0;
        virtual void doDone(    const reference& reference ) = 0;
        virtual void doWaitAction(    const reference& reference ) = 0;
        virtual void doWaitDimension(    const reference& reference, TypeID dimensionType ) = 0;
        virtual void doGetAction(    const reference& reference ) = 0;
        virtual void doGetDimension(    const reference& reference, TypeID dimensionType ) = 0;
        virtual void doRange( EGRangeDescriptionPtr pRange ) = 0;
        virtual void doLink( const reference& linkeeRef, TypeID linkeeDimension, const reference& linkValue ) = 0;
    };

    class EGRT_EXPORT EGRuntime
    {
    public:
        virtual ~EGRuntime();
        virtual void getIdentities( std::vector< const char* >& identities ) = 0;
        virtual TypeID getTypeID( const char* pszIdentity ) = 0;
        virtual void invoke( const reference& context, const std::vector< TypeID >& typePath, bool bHasParameters ) = 0;
    };

    using EGRuntimePtr = std::shared_ptr< EGRuntime >;
    EGRT_EXPORT EGRuntimePtr constructRuntime( ComponentInterop& componentInterop, const char* pszDatabaseFilePath );

}


#endif //RUNTIME_API_26_04_2019