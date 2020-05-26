
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
            if( pAction->getParent() )
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
            if( pAction->getParent() )
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
            if( pAction->getParent() )
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
	
	
	void generateExecutableActionStarter( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
	{
		VERIFY_RTE( pAction->getParent() && pAction->getParent()->getParent() );
		const concrete::Action* pParentAction = dynamic_cast< const concrete::Action* >( pAction->getParent() );
		VERIFY_RTE( pParentAction );
		
		const DataMember* pIteratorData = layout.getDataMember( pParentAction->getIterator( pAction ) );
		const DataMember* pAllocatorData = layout.getDataMember( pAction->getAllocatorData() );
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
		const DataMember* pRingIndex = layout.getDataMember( pAction->getRingIndex() );
		const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
                    
        /////starter
        {
        os << getStaticType( pAction->getAction() ) << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _parent_id )\n";
        os << "{\n";
        os << "    //claim next free index\n";
        os << "    " << EG_RING_BUFFER_ALLOCATOR_TYPE << " iter;\n";
        os << "    while( true )\n";
        os << "    {\n";
        os << "         iter = " << Printer( pIteratorData, "_parent_id" ) << ";\n";
        os << "         if( iter.protection )\n";
        os << "             continue;\n";
        os << "         else if( iter.full )\n";
        os << "             break;\n";
        os << "         const " << EG_INSTANCE << " relativeNextCellIndex = static_cast< " << EG_INSTANCE << " >( iter.head );\n";
        os << "         //claim the next free index\n";
        os << "         if( relativeNextCellIndex == " << pAction->getLocalDomainSize() - 1U << " )\n";
        os << "         {\n";
        os << "             iter.head = 0U;\n";
        os << "         }\n";
        os << "         else\n";
        os << "         {\n";
        os << "             ++iter.head;\n";
        os << "         }\n";
        os << "         if( static_cast< " << EG_INSTANCE << " >( iter.head ) == static_cast< " << EG_INSTANCE << " >( iter.tail ) )\n";
        os << "         {\n";
        os << "             iter.full = 1U;\n";
        os << "         }\n";
        
        //need to calculate the index based on local domain size
        std::ostringstream osNextIndex;
        osNextIndex << "_parent_id * " << pAction->getLocalDomainSize() << " + relativeNextCellIndex";
		os << "         " << Printer( pIteratorData, "_parent_id" ) << " = iter.data;\n";
        os << "         const " << EG_INSTANCE << " nextRingIndex = " << osNextIndex.str().c_str() << ";\n";
        os << "         const " << EG_INSTANCE << " nextInstance = " << Printer( pAllocatorData, "nextRingIndex" ) << ";\n";
        os << "         if( " << Printer( pCycleData, "nextInstance" ) << " < clock::cycle() )\n";
        os << "         {\n";
        os << "             //successfully claimed valid allocation index\n";
        os << "             const " << EG_INSTANCE << " startCycle = clock::cycle();\n";
        os << "             " << getStaticType( pAction->getAction() ) << "& reference = " << 
                            Printer( pReferenceData, "nextInstance" ) << ";\n";
        os << "             reference.data.timestamp = startCycle;\n";
        os << "             " << Printer( pStateData, "nextInstance" ) << " = " << getActionState( action_running ) << ";\n";
        os << "             " << Printer( pRingIndex, "nextInstance" ) << " = nextRingIndex;\n";
        os << "             events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        os << "             return reference;\n";
        os << "         }\n";
        os << "    }\n";   
        os << "    //failure return null handle\n";
        std::ostringstream osError;
        osError << "Failed to allocate " << pAction->getFriendlyName();
        os << "    events::put( \"error\", clock::cycle(), \"" << osError.str() << "\", " << osError.str().size() + 1 << ");\n";
        os << "    " << getStaticType( pAction->getAction() ) << " nullInstance;\n";
        os << "    return nullInstance;\n";
        os << "}\n";
        os << "\n";
        }
	}
	
	void generateMainActionStarter( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
	{
		//simple starter for root
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
				
        os << getStaticType( pAction->getAction() ) << " " << pAction->getName() << "_starter()\n";
        os << "{\n";
        os << "    const " << EG_INSTANCE << " startCycle = clock::cycle();\n";
        os << "    " << getStaticType( pAction->getAction() ) << "& reference = " << Printer( pReferenceData, "0" ) << ";\n";
        os << "    reference.data.timestamp = startCycle;\n";
        os << "    " << Printer( pStateData, "0" ) << " = " << getActionState( action_running ) << ";\n";
        os << "    events::put( \"start\", startCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        os << "    return reference;\n";
        os << "}\n";
        os << "\n";
	}
	
	void generateMainActionStopper( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
	{
        os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        os << "     " << EG_INSTANCE << " _parent_id = _gid / " << pAction->getLocalDomainSize() << ";\n";
        
		const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
		
        os << "     if( " << Printer( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
        os << "     {\n";
                        //stop the subtree
                for( const concrete::Element* pChild : pAction->getChildren() )
                {
                    if( const concrete::Action* pChildAction = dynamic_cast< const concrete::Action* >( pChild ) )
                    {
        os << "         for( " << EG_INSTANCE << " childIndex = _gid * " << pChildAction->getLocalDomainSize() << 
                                "; childIndex != ( _gid + 1 ) * " << pChildAction->getLocalDomainSize() << "; ++childIndex )\n";
        os << "         {\n";
        os << "             " << pChildAction->getName() << "_stopper( childIndex );\n";
        os << "         }\n";
                    }
                }
        os << "         " << Printer( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
        os << "         " << Printer( pCycleData, "_gid" ) << " = clock::cycle();\n";
        os << "         events::put( \"stop\", clock::cycle(), &" << Printer( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        os << "     }\n";
		
        os << "}\n";
        os << "\n";
	}
	void generateExecutableActionStopper( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
	{
		////stopper
        os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        os << "     " << EG_INSTANCE << " _parent_id = _gid / " << pAction->getLocalDomainSize() << ";\n";
        
		const concrete::Action* pParentAction = dynamic_cast< const concrete::Action* >( pAction->getParent() );
		VERIFY_RTE( pParentAction );
		
		const DataMember* pIteratorData = layout.getDataMember( pParentAction->getIterator( pAction ) );
		const DataMember* pAllocatorData = layout.getDataMember( pAction->getAllocatorData() );
		const DataMember* pCycleData = layout.getDataMember( pAction->getStopCycle() );
		const DataMember* pStateData = layout.getDataMember( pAction->getState() );
		const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
		const DataMember* pRingIndex = layout.getDataMember( pAction->getRingIndex() );
                
        os << "     if( " << Printer( pStateData, "_gid" ) << " != " << getActionState( action_stopped ) << " )\n";
        os << "     {\n";
                
        os << "         " << EG_RING_BUFFER_ALLOCATOR_TYPE << " iter;\n";
        os << "         while( true )\n";
        os << "         {\n";
        os << "              iter = " << Printer( pIteratorData, "_parent_id" ) << ";\n";
        os << "              if( iter.protection )\n";
        os << "                  continue;\n";
        os << "              const " << EG_INSTANCE << " ringBufferTailIndex = _parent_id * " << pAction->getLocalDomainSize() << " + static_cast< " << EG_INSTANCE << " >( iter.tail );\n";
        os << "              //if buffer is full then set the protection bit while freeing\n";
        os << "              if( iter.full )\n";
        os << "              {\n";
        os << "                  iter.protection = 1U;\n";
        os << "                  iter.full = 0U;\n";
        os << "              }\n";
        os << "              //claim the index to store free instance\n";
        os << "              if( static_cast< " << EG_INSTANCE << " >( iter.tail ) == " << pAction->getLocalDomainSize() << " - 1U )\n";
        os << "              {\n";
        os << "                  iter.tail = 0U;\n";
        os << "              }\n";
        os << "              else\n";
        os << "              {\n";
        os << "                  ++iter.tail;\n";
        os << "              }\n";
		os << "              " << Printer( pIteratorData, "_parent_id" ) << " = iter.data;\n";
        os << "              {\n";
        os << "                  //successfully freed index\n";
        os << "                  const " << EG_INSTANCE << " ringIndex = " << Printer( pRingIndex, "_gid" ) << ";\n";
        os << "                  if( ringBufferTailIndex != ringIndex )\n";
        os << "                  {\n";
        os << "                      const " << EG_INSTANCE << " tailInstance = " << Printer( pAllocatorData, "ringBufferTailIndex" ) << ";\n";
        os << "                      " << Printer( pAllocatorData, "ringIndex" ) << " = tailInstance;\n";
        os << "                      " << Printer( pRingIndex, "tailInstance" ) << " = ringIndex;\n";
        os << "                  }\n";
        os << "                  " << Printer( pAllocatorData, "ringBufferTailIndex" ) << " = _gid;\n";
        os << "                  if( iter.protection )\n";
        os << "                  {\n";
        os << "                      //turn off the protection bit\n";
        os << "                      iter.protection = 0;\n";
        os << "                      " << Printer( pIteratorData, "_parent_id" ) << " = iter.data;\n";
        os << "                  }\n";
        os << "                  break;\n";
        os << "              }\n";
        os << "         }\n";   
                        
                        
                        //stop the subtree
                for( const concrete::Element* pChild : pAction->getChildren() )
                {
                    if( const concrete::Action* pChildAction = dynamic_cast< const concrete::Action* >( pChild ) )
                    {
        os << "         for( " << EG_INSTANCE << " childIndex = _gid * " << pChildAction->getLocalDomainSize() << 
                                "; childIndex != ( _gid + 1 ) * " << pChildAction->getLocalDomainSize() << "; ++childIndex )\n";
        os << "         {\n";
        os << "             " << pChildAction->getName() << "_stopper( childIndex );\n";
        os << "         }\n";
                    }
                }
        
        os << "         " << Printer( pStateData, "_gid" ) << " = " << getActionState( action_stopped ) << ";\n";
        os << "         " << Printer( pCycleData, "_gid" ) << " = clock::cycle();\n";
        os << "         events::put( \"stop\", clock::cycle(), &" << Printer( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) );\n";
        os << "     }\n";
        
        os << "}\n";
        os << "\n";
	}

    void generateActionInstanceFunctions( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
    {
        if( pAction->getStopCycle() && pAction->getState() )
        {
            if( pAction->getAction()->isMainExecutable() )
            {
				generateMainActionStarter( os, layout, pAction );
				generateMainActionStopper( os, layout, pAction );
            }
			else if( pAction->getAction()->isExecutable() )
            {
                generateExecutableActionStarter( os, layout, pAction );
				generateExecutableActionStopper( os, layout, pAction );
            }
		}
    }

    void generateActionInstanceFunctions( std::ostream& os, const ReadSession& program )
    {
        const interface::Root* pRoot = program.getTreeRoot();
        
        const DerivationAnalysis& derivationAnalysis = program.getDerivationAnalysis();
        const Layout& layout = program.getLayout();
        const IndexedObject::Array& objects = program.getObjects( eg::IndexedObject::MASTER_FILE );
        std::vector< const concrete::Action* > actions = 
            many_cst< concrete::Action >( objects );
            
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() )
            {
                //os << getStaticType( pAction->getAction() ) << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _gid );\n";
                os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid );\n";
            }
        }
        
        os << "//input::Action Function Implementations\n";
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() )
            {
                os << "\n";
                generateActionInstanceFunctions( os, layout, pAction );
            }
        }
        os << "\n";
    }
}