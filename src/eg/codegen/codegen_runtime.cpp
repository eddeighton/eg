
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
#include "eg_compiler/codegen/codegen.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/allocator.hpp"

#include <ostream>

namespace eg
{
    void generate_dynamic_interface( std::ostream& os, const ReadSession& session )
    {
        const interface::Root* pRoot = session.getTreeRoot();
        const concrete::Action* pInstanceRoot = nullptr;
        {
            std::vector< const concrete::Action* > roots;
            for( const concrete::Element* pChild : session.getInstanceRoot()->getChildren() )
            {
                if( const concrete::Action* pAction = 
                    dynamic_cast< const concrete::Action* >( pChild ) )
                {
                    roots.push_back( pAction );
                }
            }
            ASSERT( !roots.empty() );
            ASSERT( roots.size() == 1U );
            pInstanceRoot = roots.front();
        }
        
        const DerivationAnalysis& derivationAnalysis = session.getDerivationAnalysis();
        const Layout& layout = session.getLayout();
        const IndexedObject::Array& objects = session.getObjects( IndexedObject::MASTER_FILE );
        std::vector< const concrete::Action* > actions = 
            many_cst< concrete::Action >( objects );
        
        if( !actions.empty() )
        {
        os << "eg::TimeStamp getTimestamp( " << EG_TYPE_ID << " typeID, " << EG_INSTANCE << " instance )\n";
        os << "{\n";
        os << "    switch( typeID )\n";
        os << "    {\n";
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() && pAction->getReference() )
            {
        os << "        case " << pAction->getIndex() << ": return " << 
                Printer( layout.getDataMember( pAction->getReference() ), "instance" ) << ".data.timestamp;\n";
            }
        }
        os << "        default: throw std::runtime_error( \"Invalid action instance\" );\n";
        os << "    }\n";
        os << "}\n";
        
        os << EG_ACTION_STATE << " getState( " << EG_TYPE_ID << " typeID, " << EG_INSTANCE << " instance )\n";
        os << "{\n";
        os << "    switch( typeID )\n";
        os << "    {\n";
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() && pAction->getState() )
            {
        os << "        case " << pAction->getIndex() << ": return " << 
            Printer( layout.getDataMember( pAction->getState() ), "instance" ) << ";\n";
            }
        }
        os << "        default: throw std::runtime_error( \"Invalid action instance\" );\n";
        os << "    }\n";
        os << "}\n";
        
        os << EG_TIME_STAMP << " getStopCycle( " << EG_TYPE_ID << " typeID, " << EG_INSTANCE << " instance )\n";
        os << "{\n";
        os << "    switch( typeID )\n";
        os << "    {\n";
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() && pAction->getStopCycle() )
            {
        os << "        case " << pAction->getIndex() << ": return " << 
            Printer( layout.getDataMember( pAction->getStopCycle() ), "instance" ) << ";\n";
            }
        }
        os << "        default: throw std::runtime_error( \"Invalid action instance\" );\n";
        os << "    }\n";
        os << "}\n";
        }
        
        os << getInterfaceType( input::Root::RootTypeName ) << "< void > get_root()\n";
        os << "{\n";
        os << "    return  " << getInterfaceType( input::Root::RootTypeName ) << "< void >( " << 
            EG_REFERENCE_TYPE << "{ 0, " << pInstanceRoot->getIndex() << ", getTimestamp( " << pInstanceRoot->getIndex() << ", 0 ) } );\n";
        os << "}\n";
        os << "\n";
        
    }
	
    void generateActionInit( std::ostream& os, const Layout& layout, const concrete::Action* pAction, const char* pszInstance )
    {
        if( const interface::Link* pLink = dynamic_cast< const interface::Link* >( pAction->getContext() ) )
        {
            const concrete::Dimension_User* pLinkBaseDimension = pAction->getLinkBaseDimension();
            VERIFY_RTE( pLinkBaseDimension );
            const DataMember* pLinkBaseData = layout.getDataMember( pLinkBaseDimension );
            VERIFY_RTE( pLinkBaseData );
            
            os << "            " << Printer( pLinkBaseData, pszInstance ) << ".data = { 0, 0, 0 };\n";
        }
    }
        
	
	void generateExecutableActionStarter( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
	{
		VERIFY_RTE( pAction->getParent() && pAction->getParent()->getParent() );
		const concrete::Action* pParentAction = dynamic_cast< const concrete::Action* >( pAction->getParent() );
		VERIFY_RTE( pParentAction );
        const concrete::Allocator* pAllocator = pParentAction->getAllocator( pAction );
        VERIFY_RTE( pAllocator );
		
        if( const concrete::NothingAllocator* pNothingAllocator =
                dynamic_cast< const concrete::NothingAllocator* >( pAllocator ) )
        {
            //do nothing
        }
        else if( const concrete::SingletonAllocator* pSingletonAllocator =
                dynamic_cast< const concrete::SingletonAllocator* >( pAllocator ) )
        {
            const DataMember* pStateData = layout.getDataMember( pAction->getState() );
            VERIFY_RTE( pStateData );
            const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
            VERIFY_RTE( pReferenceData );
		    const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
            VERIFY_RTE( pCycleData );
            
            os << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _parent_id )\n";
            os << "{\n";
            os << "    if( ( " << Printer( pStateData, "_parent_id" ) << " == " << getActionState( action_stopped ) << 
                " ) && ( " << Printer( pCycleData, "_parent_id" ) << " < clock::cycle() ) )\n";
            os << "    {\n";
            os << "        const " << EG_INSTANCE << " startCycle = clock::cycle();\n";
            os << "        " << getStaticType( pAction->getContext() ) << "& reference = " << Printer( pReferenceData, "_parent_id" ) << ";\n";
            os << "        reference.data.timestamp = startCycle;\n";
            os << "        reference.data.type = " << pAction->getIndex() << ";\n";
            os << "        " << Printer( pStateData, "_parent_id" ) << " = " << getActionState( action_running ) << ";\n";
            os << "        events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            generateActionInit( os, layout, pAction, "_parent_id" );
            
            os << "        return reference;\n";
            os << "    }\n";
            
            std::ostringstream osError;
            osError << "Error attempting to start type: " << pAction->getName();
            os << "    events::put( \"error\", clock::cycle(), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
            os << "    " << getStaticType( pAction->getContext() ) << " nullInstance;\n";
            os << "    return nullInstance;\n";
            os << "}\n";
            os << "\n";
        }
        else if( const concrete::RangeAllocator* pRangeAllocator =
                dynamic_cast< const concrete::RangeAllocator* >( pAllocator ) )
        {
            const DataMember* pStateData = layout.getDataMember( pAction->getState() );
            VERIFY_RTE( pStateData );
            const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
            VERIFY_RTE( pReferenceData );
            const DataMember* pAllocatorData = layout.getDataMember( pRangeAllocator->getAllocatorData() );
            VERIFY_RTE( pAllocatorData );
		    const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
            VERIFY_RTE( pCycleData );
            
            os << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _parent_id )\n";
            os << "{\n";
            
            os << "    if( !" << Printer( pAllocatorData, "_parent_id" ) << ".full() )\n";
            os << "    {\n";
            os << "        const " << EG_INSTANCE << " freeIndex = " << Printer( pAllocatorData, "_parent_id" ) << ".nextFree();\n";
            os << "        const " << EG_INSTANCE << " newInstance = " << "_parent_id * " << pAction->getLocalDomainSize() << " + freeIndex;\n";
            os << "        if( " << Printer( pCycleData, "newInstance" ) << " < clock::cycle() )\n";
            os << "        {\n";
            os << "            " << Printer( pAllocatorData, "_parent_id" ) << ".allocate( freeIndex );\n";
            os << "            const " << EG_INSTANCE << " startCycle = clock::cycle();\n";
            os << "            " << getStaticType( pAction->getContext() ) << "& reference = " << Printer( pReferenceData, "newInstance" ) << ";\n";
            os << "            reference.data.timestamp = startCycle;\n";
            os << "            reference.data.type = " << pAction->getIndex() << ";\n";
            os << "            " << Printer( pStateData, "newInstance" ) << " = " << getActionState( action_running ) << ";\n";
            os << "            events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            generateActionInit( os, layout, pAction, "newInstance" );
            
            os << "            return reference;\n";
            os << "        }\n";
            os << "    }\n";
            
            std::ostringstream osError;
            osError << "Error attempting to start type: " << pAction->getName();
            os << "    events::put( \"error\", clock::cycle(), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
            os << "    " << getStaticType( pAction->getContext() ) << " nullInstance;\n";
            os << "    return nullInstance;\n";
            os << "}\n";
            os << "\n";
        }
        else
        {
            THROW_RTE( "Unknown allocator type" );
        }
	}
	
	void generateMainActionStarter( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
	{
		//simple starter for root
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
				
        os << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter()\n";
        os << "{\n";
        os << "    const " << EG_INSTANCE << " startCycle = clock::cycle();\n";
        os << "    " << getStaticType( pAction->getContext() ) << "& reference = " << Printer( pReferenceData, "0" ) << ";\n";
        os << "    reference.data.timestamp = startCycle;\n";
        os << "    " << Printer( pStateData, "0" ) << " = " << getActionState( action_running ) << ";\n";
        os << "    events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        os << "    return reference;\n";
        os << "}\n";
        os << "\n";
	}
	
    void generateSubTreeStop( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
    {
        //stop the subtree
        for( const concrete::Element* pChild : pAction->getChildren() )
        {
            if( const concrete::Action* pChildAction = dynamic_cast< const concrete::Action* >( pChild ) )
            {
                if( const concrete::Allocator* pChildAllocator = pChildAction->getAllocator() )
                {
                    if( const concrete::NothingAllocator* pChildNothingAllocator =
                            dynamic_cast< const concrete::NothingAllocator* >( pChildAllocator ) )
                    {
                        //do nothing
                    }
                    else if( const concrete::SingletonAllocator* pChildSingletonAllocator =
                            dynamic_cast< const concrete::SingletonAllocator* >( pChildAllocator ) )
                    {
                        const DataMember* pStateData = layout.getDataMember( pChildAction->getState() );
            os << "        if( " << Printer( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
            os << "             " << pChildAction->getName() << "_stopper( _gid );\n";
                        
                    }
                    else if( const concrete::RangeAllocator* pChildRangeAllocator =
                            dynamic_cast< const concrete::RangeAllocator* >( pChildAllocator ) )
                    {
                        const DataMember* pChildAllocatorData = layout.getDataMember( pChildRangeAllocator->getAllocatorData() );
                        const DataMember* pStateData = layout.getDataMember( pChildAction->getState() );
                        
            os << "        if( !" << Printer( pChildAllocatorData, "_gid" ) << ".empty() )\n";
            os << "        {\n";
            os << "            for( " << EG_INSTANCE << " childIndex = _gid * " << pChildAction->getLocalDomainSize() << 
                                    "; childIndex != ( _gid + 1 ) * " << pChildAction->getLocalDomainSize() << "; ++childIndex )\n";
            os << "            {\n";
            os << "                if( " << Printer( pStateData, "childIndex" ) << " != " << getActionState( action_stopped ) << " )\n";
            os << "                    " << pChildAction->getName() << "_stopper( childIndex );\n";
            os << "            }\n";
            os << "        }\n";
                    }
                    else
                    {
                        THROW_RTE( "Unknown allocator type" );
                    }
                }
            }
        }
    }
    
    void generateStopperLinkBreaks( std::ostream& os, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
    {
        if( const interface::Link* pLink = dynamic_cast< const interface::Link* >( pAction->getContext() ) )
        {
            //call the breaker
            os << "        " << pAction->getName() << "_breaker( _gid );\n";
        }
            
        const concrete::Action* pObject = pAction->getObject();
        VERIFY_RTE( pObject );
        const concrete::Dimension_Generated* pLinkRefCount = pObject->getLinkRefCount();
        VERIFY_RTE( pLinkRefCount );
        const DataMember* pLinkRefCountDataMember = layout.getDataMember( pLinkRefCount );
        VERIFY_RTE( pLinkRefCountDataMember );
        
        std::ostringstream osObjectDomain;
        {
            const int iDomainFactor = pObject->getObjectDomainFactor();
            if( iDomainFactor == 1 )
                osObjectDomain << "_gid";
            else
                osObjectDomain << "_gid / " << iDomainFactor;
        }
        
        const concrete::Action::LinkMap& links = pAction->getLinks();
        for( concrete::Action::LinkMap::const_iterator 
                i = links.begin(), iEnd = links.end(); i!=iEnd; ++i )
        {
            const std::string& strLinkName = i->first;
            const concrete::Dimension_Generated* pLinkBackRef = i->second;
            VERIFY_RTE( pLinkBackRef );
            const DataMember* pBackRefData = layout.getDataMember( pLinkBackRef );
            const LinkGroup* pLinkGroup = pLinkBackRef->getLinkGroup();
            VERIFY_RTE( pLinkGroup );
            
            
            os << "        if( " << Printer( pBackRefData, "_gid" ) << ".timestamp != eg::INVALID_TIMESTAMP )\n";
            os << "        {\n";
            os << "            switch( " << Printer( pBackRefData, "_gid" ) << ".type )\n";
            os << "            {\n";
            for( const concrete::Action* pLink : pLinkGroup->getConcreteLinks() )
            {
            os << "                case " << pLink->getIndex() << ":\n";
            os << "                {\n";
            os << "                     " << Printer( pLinkRefCountDataMember, osObjectDomain.str().c_str() ) << 
                " = " << Printer( pLinkRefCountDataMember, osObjectDomain.str().c_str() ) << " - 1;\n";
                
            os << "                     const eg::reference backRef = " << Printer( pBackRefData, "_gid" ) << ";\n";
            os << "                     " << Printer( pBackRefData, "_gid" ) << " = { 0, 0, 0 };\n";
                const concrete::Dimension_User* pLinkBaseDimension = pLink->getLinkBaseDimension();
                VERIFY_RTE( pLinkBaseDimension );
                const DataMember* pLinkBaseData = layout.getDataMember( pLinkBaseDimension );
                VERIFY_RTE( pLinkBaseData );
            
            os << "                     " << Printer( pLinkBaseData, "backRef.instance" ) << ".data = { 0, 0, 0 };\n";
            os << "                     ::eg::Scheduler::stop_ref( backRef );\n";
                
       // TODO - should stopping a link target trigger referenced count based stop  
       // os << "           " << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " = " << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " - 1;\n";
       // os << "           if( " << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " == 0 )\n";
       // os << "           {\n";
       // os << "             ::eg::Scheduler::zeroRefCount( " << Printer( pReferenceData, osDomain.str().c_str() ) << ".data, " <<
       //                         "&" << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " );\n";
       // os << "           }\n";
            
            
            
            os << "                }\n";
            os << "                break;\n";
            
            }   
            os << "                default: ERR( \"Unknown link type\" ); break;\n";
            os << "            }\n";
            os << "        }\n";
        }
        
    }
    
	void generateMainActionStopper( std::ostream& os, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
	{
        os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        os << "     " << EG_INSTANCE << " _parent_id = _gid / " << pAction->getLocalDomainSize() << ";\n";
        
		const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
		
        os << "     if( " << Printer( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
        os << "     {\n";
        os << "         " << Printer( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
        os << "         " << Printer( pCycleData, "_gid" ) << " = clock::cycle();\n";
        os << "         events::put( \"stop\", clock::cycle(), &" << Printer( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        //stop the subtree
        generateStopperLinkBreaks( os, layout, linkAnalysis, pAction );
        generateSubTreeStop( os, layout, pAction );
        os << "     }\n";
		
        os << "}\n";
        os << "\n";
	}
	void generateExecutableActionStopper( std::ostream& os, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
	{
		VERIFY_RTE( pAction->getParent() && pAction->getParent()->getParent() );
		const concrete::Action* pParentAction = dynamic_cast< const concrete::Action* >( pAction->getParent() );
		VERIFY_RTE( pParentAction );
        const concrete::Allocator* pAllocator = pParentAction->getAllocator( pAction );
        VERIFY_RTE( pAllocator );
		
        if( const concrete::NothingAllocator* pNothingAllocator =
                dynamic_cast< const concrete::NothingAllocator* >( pAllocator ) )
        {
            //do nothing
        }
        else if( const concrete::SingletonAllocator* pSingletonAllocator =
                dynamic_cast< const concrete::SingletonAllocator* >( pAllocator ) )
        {
            const DataMember* pStateData = layout.getDataMember( pAction->getState() );
            const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
		    const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
            
            os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid )\n";
            os << "{\n";
            os << "    if( " << Printer( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
            os << "    {\n";
            os << "        ::eg::Scheduler::stopperStopped( " << Printer( pReferenceData, "_gid" ) << ".data );\n";
            os << "        " << Printer( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
            os << "        " << Printer( pCycleData, "_gid" ) << " = clock::cycle();\n";
            os << "        events::put( \"stop\", clock::cycle(), &" << Printer( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            //stop the subtree
            generateStopperLinkBreaks( os, layout, linkAnalysis, pAction );
            generateSubTreeStop( os, layout, pAction );
        
            os << "    }\n";
            os << "    else\n";
            os << "    {\n";
            std::ostringstream osError;
            osError << "Error attempting to stop type: " << pAction->getName();
            os << "        events::put( \"error\", clock::cycle(), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
            os << "    }\n";
            os << "}\n";
            os << "\n";
        }
        else if( const concrete::RangeAllocator* pRangeAllocator =
                dynamic_cast< const concrete::RangeAllocator* >( pAllocator ) )
        {
            const DataMember* pStateData = layout.getDataMember( pAction->getState() );
            const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
            const DataMember* pAllocatorData = layout.getDataMember( pRangeAllocator->getAllocatorData() );
		    const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
            
            os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid )\n";
            os << "{\n";
            os << "    if( " << Printer( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
            os << "    {\n";
            os << "        ::eg::Scheduler::stopperStopped( " << Printer( pReferenceData, "_gid" ) << ".data );\n";
            os << "        const " << EG_INSTANCE << " _parentIndex = " << "_gid / " << pAction->getLocalDomainSize() << ";\n";
            os << "        const " << EG_INSTANCE << " freeIndex = " << "_gid - _parentIndex * " << pAction->getLocalDomainSize() << ";\n";
            os << "        " << Printer( pAllocatorData, "_parentIndex" ) << ".free( freeIndex );\n";
            os << "        " << Printer( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
            os << "        " << Printer( pCycleData, "_gid" ) << " = clock::cycle();\n";
            os << "        events::put( \"stop\", clock::cycle(), &" << Printer( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            //stop the subtree
            generateStopperLinkBreaks( os, layout, linkAnalysis, pAction );
            generateSubTreeStop( os, layout, pAction );
        
            os << "    }\n";
            os << "    else\n";
            os << "    {\n";
            std::ostringstream osError;
            osError << "Error attempting to stop type: " << pAction->getName();
            os << "        events::put( \"error\", clock::cycle(), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
            os << "    }\n";
            os << "}\n";
            os << "\n";
        }
        else
        {
            THROW_RTE( "Unknown allocator type" );
        }
	}

	void generateBreaker( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
	{
        const interface::Link* pLink = dynamic_cast< const interface::Link* >( pAction->getContext() );
        VERIFY_RTE( pLink );
        
        const concrete::Dimension_User* pLinkBase = pAction->getLinkBaseDimension();
        VERIFY_RTE( pLinkBase );
        const DataMember* pLinkBaseData = layout.getDataMember( pLinkBase );
        VERIFY_RTE( pLinkBaseData );
        
        const LinkGroup* pLinkGroup = pLinkBase->getLinkGroup();
        VERIFY_RTE( pLinkGroup );
        
        os << "void " << pAction->getName() << "_breaker( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        
        os << "  const " << EG_REFERENCE_TYPE << " currentBase = " << Printer( pLinkBaseData, "_gid" ) << ".data;\n";
        
        os << "  if( currentBase.timestamp != eg::INVALID_TIMESTAMP )\n";
        os << "  {\n";
        
        os << "    switch( currentBase.type )\n";
        os << "    {\n";
        
            for( const concrete::Action* pTargetType : pLinkGroup->getTargets() )
            {
                if( const concrete::Action* pObject = pTargetType->getObject() )
                {
                    LinkGroup::LinkRefMap::const_iterator iFind =
                        pLinkGroup->getDimensionMap().find( pTargetType );
                    VERIFY_RTE( iFind != pLinkGroup->getDimensionMap().end() );
                    const concrete::Dimension_Generated* pLinkTargetRefDimension = iFind->second;
                    VERIFY_RTE( pLinkTargetRefDimension );
                    const DataMember* pLinkRef = layout.getDataMember( pLinkTargetRefDimension );
                    VERIFY_RTE( pLinkRef );
                    
                    VERIFY_RTE( pObject );
                    
                    if( pObject->getContext()->isMainExecutable() )
                    {
        os << "      case " << pTargetType->getIndex() << ": " << Printer( pLinkRef, "currentBase.instance" ) << " = { 0, 0, 0 }; break;\n";
                    }
                    else
                    {
                        const concrete::Dimension_Generated* pLinkRefCount = pObject->getLinkRefCount();
                        VERIFY_RTE( pLinkRefCount );
                        const DataMember* pLinkRefCountDataMember = layout.getDataMember( pLinkRefCount );
                        VERIFY_RTE( pLinkRefCountDataMember );
                        
                        const DataMember* pReferenceData = layout.getDataMember( pObject->getReference() );
                        
                        std::ostringstream osDomain;
                        {
                            const int iDomainFactor = pObject->getObjectDomainFactor();
                            if( iDomainFactor == 1 )
                                osDomain << "currentBase.instance";
                            else
                                osDomain << "currentBase.instance / " << iDomainFactor;
                        }
                    
        os << "      case " << pTargetType->getIndex() << ": " << Printer( pLinkRef, "currentBase.instance" ) << " = { 0, 0, 0 };\n";
        os << "           " << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " = " << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " - 1;\n";
        os << "           if( " << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " == 0 )\n";
        os << "           {\n";
        os << "             ::eg::Scheduler::zeroRefCount( " << Printer( pReferenceData, osDomain.str().c_str() ) << ".data, " <<
                                "&" << Printer( pLinkRefCountDataMember, osDomain.str().c_str() ) << " );\n";
        os << "           }\n";
        os << "           break;\n";
                    }
                }
            }
            
        os << "      default: ERR( \"Invalid link base in breaker\" ); break;\n";
        
        os << "    }\n";
        os << "  }\n";
        
        
        os << "}\n";
        os << "\n";
    }
    
    void generateActionInstanceFunctions( std::ostream& os, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
    {
        if( pAction->getStopCycle() && pAction->getState() )
        {
            if( pAction->getContext()->isMainExecutable() )
            {
				generateMainActionStarter( os, layout, pAction );
				generateMainActionStopper( os, layout, linkAnalysis, pAction );
            }
			else if( pAction->getContext()->isExecutable() )
            {
                generateExecutableActionStarter( os, layout, pAction );
				generateExecutableActionStopper( os, layout, linkAnalysis, pAction );
            }
		}
        if( dynamic_cast< const interface::Link* >( pAction->getContext() ) )
        {
            generateBreaker( os, layout, pAction );
        }
    }

    void generateActionInstanceFunctions( std::ostream& os, const ReadSession& program )
    {
        const interface::Root* pRoot = program.getTreeRoot();
        
        const DerivationAnalysis&   derivationAnalysis  = program.getDerivationAnalysis();
        const Layout&               layout              = program.getLayout();
        const LinkAnalysis&         linkAnalysis        = program.getLinkAnalysis();
        const IndexedObject::Array& objects             = program.getObjects( eg::IndexedObject::MASTER_FILE );
        std::vector< const concrete::Action* > actions = 
            many_cst< concrete::Action >( objects );
            
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() )
            {
                //os << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _gid );\n";
                os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid );\n";
            }
            if( dynamic_cast< const interface::Link* >( pAction->getContext() ) )
            {
                os << "void " << pAction->getName() << "_breaker( " << EG_INSTANCE << " _gid );\n";
            }
        }
        
        os << "//input::Context Function Implementations\n";
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() )
            {
                os << "\n";
                generateActionInstanceFunctions( os, layout, linkAnalysis, pAction );
            }
        }
        os << "\n";
    }
}