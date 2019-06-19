
#include "eg/implementation_session.hpp"
#include "eg/codegen.hpp"
#include "eg/input.hpp"

#include <ostream>

void generate_dynamic_interface( std::ostream& os, eg::ReadSession& session )
{
    const eg::interface::Root* pRoot = session.getTreeRoot();
    const eg::concrete::Action* pInstanceRoot = nullptr;
    {
        std::vector< const eg::concrete::Action* > roots;
        for( const eg::concrete::Element* pChild : session.getInstanceRoot()->getChildren() )
        {
            if( const eg::concrete::Action* pAction = 
                dynamic_cast< const eg::concrete::Action* >( pChild ) )
            {
                roots.push_back( pAction );
            }
        }
        ASSERT( !roots.empty() );
        ASSERT( roots.size() == 1U );
        pInstanceRoot = roots.front();
    }
    
    const eg::DerivationAnalysis& derivationAnalysis = session.getDerivationAnalysis();
    const eg::Layout& layout = session.getLayout();
    const eg::IndexedObject::Array& objects = session.getObjects( eg::IndexedObject::MASTER_FILE );
    std::vector< const eg::concrete::Action* > actions = 
        eg::many_cst< eg::concrete::Action >( objects );
    
    if( !actions.empty() )
    {
    os << "eg::TimeStamp getTimestamp( " << eg::EG_TYPE_ID << " typeID, " << eg::EG_INSTANCE << " instance )\n";
    os << "{\n";
    os << "    switch( typeID )\n";
    os << "    {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "        case " << pAction->getIndex() << ": return " << 
        eg::Printer( layout.getDataMember( pAction->getReference() ), "instance" ) << ".data.timestamp;\n";
        }
    }
    os << "        default: throw std::runtime_error( \"Invalid action instance\" );\n";
    os << "    }\n";
    os << "}\n";
    
    os << eg::EG_ACTION_STATE << " getState( " << eg::EG_TYPE_ID << " typeID, " << eg::EG_INSTANCE << " instance )\n";
    os << "{\n";
    os << "    switch( typeID )\n";
    os << "    {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "        case " << pAction->getIndex() << ": return " << 
        eg::Printer( layout.getDataMember( pAction->getState() ), "instance" ) << ";\n";
        }
    }
    os << "        default: throw std::runtime_error( \"Invalid action instance\" );\n";
    os << "    }\n";
    os << "}\n";
    
    os << eg::EG_FIBER_TYPE << "& getFiber( " << eg::EG_TYPE_ID << " typeID, " << eg::EG_INSTANCE << " instance )\n";
    os << "{\n";
    os << "    switch( typeID )\n";
    os << "    {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "        case " << pAction->getIndex() << ": return " << 
        eg::Printer( layout.getDataMember( pAction->getFiber() ), "instance" ) << ";\n";
        }
    }
    os << "        default: throw std::runtime_error( \"Invalid action instance\" );\n";
    os << "    }\n";
    os << "}\n";
    
    os << eg::EG_TIME_STAMP << " getStopCycle( " << eg::EG_TYPE_ID << " typeID, " << eg::EG_INSTANCE << " instance )\n";
    os << "{\n";
    os << "    switch( typeID )\n";
    os << "    {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "        case " << pAction->getIndex() << ": return " << 
        eg::Printer( layout.getDataMember( pAction->getStopCycle() ), "instance" ) << ";\n";
        }
    }
    os << "        default: throw std::runtime_error( \"Invalid action instance\" );\n";
    os << "    }\n";
    os << "}\n";
    }
    
    os << eg::getInterfaceType( eg::input::Root::RootTypeName ) << "< void > get_root()\n";
    os << "{\n";
    os << "    return  " << eg::getInterfaceType( eg::input::Root::RootTypeName ) << "< void >( " << 
        eg::EG_REFERENCE_TYPE << "{ 0, " << pInstanceRoot->getIndex() << ", getTimestamp( " << pInstanceRoot->getIndex() << ", 0 ) } );\n";
    os << "}\n";
    os << "\n";
    
}