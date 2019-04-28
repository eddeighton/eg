
#include "eg/implementation_session.hpp"
#include "eg/codegen.hpp"

#include <ostream>

void generate_python( std::ostream& os, eg::ReadSession& session )
{
    const eg::abstract::Root* pRoot = session.getTreeRoot();
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
    
    std::vector< const eg::abstract::Action* > abstractActions = 
        eg::many_cst< eg::abstract::Action >( objects );
        
    using ActionTypeMap = std::map< const eg::abstract::Action*, std::string >;
    ActionTypeMap actionTypeMap;
    
    for( const eg::abstract::Action* pAbstractAction : abstractActions )
    {
        if( pAbstractAction->getParent() )
        {
            std::ostringstream osType;
            {
                std::vector< const eg::abstract::Element* > path = eg::abstract::getPath( pAbstractAction );
                //generate type explicit template specialisation
                {
                    for( const eg::abstract::Element* pNodeIter : path )
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
        const eg::abstract::Action* pAbstractAction = i->first;
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
    
    
    os << "__interface_root< void > get_root()\n";
    os << "{\n";
    os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
    os << "    return  __interface_root< void >( 0, 0 );\n";
    os << "}\n";
    os << "\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
                pDimension->printType( os );
                os << " read_" << pBuffer->getVariableName() << '_' << pDataMember->getName() << "( " << eg::EG_INSTANCE << " instance )\n";
                os << "{\n";
                os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
                os << "    return " << eg::Printer( pDataMember, "instance" ) << ";\n";
                os << "}\n";
                os << "void write_" << pBuffer->getVariableName() << '_'<< pDataMember->getName() << "( " << eg::EG_INSTANCE << " instance, pybind11::tuple args )\n";
                os << "{\n";
                os << "    std::lock_guard< std::mutex > guard( *g_pSimulationMutex );\n";
                os << "    " << eg::Printer( pDataMember, "instance" ) << " = pybind11::cast< "; pDimension->printType( os ); os << " >( args[ 0 ] );\n";
                os << "}\n";
            }
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
    os << "    module.def( \"read_" << pBuffer->getVariableName() << '_'<< pDataMember->getName() << 
                            "\", read_" << pBuffer->getVariableName() << '_'<< pDataMember->getName() << " );\n";
    os << "    module.def( \"write_" << pBuffer->getVariableName() << '_'<< pDataMember->getName() << 
                            "\", write_" << pBuffer->getVariableName() << '_'<< pDataMember->getName() << " );\n";
            }
        }
    }
    
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getParent() )
        {
    os << "    module.def( \"" << pAction->getName() << "_stopper\", " << pAction->getName() << "_stopper );\n";
    os << "    module.def( \"" << pAction->getName() << "_starter\", " << pAction->getName() << "_starter );\n";
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
    os << "    virtual void doRead(    const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            switch( reference.type )\n";
    os << "            {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "                case " << pDimension->getDimension()->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"read_" << pBuffer->getVariableName() << '_' << pDataMember->getName()  << "\" )( reference.instance );\n";
    os << "                    break;\n";
            }
        }
    }
    
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n"; 
    os << "    virtual void doWrite(   const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "        if( Stack::SharedPtr pStack = m_pStack.lock() )\n";
    os << "        {\n";
    os << "            pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );\n";
    os << "            switch( reference.type )\n";
    os << "            {\n";
    
    for( const eg::Buffer* pBuffer : layout.getBuffers() )
    {
        for( const eg::DataMember* pDataMember : pBuffer->getDimensions() )
        {
            if( const eg::concrete::Dimension_User* pDimension = 
                dynamic_cast< const eg::concrete::Dimension_User* >( pDataMember->getInstanceDimension() ) )
            {
    os << "                case " << pDimension->getDimension()->getIndex() << ":\n";
    os << "                     pStack->m_result = m_module_eg.attr( \"write_" << pBuffer->getVariableName() << '_' << pDataMember->getName()  << "\" )( reference.instance, args );\n";
    os << "                     break;\n";
            }
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doStart(   const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
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
    os << "                case " << pAction->getAction()->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << pAction->getName() << "_starter\" )( reference.instance );\n";
    os << "                    break;\n";
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doStop(    const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
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
    os << "                case " << pAction->getAction()->getIndex() << ":\n";
    os << "                    pStack->m_result = m_module_eg.attr( \"" << pAction->getName() << "_stopper\" )( reference.instance );\n";
    os << "                    break;\n";
        }
    }
    os << "                default:\n";
    os << "                    break;\n";
    os << "            }\n";
    os << "        }\n";
    os << "    }\n";
    os << "    virtual void doPause(   const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "    }\n";
    os << "    virtual void doResume(  const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "    }\n";
    os << "    virtual void doEmpty(   const " << eg::EG_REFERENCE_TYPE << "& reference )\n";
    os << "    {\n";
    os << "    }\n";
    os << "    virtual eg::reference getReference( const " << eg::EG_REFERENCE_TYPE << "& dimension )\n";
    os << "    {\n";
    os << "        return dimension;\n";
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