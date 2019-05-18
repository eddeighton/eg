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
    const eg::concrete::Action* pInstanceRoot = session.getInstanceRoot();
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
        
    using ActionTypeMap = std::map< const eg::interface::Action*, std::string >;
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
    
    if( !actions.empty() )
    {
    os << "eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance )\n";
    os << "{\n";
    os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
    os << "    switch( typeID )\n";
    os << "    {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "        case " << pAction->getIndex() << ": return " << 
        eg::Printer( layout.getDataMember( pAction->getRunningTimestamp() ), "instance" ) << ";\n";
        }
    }
    os << "        default: return eg::INVALID_TIMESTAMP;\n";
    os << "    }\n";
    os << "}\n";
    }
    
    os << eg::getInterfaceType( eg::input::Root::RootTypeName ) << "< void > get_root()\n";
    os << "{\n";
    os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
    os << "    return  " << eg::getInterfaceType( eg::input::Root::RootTypeName ) << "< void >( " << 
        eg::EG_REFERENCE_TYPE << "{ 0, " << pInstanceRoot->getIndex() << ", getTimestamp( 0, " << pInstanceRoot->getIndex() << " ) } );\n";
    os << "}\n";
    os << "\n";
    
    //sleep
    //sleep seconds
    //sleep until
    //wait
    //wait until
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
                pDimension->printType( os );
                os << " " << getFuncName( pDataMember, "read" ) << "( " << eg::EG_INSTANCE << " instance )\n";
                os << "{\n";
                os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
                os << "    return " << eg::Printer( pDataMember, "instance" ) << ";\n";
                os << "}\n";
                os << "void " << getFuncName( pDataMember, "write" ) << "( " << eg::EG_INSTANCE << " instance, pybind11::tuple args )\n";
                os << "{\n";
                os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
                os << "    " << eg::Printer( pDataMember, "instance" ) << " = pybind11::cast< "; pDimension->printType( os ); os << " >( args[ 0 ] );\n";
                os << "}\n";
            }
        }
    }
    
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
            pAction->printType( os );
            os << " " << getFuncName( pAction, "start" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
            os << "    return " << pAction->getName() << "_starter( instance );\n";
            os << "}\n";
            
            os << "void " << getFuncName( pAction, "stop" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
            os << "    " << pAction->getName() << "_stopper( instance );\n";
            os << "}\n";
            
            os << "void " << getFuncName( pAction, "pause" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
            os << "    " << eg::Printer( layout.getDataMember( pAction->getPauseTimestamp() ), "instance" ) << " = " << eg::EG_INVALID_TIMESTAMP << ";\n";
            os << "}\n";
            
            os << "void " << getFuncName( pAction, "resume" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
            os << "    " << eg::Printer( layout.getDataMember( pAction->getPauseTimestamp() ), "instance" ) << " = clock::subcycle() + 1;\n";
            os << "}\n";
            
            os << "bool " << getFuncName( pAction, "done" ) << "( " << eg::EG_INSTANCE << " instance )\n";
            os << "{\n";
            os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
            
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
    
    os << "\n//Embedded Python Module\n";
    os << "PYBIND11_EMBEDDED_MODULE( pyeg, module ) \n";
    os << "{\n";
    os << "    module.def( \"get_root\", get_root );\n";
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "    module.def( \"" << getFuncName( pDataMember, "read" )    << "\", " << getFuncName( pDataMember, "read" )     << " );\n";
    os << "    module.def( \"" << getFuncName( pDataMember, "write" )   << "\", " << getFuncName( pDataMember, "write" )    << " );\n";
            }
        }
    }
    
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "    module.def( \"" << getFuncName( pAction, "start" )   << "\", " << getFuncName( pAction, "start" )    << ");\n";
    os << "    module.def( \"" << getFuncName( pAction, "stop" )    << "\", " << getFuncName( pAction, "stop" )     << ");\n";
    os << "    module.def( \"" << getFuncName( pAction, "pause" )   << "\", " << getFuncName( pAction, "pause" )    << ");\n";
    os << "    module.def( \"" << getFuncName( pAction, "resume" )  << "\", " << getFuncName( pAction, "resume" )   << ");\n";
    os << "    module.def( \"" << getFuncName( pAction, "done" )    << "\", " << getFuncName( pAction, "done" )     << ");\n";
        }
    }
    
    os << "}\n";
        
    os << "\n";
    
    os << "struct HostFunctions : public eg::HostFunctionAccessor, public eg::HostEvaluator\n";
    os << "{\n";
    os << "private:\n";
    os << "    struct Stack\n";
    os << "    {\n";
    os << "        PyObject *args;\n";
    os << "        PyObject *kwargs;\n";
    os << "        pybind11::object m_result;\n";
    os << "        Stack( PyObject *args, PyObject *kwargs )\n";
    os << "            :   args( args ), kwargs( kwargs )\n";
    os << "        {\n";
    os << "        }\n";
    os << "        using WeakPtr = std::weak_ptr< Stack >;\n";
    os << "        using SharedPtr = std::shared_ptr< Stack >;\n";
    os << "    };\n";
    os << "public:\n";
    os << "    HostFunctions( const std::string& strDatabaseFile, pybind11::module module_eg )\n";
    os << "        :   m_pRuntime( std::shared_ptr< eg::EGRuntime >( \n";
    os << "                eg::constructRuntime( *this, strDatabaseFile.c_str() ) ) ),\n";
    os << "            m_module_eg( module_eg )\n";
    os << "    {\n";
    os << "    }\n";
    os << "    //HostFunctionAccessor\n";
    
    
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
    
    os << "    virtual " << eg::EG_REFERENCE_TYPE << " dereferenceDimension( const " << eg::EG_REFERENCE_TYPE << "& action, const " << eg::EG_TYPE_ID << "& dimensionType )\n";
    os << "    {\n";
    if( !referenceDimensions.empty() )
    {
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            switch( dimensionType )\n";
    os << "            {\n";
    
        for( const eg::DataMember* pDataMember : referenceDimensions )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
                if( pDimension->isEGType() )
                {
    os << "                case " << pDimension->getIndex() << ":\n";
    os << "                    return " << eg::Printer( pDataMember, "action.instance" ) << ".data;\n";
                }
            }
        }
    
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "        return " << eg::EG_REFERENCE_TYPE << "{ 0, 0, 0 };\n";
    }
    else
    {
    os << "        throw std::runtime_error( \"Unreachable\" );\n";
    }
    os << "    }\n";
    os << "    virtual void doRead( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " dimensionType )\n";
    os << "    {\n";
    
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            switch( dimensionType )\n";
    os << "            {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "                case " << pDimension->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << getFuncName( pDataMember, "read" ) << "\" )( reference.instance );\n";
    os << "                    break;\n";
            }
        }
    }
    
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
               
    os << "    }\n";
    os << "    virtual void doWrite( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " dimensionType )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "            switch( dimensionType )\n";
    os << "            {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "                case " << pDimension->getIndex() << ":\n";
    os << "                     pStack->m_result = m_module_eg.attr( \"" << getFuncName( pDataMember, "write" ) << "\" )( reference.instance, args );\n";
    os << "                     break;\n";
            }
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doStart( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " actionType )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    //os << "            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "            switch( actionType )\n";
    os << "            {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "                case " << pAction->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << getFuncName( pAction, "start" ) << "\" )( reference.instance );\n";
    os << "                    break;\n";
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doStop( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    //os << "            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "            switch( reference.type )\n";
    os << "            {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "                case " << pAction->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << getFuncName( pAction, "stop" ) << "\" )( reference.instance );\n";
    os << "                    break;\n";
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doPause( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    //os << "            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "            switch( reference.type )\n";
    os << "            {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "                case " << pAction->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << getFuncName( pAction, "pause" ) << "\" )( reference.instance );\n";
    os << "                    break;\n";
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doResume( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    //os << "            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "            switch( reference.type )\n";
    os << "            {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "                case " << pAction->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << getFuncName( pAction, "resume" ) << "\" )( reference.instance );\n";
    os << "                    break;\n";
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doDone( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    //os << "            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "            switch( reference.type )\n";
    os << "            {\n";
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "                case " << pAction->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << getFuncName( pAction, "done" ) << "\" )( reference.instance );\n";
    os << "                    break;\n";
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doGetAction( const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( reference ) );\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doGetDimension( const " << eg::EG_REFERENCE_TYPE << "& reference, " << eg::EG_TYPE_ID << " dimensionType )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            switch( dimensionType )\n";
    os << "            {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "                case " << pDimension->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << getFuncName( pDataMember, "read" ) << "\" )( reference.instance );\n";
    os << "                    break;\n";
            }
        }
    }
    
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    
    os << "    virtual void doRange( eg::EGRangeDescriptionPtr pRange )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            pStack->m_result = pybind11::make_iterator( eg::PythonIterator( *g_pEGRefType, pRange, false ), eg::PythonIterator( *g_pEGRefType, pRange, true ) );\n";
    
    os << "        }\n";
    os << "    }\n";
    
    
    
    os << "    //HostEvaluator\n";
    os << "    virtual void getIdentities( std::vector< const char* >& identities )\n";
    os << "    {\n";
    os << "        m_pRuntime->getIdentities( identities );\n";
    os << "    }\n";
    os << "    virtual " << eg::EG_TYPE_ID << " getTypeID( const char* pszIdentity )\n";
    os << "    {\n";
    os << "        return m_pRuntime->getTypeID( pszIdentity );\n";
    os << "    }\n";
    os << "    virtual PyObject* invoke( const " << eg::EG_REFERENCE_TYPE << "& reference, const std::vector< " << eg::EG_TYPE_ID << " >& typePath, PyObject *args, PyObject *kwargs )\n";
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
    os << "        else\n";
    os << "        {\n";
    os << "            Py_INCREF( Py_None );\n";
    os << "            return Py_None;\n";
    os << "        }\n";
    os << "    }\n";
    os << "private:\n";
    os << "    pybind11::module m_module_eg;\n";
    os << "    Stack::WeakPtr m_pStack;\n";
    os << "    std::shared_ptr< eg::EGRuntime > m_pRuntime;\n";
    os << "};\n";
    os << "\n";
    
    os << "void runPython( const std::string& strDatabaseFile, const std::string& strScript )\n";
    os << "{\n";
    os << "    pybind11::scoped_interpreter guard{}; // start the interpreter and keep it alive\n";
    os << "\n";
    os << "    try\n";
    os << "    {\n";
    os << "        pybind11::module pyeg_module = pybind11::module::import( \"pyeg\" );\n";
    os << "\n";
    os << "        HostFunctions hostFunctions( strDatabaseFile, pyeg_module );\n";
    os << "\n";
    os << "        g_pEGRefType = std::make_shared< eg::PythonEGReferenceType >( hostFunctions );\n";
    os << "\n";
    os << "        pybind11::exec( strScript );\n";
    os << "    }\n";
    os << "    catch( std::exception& e )\n";
    os << "    {\n";
    os << "        std::cout << e.what() << std::endl;\n";
    os << "    }\n";
    os << "}\n";
    os << "\n";
}