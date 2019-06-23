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


#include "eg/implementation_session.hpp"
#include "eg/codegen.hpp"
#include "eg/input.hpp"

#include <ostream>

inline std::string getFuncName( const eg::concrete::Action* pAction, const char* pszName )
{
    std::ostringstream os;
    os << pAction->getName() << "_" << pszName;
    return os.str();
}

inline std::string getFuncName( const eg::DataMember* pDataMember, const char* pszName )
{
    std::ostringstream os;
    os << pDataMember->getBuffer()->getVariableName() << '_' << pDataMember->getName() << "_" << pszName;
    return os.str();
}


void generate_python( std::ostream& os, eg::ReadSession& session )
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

    os << "\n";
    os << "//Python Interface Converters\n";
    os << "std::shared_ptr< eg::PythonEGReferenceType > g_pEGRefType;\n";
    os << "namespace pybind11\n";
    os << "{\n";
    os << "    namespace detail\n";
    os << "    {\n";
    
    std::vector< const eg::interface::Action* > abstractActions = 
        eg::many_cst< eg::interface::Action >( objects );
        
    using ActionTypeMap = std::map< const eg::interface::Action*, std::string, eg::CompareIndexedObjects >;
    ActionTypeMap actionTypeMap;
    
    for( const eg::interface::Action* pAbstractAction : abstractActions )
    {
        if( pAbstractAction->getParent() )
        {
            std::ostringstream osType;
            {
                std::vector< const eg::interface::Element* > path = eg::interface::getPath( pAbstractAction );
                //generate type explicit template specialisation
                {
                    for( const eg::interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            osType << "::";
                        osType << eg::getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
                    }
                }
            }
            actionTypeMap.insert( std::make_pair( pAbstractAction, osType.str() ) );
        }
    }
    
    for( ActionTypeMap::const_iterator 
            i = actionTypeMap.begin(),
            iEnd = actionTypeMap.end(); i!=iEnd; ++i )
    {
        const eg::interface::Action* pAbstractAction = i->first;
        const std::string& strType = i->second;
    os << "        template <> struct type_caster< " << strType << " >\n";
    os << "        {\n";
    os << "        public:\n";
    os << "            PYBIND11_TYPE_CASTER( " << strType << ", _(\"pyeg.reference\"));\n";
    os << "        \n";
    os << "            bool load( handle src, bool )\n";
    os << "            {\n";
    os << "                const eg::PythonEGReference* pEGReference =\n";
    os << "                    eg::PythonEGReferenceType::getReference( src.ptr() );\n";
    os << "                value.data = pEGReference->getEGReference();\n";
    os << "                return !PyErr_Occurred();\n";
    os << "            }\n";
    os << "        \n";
    os << "            static handle cast( " << strType << " src, return_value_policy /* policy */, handle /* parent */)\n";
    os << "            {\n";
    os << "                return g_pEGRefType->create( src.data );\n";
    os << "            }\n";
    os << "        };\n";
    }
    os << "    }   //namespace detail\n";
    os << "} // namespace pybind11\n";
    

    os << "\n";
    os << "//Python Interface\n";
    
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
            os << "void " << getFuncName( pAction, "pause" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            os << "    if( " << eg::Printer( layout.getDataMember( pAction->getState() ), "instance" ) << " == " << eg::getActionState( eg::action_running ) << " )\n";
            os << "        " << eg::Printer( layout.getDataMember( pAction->getState() ), "instance" ) << " = " << eg::getActionState( eg::action_paused ) << ";\n";
            os << "}\n";
            
            os << "void " << getFuncName( pAction, "resume" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            os << "    if( " << eg::Printer( layout.getDataMember( pAction->getState() ), "instance" ) << " == " << eg::getActionState( eg::action_paused ) << " )\n";
            os << "        " << eg::Printer( layout.getDataMember( pAction->getState() ), "instance" ) << " = " << eg::getActionState( eg::action_running ) << ";\n";
            os << "}\n";
            
            os << "bool " << getFuncName( pAction, "done" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            
            const eg::concrete::Action::IteratorMap& iterators = pAction->getAllocators();
            os << "    " << eg::EG_ITERATOR_TYPE << " iter;\n";
            for( eg::concrete::Action::IteratorMap::const_iterator 
                i = iterators.begin(),
                iEnd = iterators.end(); i!=iEnd; ++i )
            {
                const eg::concrete::Dimension_Generated* pIterator = i->second;
                os << "    iter = " << eg::EG_ITERATOR_TYPE << "( " << 
                    eg::Printer( layout.getDataMember( pIterator ), "instance" ) << ".load() );\n";
                os << "    if( iter.full || ( iter.head != iter.tail ) ) return false;\n";
            }
            os << "    return true;\n";
            os << "}\n";
        }
    }
    
const char* pszSleepUtils = R"(

void python_sleep_cycle()
{
    eg::sleep();
}
    
)";
    os << pszSleepUtils;
    
    os << "\n//Embedded Python Module\n";
    os << "PYBIND11_EMBEDDED_MODULE( pyeg, module ) \n";
    os << "{\n";
    os << "    module.def( \"root\", get_root );\n";
    
    os << "    module.def( \"sleep\", python_sleep_cycle );\n";
    //os << "    module.def( \"sleep\", python_sleep_event );\n"; //TODO... 
    os << "    module.def( \"wait\", eg::wait );\n";
        
    os << "}\n";
        
    os << "\n";
    
    
    os << "HostFunctions::HostFunctions( const std::string& strDatabaseFile, pybind11::module module_eg )\n";
    os << "    :   m_pRuntime( std::shared_ptr< eg::EGRuntime >( \n";
    os << "            eg::constructRuntime( *this, strDatabaseFile.c_str() ) ) ),\n";
    os << "        m_module_eg( module_eg )\n";
    os << "{\n";
    os << "}\n";
    os << "//HostFunctionAccessor\n";
    
    
    std::vector< const eg::DataMember* > referenceDimensions;
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
                if( pDimension->isEGType() )
                {
                    referenceDimensions.push_back( pDataMember );
                }
            }
        }
    }
    
    os << "" << eg::EG_REFERENCE_TYPE << " HostFunctions::dereferenceDimension( const " << eg::EG_REFERENCE_TYPE << "& action, const " << eg::EG_TYPE_ID << "& dimensionType )\n";
    os << "{\n";
    if( !referenceDimensions.empty() )
    {
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( dimensionType )\n";
    os << "        {\n";
    
        for( const eg::DataMember* pDataMember : referenceDimensions )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
                if( pDimension->isEGType() )
                {
    os << "            case " << pDimension->getIndex() << ":\n";
    os << "                return " << eg::Printer( pDataMember, "action.instance" ) << ".data;\n";
                }
            }
        }
    
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "    return " << eg::EG_REFERENCE_TYPE << "{ 0, 0, 0 };\n";
    }
    else
    {
    os << "    throw std::runtime_error( \"Unreachable\" );\n";
    }
    os << "}\n";
    os << "void HostFunctions::doRead( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " dimensionType )\n";
    os << "{\n";
    
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( dimensionType )\n";
    os << "        {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "            case " << pDimension->getIndex() << ":\n";
    os << "                pStack->m_result = pybind11::cast( " << eg::Printer( pDataMember, "reference.instance" ) << " );\n";
    os << "                break;\n";
            }
        }
    }
    
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
           
    os << "}\n";
    os << "void HostFunctions::doWrite( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " dimensionType )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "        switch( dimensionType )\n";
    os << "        {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "            case " << pDimension->getIndex() << ":\n";
    os << "                 " << eg::Printer( pDataMember, "reference.instance" ) << " = pybind11::cast< "; pDimension->printType( os ); os << " >( args[ 0 ] );\n";
    os << "                 break;\n";
            }
        }
    }
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doCall( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " actionType )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "        switch( actionType )\n";
    os << "        {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() && pAction->getParent()->getParent() )
        {
    os << "            case " << pAction->getIndex() << ":\n";
    os << "                {\n";
    os << "                    " << pAction->getAction()->getStaticType() << " ref = " << pAction->getName() << "_starter( reference.instance );\n";
    os << "                    if( ref )\n";
    os << "                    {\n";
    
    const std::vector< std::string >& parameters = pAction->getAction()->getParameters();
    os << "                            ref(";
    bool bFirst = true;
    int iIndex = 0;
    for( const std::string& strParamType : parameters )
    {
        if( bFirst ) 
        {
            bFirst = false;
            os << " ";
        }
        else os << ", ";
        os << "pybind11::cast< " << strParamType << " >( args[ " << iIndex++ << " ] )";
    }
    os << ");\n";
    os << "                        " << pAction->getName() << "_stopper( ref.data.instance );\n";
    
    os << "                    }\n";
    os << "                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );\n";
    os << "                }\n";
    os << "                break;\n";
        }
    }
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doStart( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " actionType )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "        switch( actionType )\n";
    os << "        {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() && pAction->getParent()->getParent() )
        {
    os << "            case " << pAction->getIndex() << ":\n";
    os << "                {\n";
    os << "                    " << pAction->getAction()->getStaticType() << " ref = " << pAction->getName() << "_starter( reference.instance );\n";
    os << "                    if( ref )\n";
    os << "                    {\n";
    
    const std::vector< std::string >& parameters = pAction->getAction()->getParameters();
    
    os << "                        std::function< void() > functor = std::bind( &" << pAction->getAction()->getStaticType() << "::operator(), ref";
    int iIndex = 0;
    for( const std::string& strParamType : parameters )
    {
        os << ", pybind11::cast< " << strParamType << " >( args[ " << iIndex++ << " ] )";
    }
    os << ");\n";
    
    os << "                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber\n";
    os << "                        (\n";
    os << "                            std::allocator_arg,\n";
    os << "                            " << eg::EG_DEFAULT_FIBER_STACK_TYPE << ",\n";
    os << "                            [ functor, ref ]()\n";
    os << "                            {\n";
    os << "                                try\n";
    os << "                                {\n";
    os << "                                    functor();\n";
    os << "                                }\n";
    os << "                                catch( eg::termination_exception )\n";
    os << "                                {\n";
    os << "                                }\n";
    os << "                                " << pAction->getName() << "_stopper( ref.data.instance );\n";
    os << "                            }\n";
    os << "                        );\n";
    os << "                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );\n";
    os << "                    }\n";
    os << "                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );\n";
    os << "                }\n";
    os << "                break;\n";
        }
    }
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doStop( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( reference.type )\n";
    os << "        {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "            case " << pAction->getIndex() << ":\n";
    os << "                " << pAction->getName() << "_stopper( reference.instance );\n";
    os << "                break;\n";
        }
    }
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doPause( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( reference.type )\n";
    os << "        {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "            case " << pAction->getIndex() << ":\n";
    os << "                " << getFuncName( pAction, "pause" ) << "( reference.instance );\n";
    os << "                break;\n";
        }
    }
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doResume( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( reference.type )\n";
    os << "        {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "            case " << pAction->getIndex() << ":\n";
    os << "                " << getFuncName( pAction, "resume" ) << "( reference.instance );\n";
    os << "                break;\n";
        }
    }
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doDone( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( reference.type )\n";
    os << "        {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "            case " << pAction->getIndex() << ":\n";
    os << "                pStack->m_result = pybind11::cast( " << getFuncName( pAction, "done" ) << "( reference.instance ) );\n";
    os << "                break;\n";
        }
    }
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doWaitAction( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( reference ) );\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doWaitDimension( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " dimensionType )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( dimensionType )\n";
    os << "        {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "            case " << pDimension->getIndex() << ":\n";
    //os << "                pStack->m_result = m_module_eg.attr( \"" << getFuncName( pDataMember, "read" ) << "\" )( reference.instance );\n";
    os << "                break;\n";
            }
        }
    }
    
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doGetAction( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( reference ) );\n";
    os << "    }\n";
    os << "}\n";
    os << "void HostFunctions::doGetDimension( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " dimensionType )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        switch( dimensionType )\n";
    os << "        {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "            case " << pDimension->getIndex() << ":\n";
    os << "                pStack->m_result = pybind11::cast( " << eg::Printer( pDataMember, "reference.instance" ) << " );\n";
    os << "                break;\n";
            }
        }
    }
    
    os << "            default:\n";
    os << "                break;\n";
    os << "        }\n";
    os << "    }\n";
    os << "}\n";
    
    os << "void HostFunctions::doRange( eg::EGRangeDescriptionPtr pRange )\n";
    os << "{\n";
    os << "    if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "    {\n";
    os << "        pStack->m_result = pybind11::make_iterator( eg::PythonIterator( *g_pEGRefType, pRange, false ), eg::PythonIterator( *g_pEGRefType, pRange, true ) );\n";
    
    os << "    }\n";
    os << "}\n";
    
    
    os << "void HostFunctions::getIdentities( std::vector< const char* >& identities )\n";
    os << "{\n";
    os << "    m_pRuntime->getIdentities( identities );\n";
    os << "}\n";
    os << "" << eg::EG_TYPE_ID << " HostFunctions::getTypeID( const char* pszIdentity )\n";
    os << "{\n";
    os << "    return m_pRuntime->getTypeID( pszIdentity );\n";
    os << "}\n";
    os << "PyObject* HostFunctions::invoke( const " << eg::EG_REFERENCE_TYPE << "& reference, const std::vector< " << eg::EG_TYPE_ID << " >& typePath, PyObject *args, PyObject *kwargs )\n";
    os << "{\n";
    os << "    if( reference.type != 0 )\n";
    os << "    {\n";
    os << "        Stack::SharedPtr pStack = std::make_shared< Stack >( args, kwargs );\n";
    os << "        m_pStack = pStack;\n";
    os << "        pybind11::args pyArgs = pybind11::reinterpret_borrow< pybind11::args >( args );\n";
    os << "        m_pRuntime->invoke( reference, typePath, pyArgs.size() != 0 );\n";
    os << "        if( pStack->m_result )\n";
    os << "        {\n";
    os << "            pybind11::handle h = pStack->m_result;\n";
    os << "            h.inc_ref();\n";
    os << "            return h.ptr();\n";
    os << "        }\n";
    os << "    }\n";
    os << "    else\n";
    os << "    {\n";
    os << "        throw std::runtime_error( \"Invalid reference used in invocation\" );\n";
    os << "    }\n";
    os << "    Py_INCREF( Py_None );\n";
    os << "    return Py_None;\n";
    os << "}\n";
    
    os << "\n";
    
}