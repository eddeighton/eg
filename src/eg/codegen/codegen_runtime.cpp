
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
    
    void generateAccessorFunctionImpls( std::ostream& os, PrinterFactory& printerFactory, const ReadSession& program )
    {
        const interface::Root*      pRoot               = program.getTreeRoot();
        const Layout&               layout              = program.getLayout();
        const LinkAnalysis&         linkAnalysis        = program.getLinkAnalysis();
        const IndexedObject::Array& objects             = program.getObjects( eg::IndexedObject::MASTER_FILE );
        
        std::vector< const concrete::Dimension* > dimensions = 
            many_cst< concrete::Dimension >( objects );
        for( const concrete::Dimension* pDimension : dimensions )
        {
            if( const concrete::Dimension_Generated* pGeneratedDimension = 
                dynamic_cast< const concrete::Dimension_Generated* >( pDimension ) )
            {
                switch( pGeneratedDimension->getDimensionType() )
                {
                    case concrete::Dimension_Generated::eActionStopCycle    : 
                    case concrete::Dimension_Generated::eActionState        : 
                    case concrete::Dimension_Generated::eLinkReferenceCount : 
                        {
                            generateDimensionType( os, pGeneratedDimension );
                            os << " " << getDimensionAccessorFunctionName( pDimension ) << "( " << EG_INSTANCE << " gid )\n";
                            os << "{\n";
                            os << "  return " << *printerFactory.read( layout.getDataMember( pDimension ), "gid" ) << ";\n";
                            os << "}\n";
                        }
                        break;
                    case concrete::Dimension_Generated::eActionReference    : 
                    case concrete::Dimension_Generated::eLinkReference      : 
                        {
                            os << "const ";
                            generateDimensionType( os, pGeneratedDimension );
                            os << "& " << getDimensionAccessorFunctionName( pDimension ) << "( " << EG_INSTANCE << " gid )\n";
                            os << "{\n";
                            os << "  return " << *printerFactory.read( layout.getDataMember( pDimension ), "gid" ) << ";\n";
                            os << "}\n";
                        }
                        break;
                    case concrete::Dimension_Generated::eActionAllocator    : break;
                    default:
                        THROW_RTE( "Unknown generated dimension type" );
                }
            
            }
            else if( const concrete::Dimension_User* pUserDimension = 
                dynamic_cast< const concrete::Dimension_User* >( pDimension ) )
            {
                if( pUserDimension->getLinkGroup() )
                {
                    os << "const ";
                    generateDimensionType( os, pUserDimension );
                    os << "& " << getDimensionAccessorFunctionName( pDimension ) << "( " << EG_INSTANCE << " gid )\n";
                    os << "{\n";
                    os << "  return " << *printerFactory.read( layout.getDataMember( pDimension ), "gid" ) << ";\n";
                    os << "}\n";
                }
            }
            else
            {
                THROW_RTE( "Unknown dimension type" );
            }   
        }  
    }
    
    void generateActionInit( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const concrete::Action* pAction, const char* pszInstance )
    {
        if( const interface::Link* pLink = dynamic_cast< const interface::Link* >( pAction->getContext() ) )
        {
            const concrete::Dimension_User* pLinkBaseDimension = pAction->getLinkBaseDimension();
            VERIFY_RTE( pLinkBaseDimension );
            const DataMember* pLinkBaseData = layout.getDataMember( pLinkBaseDimension );
            VERIFY_RTE( pLinkBaseData );
            
            os << "            " << *printerFactory.write( pLinkBaseData, pszInstance ) << ".data = { 0, 0, 0 };\n";
        }
    }
        
	
	void generateExecutableActionStarter( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const concrete::Action* pAction )
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
            os << "    if( ( " << *printerFactory.read( pStateData, "_parent_id" ) << " == " << getActionState( action_stopped ) << 
                " ) && ( " << *printerFactory.read( pCycleData, "_parent_id" ) << " < clock::cycle( " << pAction->getIndex() << " ) ) )\n";
            os << "    {\n";
            os << "        const " << EG_INSTANCE << " startCycle = clock::cycle( " << pAction->getIndex() << " );\n";
            os << "        " << getStaticType( pAction->getContext() ) << "& reference = " << *printerFactory.write( pReferenceData, "_parent_id" ) << ";\n";
            os << "        reference.data.timestamp = startCycle;\n";
            os << "        reference.data.type = " << pAction->getIndex() << ";\n";
            os << "        " << *printerFactory.write( pStateData, "_parent_id" ) << " = " << getActionState( action_running ) << ";\n";
            os << "        events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            generateActionInit( os, printerFactory, layout, pAction, "_parent_id" );
            
            os << "        return reference;\n";
            os << "    }\n";
            
            std::ostringstream osError;
            osError << "Error attempting to start type: " << pAction->getName();
            os << "    events::put( \"error\", clock::cycle( 0 ), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
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
            
            os << "    if( !" << *printerFactory.read( pAllocatorData, "_parent_id" ) << ".full() )\n";
            os << "    {\n";
            os << "        const " << EG_INSTANCE << " freeIndex = " << *printerFactory.read( pAllocatorData, "_parent_id" ) << ".nextFree();\n";
            os << "        const " << EG_INSTANCE << " newInstance = " << "_parent_id * " << pAction->getLocalDomainSize() << " + freeIndex;\n";
            os << "        if( " << *printerFactory.read( pCycleData, "newInstance" ) << " < clock::cycle( " << pAction->getIndex() << " ) )\n";
            os << "        {\n";
            os << "            " << *printerFactory.write( pAllocatorData, "_parent_id" ) << ".allocate( freeIndex );\n";
            os << "            const " << EG_INSTANCE << " startCycle = clock::cycle( " << pAction->getIndex() << " );\n";
            os << "            " << getStaticType( pAction->getContext() ) << "& reference = " << *printerFactory.write( pReferenceData, "newInstance" ) << ";\n";
            os << "            reference.data.timestamp = startCycle;\n";
            os << "            reference.data.type = " << pAction->getIndex() << ";\n";
            os << "            " << *printerFactory.write( pStateData, "newInstance" ) << " = " << getActionState( action_running ) << ";\n";
            os << "            events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            generateActionInit( os, printerFactory, layout, pAction, "newInstance" );
            
            os << "            return reference;\n";
            os << "        }\n";
            os << "    }\n";
            
            std::ostringstream osError;
            osError << "Error attempting to start type: " << pAction->getName();
            os << "    events::put( \"error\", clock::cycle( 0 ), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
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
	
	void generateMainActionStarter( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const concrete::Action* pAction )
	{
		//simple starter for root
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
				
        os << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter()\n";
        os << "{\n";
        os << "    const " << EG_INSTANCE << " startCycle = clock::cycle( " << pAction->getIndex() << " );\n";
        os << "    " << getStaticType( pAction->getContext() ) << "& reference = " << *printerFactory.write( pReferenceData, "0" ) << ";\n";
        os << "    reference.data.timestamp = startCycle;\n";
        os << "    " << *printerFactory.write( pStateData, "0" ) << " = " << getActionState( action_running ) << ";\n";
        os << "    events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        os << "    return reference;\n";
        os << "}\n";
        os << "\n";
	}
	
    void generateSubTreeStop( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const concrete::Action* pAction )
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
            os << "        if( " << *printerFactory.read( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
            os << "             " << pChildAction->getName() << "_stopper( _gid );\n";
                        
                    }
                    else if( const concrete::RangeAllocator* pChildRangeAllocator =
                            dynamic_cast< const concrete::RangeAllocator* >( pChildAllocator ) )
                    {
                        const DataMember* pChildAllocatorData = layout.getDataMember( pChildRangeAllocator->getAllocatorData() );
                        const DataMember* pStateData = layout.getDataMember( pChildAction->getState() );
                        
            os << "        if( !" << *printerFactory.read( pChildAllocatorData, "_gid" ) << ".empty() )\n";
            os << "        {\n";
            os << "            for( " << EG_INSTANCE << " childIndex = _gid * " << pChildAction->getLocalDomainSize() << 
                                    "; childIndex != ( _gid + 1 ) * " << pChildAction->getLocalDomainSize() << "; ++childIndex )\n";
            os << "            {\n";
            os << "                if( " << *printerFactory.read( pStateData, "childIndex" ) << " != " << getActionState( action_stopped ) << " )\n";
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
    
    void generateStopperLinkBreaks( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
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
            
            
            os << "        if( " << *printerFactory.read( pBackRefData, "_gid" ) << ".timestamp != eg::INVALID_TIMESTAMP )\n";
            os << "        {\n";
            os << "            switch( " << *printerFactory.read( pBackRefData, "_gid" ) << ".type )\n";
            os << "            {\n";
            for( const concrete::Action* pLink : pLinkGroup->getConcreteLinks() )
            {
            os << "                case " << pLink->getIndex() << ":\n";
            os << "                {\n";
            os << "                     " << *printerFactory.write( pLinkRefCountDataMember, osObjectDomain.str().c_str() ) << 
                " = " << *printerFactory.read( pLinkRefCountDataMember, osObjectDomain.str().c_str() ) << " - 1;\n";
                
            os << "                     const eg::reference backRef = " << *printerFactory.read( pBackRefData, "_gid" ) << ";\n";
            os << "                     " << *printerFactory.write( pBackRefData, "_gid" ) << " = { 0, 0, 0 };\n";
                const concrete::Dimension_User* pLinkBaseDimension = pLink->getLinkBaseDimension();
                VERIFY_RTE( pLinkBaseDimension );
                const DataMember* pLinkBaseData = layout.getDataMember( pLinkBaseDimension );
                VERIFY_RTE( pLinkBaseData );
            
            os << "                     " << *printerFactory.write( pLinkBaseData, "backRef.instance" ) << ".data = { 0, 0, 0 };\n";
            os << "                     ::eg::Scheduler::stop_ref( backRef );\n";
                
       // TODO - should stopping a link target trigger referenced count based stop  
            
            os << "                }\n";
            os << "                break;\n";
            
            }   
            os << "                default: ERR( \"Unknown link type\" ); break;\n";
            os << "            }\n";
            os << "        }\n";
        }
        
    }
    
	void generateMainActionStopper( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
	{
        os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        os << "     " << EG_INSTANCE << " _parent_id = _gid / " << pAction->getLocalDomainSize() << ";\n";
        
		const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
		
        os << "     if( " << *printerFactory.read( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
        os << "     {\n";
        os << "         " << *printerFactory.write( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
        os << "         " << *printerFactory.write( pCycleData, "_gid" ) << " = clock::cycle( " << pAction->getIndex() << " );\n";
        os << "         events::put( \"stop\", clock::cycle( " << pAction->getIndex() << " ), &" << *printerFactory.read( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        //stop the subtree
        generateStopperLinkBreaks( os, printerFactory, layout, linkAnalysis, pAction );
        generateSubTreeStop( os, printerFactory, layout, pAction );
        os << "     }\n";
		
        os << "}\n";
        os << "\n";
	}
	void generateExecutableActionStopper( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
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
            os << "    if( " << *printerFactory.read( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
            os << "    {\n";
            os << "        ::eg::Scheduler::stopperStopped( " << *printerFactory.read( pReferenceData, "_gid" ) << ".data );\n";
            os << "        " << *printerFactory.write( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
            os << "        " << *printerFactory.write( pCycleData, "_gid" ) << " = clock::cycle( " << pAction->getIndex() << " );\n";
            os << "        events::put( \"stop\", clock::cycle( " << pAction->getIndex() << " ), &" << *printerFactory.read( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            //stop the subtree
            generateStopperLinkBreaks( os, printerFactory, layout, linkAnalysis, pAction );
            generateSubTreeStop( os, printerFactory, layout, pAction );
        
            os << "    }\n";
            os << "    else\n";
            os << "    {\n";
            std::ostringstream osError;
            osError << "Error attempting to stop type: " << pAction->getName();
            os << "        events::put( \"error\", clock::cycle( 0 ), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
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
            os << "    if( " << *printerFactory.read( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
            os << "    {\n";
            os << "        ::eg::Scheduler::stopperStopped( " << *printerFactory.read( pReferenceData, "_gid" ) << ".data );\n";
            os << "        const " << EG_INSTANCE << " _parentIndex = " << "_gid / " << pAction->getLocalDomainSize() << ";\n";
            os << "        const " << EG_INSTANCE << " freeIndex = " << "_gid - _parentIndex * " << pAction->getLocalDomainSize() << ";\n";
            os << "        " << *printerFactory.write( pAllocatorData, "_parentIndex" ) << ".free( freeIndex );\n";
            os << "        " << *printerFactory.write( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
            os << "        " << *printerFactory.write( pCycleData, "_gid" ) << " = clock::cycle( " << pAction->getIndex() << " );\n";
            os << "        events::put( \"stop\", clock::cycle( 0 ), &" << *printerFactory.read( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
            
            //stop the subtree
            generateStopperLinkBreaks( os, printerFactory, layout, linkAnalysis, pAction );
            generateSubTreeStop( os, printerFactory, layout, pAction );
        
            os << "    }\n";
            os << "    else\n";
            os << "    {\n";
            std::ostringstream osError;
            osError << "Error attempting to stop type: " << pAction->getName();
            os << "        events::put( \"error\", clock::cycle( 0 ), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
            os << "    }\n";
            os << "}\n";
            os << "\n";
        }
        else
        {
            THROW_RTE( "Unknown allocator type" );
        }
	}

	void generateBreaker( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const concrete::Action* pAction )
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
        
        os << "  const " << EG_REFERENCE_TYPE << " currentBase = " << *printerFactory.read( pLinkBaseData, "_gid" ) << ".data;\n";
        
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
        os << "      case " << pTargetType->getIndex() << ": " << *printerFactory.write( pLinkRef, "currentBase.instance" ) << " = { 0, 0, 0 }; break;\n";
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
                    
        os << "      case " << pTargetType->getIndex() << ": " << *printerFactory.write( pLinkRef, "currentBase.instance" ) << " = { 0, 0, 0 };\n";
        os << "           " << *printerFactory.write( pLinkRefCountDataMember, osDomain.str().c_str() ) << " = " << *printerFactory.read( pLinkRefCountDataMember, osDomain.str().c_str() ) << " - 1;\n";
        os << "           if( " << *printerFactory.read( pLinkRefCountDataMember, osDomain.str().c_str() ) << " == 0 )\n";
        os << "           {\n";
        os << "               " << pObject->getName() << "_stopper( " << *printerFactory.read( pReferenceData, osDomain.str().c_str() ) << ".data.instance );\n";
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
    
    void generateActionInstanceFunctionsForwardDecls( std::ostream& os, const concrete::Action* pAction )
    {
        if( pAction->getContext()->isMainExecutable() )
        {
            os << "extern " << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter();\n";
            os << "extern void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid );\n";
            if( dynamic_cast< const interface::Link* >( pAction->getContext() ) )
                os << "extern void " << pAction->getName() << "_breaker( " << EG_INSTANCE << " _gid );\n";
        }
        else if( pAction->getContext()->isExecutable() )
        {
            os << "extern " << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _gid );\n";
            os << "extern void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid );\n";
            if( dynamic_cast< const interface::Link* >( pAction->getContext() ) )
                os << "extern void " << pAction->getName() << "_breaker( " << EG_INSTANCE << " _gid );\n";
        }
    }
    
    void generateActionInstanceFunctionsForwardDecls( std::ostream& os, const std::vector< const concrete::Action* >& actions )
    {
        os << "\n";
        os << "//Starter / Stopper / Breaker forward declarations\n";
        
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getStopCycle() && pAction->getState() )
            {
                os << "\n";
                generateActionInstanceFunctionsForwardDecls( os, pAction );
            }
        }
        
        os << "\n";
    }
    
    void generateActionInstanceFunctions( std::ostream& os, PrinterFactory& printerFactory, const Layout& layout, const LinkAnalysis& linkAnalysis, const concrete::Action* pAction )
    {
        if( pAction->getStopCycle() && pAction->getState() )
        {
            if( pAction->getContext()->isMainExecutable() )
            {
				generateMainActionStarter( os, printerFactory, layout, pAction );
				generateMainActionStopper( os, printerFactory, layout, linkAnalysis, pAction );
            }
			else if( pAction->getContext()->isExecutable() )
            {
                generateExecutableActionStarter( os, printerFactory, layout, pAction );
				generateExecutableActionStopper( os, printerFactory, layout, linkAnalysis, pAction );
            }
		}
        if( dynamic_cast< const interface::Link* >( pAction->getContext() ) )
        {
            generateBreaker( os, printerFactory, layout, pAction );
        }
    }

    void generateActionInstanceFunctions( std::ostream& os, PrinterFactory& printerFactory, const ReadSession& program )
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
                generateActionInstanceFunctions( os, printerFactory, layout, linkAnalysis, pAction );
            }
        }
        os << "\n";
    }
}