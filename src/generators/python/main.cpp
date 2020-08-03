
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


#include "generators.hpp"

#include "eg_compiler/sessions/implementation_session.hpp"
#include "eg_compiler/codegen/codegen.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/program_options.hpp>

#include <iostream>
    
extern void generate_python( std::ostream& os, const eg::ReadSession& session );

int main( int argc, const char* argv[] )
{
    const eg::CmdLine cmdLine = eg::handleCommandLine( argc, argv );
    
    eg::ReadSession session( cmdLine.programDataBaseFile );
    
    std::ostringstream os;
    
    const eg::interface::Root* pRoot = session.getTreeRoot();
    
    const eg::DerivationAnalysis& derivationAnalysis = session.getDerivationAnalysis();
    const eg::Layout& layout = session.getLayout();
    const eg::IndexedObject::Array& objects = session.getObjects( eg::IndexedObject::MASTER_FILE );
    
    os << "#include \"structures.hpp\"\n";
   
    os << "//Action functions\n";
    std::vector< const eg::concrete::Action* > actions = 
        eg::many_cst< eg::concrete::Action >( objects );
    for( const eg::concrete::Action* pAction : actions )
    {
        if( pAction->getContext()->isExecutable() && !pAction->getContext()->isMainExecutable() )
        {
			VERIFY_RTE( pAction->getParent() && pAction->getParent()->getParent() );
    os << "extern " << getStaticType( pAction->getContext() ) << " " << pAction->getName() << "_starter( " << eg::EG_INSTANCE << " _gid );\n";
        }
        if( pAction->getContext()->isExecutable() )
        {
    os << "extern void " << pAction->getName() << "_stopper( " << eg::EG_INSTANCE << " _gid );\n";
        }
    }
    
    os << "extern " << eg::getInterfaceType( eg::input::Root::RootTypeName ) << "< void > get_root();\n";

const char* pszHostFunctions = R"(


struct HostFunctions : public eg::HostFunctionAccessor
{
private:
    struct Stack
    {
        PyObject *args;
        PyObject *kwargs;
        pybind11::object m_result;
        Stack( PyObject *args, PyObject *kwargs )
            :   args( args ), kwargs( kwargs )
        {
            Py_INCREF( args );
        }
        ~Stack()
        {
            Py_DECREF( args );
        }
        using WeakPtr = std::weak_ptr< Stack >;
        using SharedPtr = std::shared_ptr< Stack >;
    };
public:
    HostFunctions( const std::string& strDatabaseFile );
    //HostFunctionAccessor
    virtual eg::reference dereferenceDimension( const eg::reference& action, const eg::TypeID& dimensionType );
    virtual void doRead( const eg::reference& reference, eg::TypeID dimensionType );
    virtual void doWrite( const eg::reference& reference, eg::TypeID dimensionType );
    virtual void doCall( const eg::reference& reference, eg::TypeID actionType );
    virtual void doStart( const eg::reference& reference, eg::TypeID actionType );
    virtual void doStop( const eg::reference& reference );
    virtual void doPause( const eg::reference& reference );
    virtual void doResume( const eg::reference& reference );
    virtual void doDone( const eg::reference& reference );
    virtual void doWaitAction( const eg::reference& reference );
    virtual void doWaitDimension( const eg::reference& reference, eg::TypeID dimensionType );
    virtual void doGetAction( const eg::reference& reference );
    virtual void doGetDimension( const eg::reference& reference, eg::TypeID dimensionType );
    virtual void doRange( eg::EGRangeDescriptionPtr pRange );
    
    void getIdentities( std::vector< const char* >& identities );
    eg::TypeID getTypeID( const char* pszIdentity );
    PyObject* invoke( const eg::reference& reference, const std::vector< eg::TypeID >& typePath, PyObject *args, PyObject *kwargs );
private:
    Stack::WeakPtr m_pStack;
    std::shared_ptr< eg::EGRuntime > m_pRuntime;
};


)";

    os << pszHostFunctions;

    const char pszPythonRef[] = R"(
    
//requires the dynamic interface
extern eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance );
extern eg::ActionState getState( eg::TypeID typeID, eg::Instance instance );
extern eg::TimeStamp getStopCycle( eg::TypeID typeID, eg::Instance instance );

namespace eg
{
    
class PythonEGReferenceType;

class PythonEGReference
{
public:
    PythonEGReference( PythonEGReferenceType& pythonType, const reference& ref ) 
        :   m_pythonType( pythonType ),
            m_reference( ref ) 
    {
    }
    
    PyObject* get( void* pClosure );
    
    int set( void* pClosure, PyObject* pValue );
    
    PyObject* str() const;
    
    PyObject* call( PyObject *args, PyObject *kwargs );
    
    const reference getEGReference() const { return m_reference; }
private:
    PythonEGReferenceType& m_pythonType;
    reference m_reference;
    std::vector< TypeID > m_type_path;
};

typedef struct 
{
    PyObject_HEAD
    PythonEGReference* pReference;
}eg_reference_data;

class PythonEGReferenceType
{
public:
    static PythonEGReference* getReference( PyObject* pPyObject );

    PythonEGReferenceType( const std::string& strDatabaseFile );
    
    PyObject* create( reference ref );
    
    HostFunctions& getEvaluator() { return m_evaluator; }
private:
    HostFunctions m_evaluator;
    PyTypeObject* m_pTypeObject;
    std::vector< PyGetSetDef > m_pythonAttributesData;
    std::vector< const char* > m_identities;
};

PythonEGReference* PythonEGReferenceType::getReference( PyObject* pPyObject )
{
    eg_reference_data* pLogicalObject = (eg_reference_data*)pPyObject;
    return pLogicalObject->pReference;
}

static void type_dealloc( PyObject* pPyObject )
{
    PythonEGReference* pReference = PythonEGReferenceType::getReference( pPyObject );
    delete pReference;
    Py_TYPE( pPyObject )->tp_free( (PyObject*)pPyObject );
}

static PyObject* type_get( PyObject* self, void* pClosure )
{
    if( PythonEGReference* pRef = PythonEGReferenceType::getReference( self ) )
    {
        return pRef->get( pClosure );
    }
    else
    {
        //PYTHON_ERROR( "PythonEGReferenceType is out of date" );
        return 0;
    }
}

static int type_set( PyObject* self, PyObject* pValue, void* pClosure )
{
    if( PythonEGReference* pRef = PythonEGReferenceType::getReference( self ) )
    {
        return pRef->set( pClosure, pValue );
    }
    else
    {
        //PYTHON_ERROR( "PythonEGReferenceType is out of date" );
        return -1;
    }
}

PyObject* type_str( PyObject* self )
{
    if( PythonEGReference* pRef = PythonEGReferenceType::getReference( self ) )
    {
        return pRef->str();
    }
    else
    {
        //PYTHON_ERROR( "PythonEGReferenceType is out of date" );
        return NULL;
    }
}

PyObject* type_call( PyObject *callable, PyObject *args, PyObject *kwargs )
{
    if( PythonEGReference* pRef = PythonEGReferenceType::getReference( callable ) )
    {
        return pRef->call( args, kwargs );
    }
    else
    {
        //PYTHON_ERROR( "PythonEGReferenceType is out of date" );
        return NULL;
    }
}
    
static PyMethodDef type_methods[] = 
{
    //{"sample", (PyCFunction)type_sample, METH_VARARGS, "Sample the object and its subtree" },
    //{"assign", (PyCFunction)type_assign, METH_VARARGS, "Assign the object to a sample" },
    //{"update", (PyCFunction)type_update, METH_NOARGS, "Update objects associated gpu buffers" },
    {NULL}  /* Sentinel */
};

PythonEGReferenceType::PythonEGReferenceType( const std::string& strDatabaseFile )
    :   m_evaluator( strDatabaseFile )
{        
    m_evaluator.getIdentities( m_identities );
    for( const char* pszIdentity : m_identities )
    {
        char* pszNonConst = const_cast< char* >( pszIdentity );
        PyGetSetDef data =
        { 
            pszNonConst, 
            (getter)type_get, 
            (setter)type_set, 
            pszNonConst, 
            (void*)pszNonConst
        };
        m_pythonAttributesData.push_back( data );
    }
    m_pythonAttributesData.push_back( PyGetSetDef{ NULL } );
    
    //generate heap allocated python type...
    std::vector< PyType_Slot > slots;
    {
        slots.push_back( PyType_Slot{ Py_tp_str,        reinterpret_cast< void*>( &type_str ) } );
        slots.push_back( PyType_Slot{ Py_tp_repr,       reinterpret_cast< void*>( &type_str ) } );
        slots.push_back( PyType_Slot{ Py_tp_dealloc,    reinterpret_cast< void*>( &type_dealloc ) } );
        slots.push_back( PyType_Slot{ Py_tp_call,       reinterpret_cast< void*>( &type_call ) } );
        slots.push_back( PyType_Slot{ Py_tp_methods,    reinterpret_cast< void*>( &type_methods ) } );
        slots.push_back( PyType_Slot{ Py_tp_getset,     m_pythonAttributesData.data() } );
    }
    
    slots.push_back( PyType_Slot{ 0 } );
    
    PyType_Spec spec = 
    { 
        "pyeg.reference",
        sizeof( eg_reference_data ), 
        0, 
        Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, 
        slots.data() 
    };
    
    m_pTypeObject = (PyTypeObject*)PyType_FromSpec( &spec );        
    
    if( PyType_Ready( m_pTypeObject ) < 0 )
    {
        //set exception
    }
    else
    {
        Py_INCREF( m_pTypeObject );
        //successfully generated the dynamic type...
        //PyModule_AddObject( pPythonModule, "Host", (PyObject*)&m_type );
    }
}

PyObject* PythonEGReferenceType::create( reference egReference )
{
    eg_reference_data* pRootObject = PyObject_New( eg_reference_data, m_pTypeObject );
    PyObject* pPythonObject = PyObject_Init( (PyObject*)pRootObject, m_pTypeObject );
    pRootObject->pReference = new PythonEGReference( *this, egReference );
    Py_INCREF( pPythonObject );
    return pPythonObject;
}

    
PyObject* PythonEGReference::get( void* pClosure )
{
    const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    const TypeID typeID = m_pythonType.getEvaluator().getTypeID( pszAttributeIdentity );
    if( typeID == 0 )
    {
        std::ostringstream os;
        os << "Invalid identity" << pszAttributeIdentity;
        throw std::runtime_error( os.str() );
        
        //Py_INCREF( Py_None );
        //return Py_None;
    }
    else
    {
        PyObject* pResult = m_pythonType.create( m_reference );
        {
            PythonEGReference* pNewRef = PythonEGReferenceType::getReference( pResult );
            pNewRef->m_type_path.reserve( m_type_path.size() + 1U );
            pNewRef->m_type_path = m_type_path;
            pNewRef->m_type_path.push_back( typeID );
        }
        
        return pResult;
    }
}

int PythonEGReference::set( void* pClosure, PyObject* pValue )
{
    //const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
    return 0;
}

PyObject* PythonEGReference::str() const
{
    std::ostringstream os;
    os << "instance: " << m_reference.instance << " type: " << m_reference.type << " timestamp: " << m_reference.timestamp;
    for( std::vector< TypeID >::const_iterator 
        i = m_type_path.begin(), iEnd = m_type_path.end(); i!=iEnd; ++i )
    {
        if( i == m_type_path.begin() )
        {
            os << " type path: ";
        }
        os << *i << " ";
    }
    return Py_BuildValue( "s", os.str().c_str() );
}

PyObject* PythonEGReference::call( PyObject *args, PyObject *kwargs )
{
    return m_pythonType.getEvaluator().invoke( m_reference, m_type_path, args, kwargs );
}

class PythonIterator : public std::iterator< std::forward_iterator_tag, pybind11::object >
{
    PythonEGReferenceType& m_pythonType;
    EGRangeDescriptionPtr m_pRange;
    Instance m_position, m_subRange;
public:
    inline PythonIterator( PythonEGReferenceType& pythonType, EGRangeDescriptionPtr pRange, bool bEnd ) 
        :   m_pythonType( pythonType ), 
            m_pRange( pRange ),
            m_position( 0U ),
            m_subRange( 0U )
    {
        if( bEnd )
        {
            m_subRange = m_pRange->getSize();
        }
        else if( m_subRange != m_pRange->getSize() )
        {
            m_position = m_pRange->getBegin( m_subRange );
            scanToNextOrEnd();
        }
    }
    
    inline PythonIterator( const PythonIterator& from ) : 
        m_pythonType( from.m_pythonType ), 
        m_pRange( from.m_pRange ),
        m_position( from.m_position ),
        m_subRange( from.m_subRange )
    {
    }
    
    inline void scanToNextOrEnd()
    {
        while( true )
        {
            //if at end of current range and remaining subranges increment subrange
            while( ( m_subRange != m_pRange->getSize() ) && ( m_position == m_pRange->getEnd( m_subRange ) ) )
            {
                ++m_subRange;
                if( m_subRange == m_pRange->getSize() )
                    break;
                m_position = m_pRange->getBegin( m_subRange );
            }
            
            //at end of all sub ranges then exit
            if( m_subRange == m_pRange->getSize() )
                break;
            
            //now actually see if the current position is valid
            if( !m_pRange->raw() )
            {
                if( ( ::getState( m_pRange->getType( m_subRange ), m_position ) != action_stopped ) || 
                    ( ::getStopCycle( m_pRange->getType( m_subRange ), m_position ) == clock::cycle( TODO ) ) )
                    break;
            }
            else
            {
                break;
            }
            
            //skip to next and continue scanning
            ++m_position;
        }
    }
    
    inline PythonIterator& operator++()
    {
        //ASSERT( m_subRange != m_pRange->getSize() );
        ++m_position;
        scanToNextOrEnd();
        return *this;
    }
    
    inline PythonIterator operator++(int) 
    {
        PythonIterator tmp( *this ); 
        operator++(); 
        return tmp;
    }
    
    inline bool operator==(const PythonIterator& rhs) const 
    {
        if( ( m_pRange == rhs.m_pRange ) && ( m_subRange == rhs.m_subRange ) )
        {
            if( m_subRange == m_pRange->getSize() )
                return true;
            else
                return m_position == rhs.m_position;
        }
        return false;
    }
    
    inline bool operator!=(const PythonIterator& rhs) const { return !(rhs==*this); }
    
    inline pybind11::object operator*()
    {
        if( m_subRange == m_pRange->getSize() )
        {
            Py_INCREF( Py_None );
            return pybind11::reinterpret_borrow< pybind11::object >( Py_None );
        }
        else
        {
            const TypeID typeID = m_pRange->getType( m_subRange );
            ::eg::reference ref{ m_position, typeID, getTimestamp( typeID, m_position ) };
            return pybind11::reinterpret_borrow< pybind11::object >( m_pythonType.create( ref ) );
        }
    }
};

}
)";

    os << pszPythonRef;

    generate_python( os, session );
    
const char* pszPythonRun = R"(

void runPythonScript( const std::string& strPythonFile )
{
    try
    {
        std::string strScript;
        {
            const boost::filesystem::path pythonFilePath( strPythonFile );
            if( !boost::filesystem::exists( pythonFilePath ) )
            {
                std::cout << "Cannot locate file: " << pythonFilePath.string() << std::endl;
                return;
            } 
            std::ifstream inputFileStream( pythonFilePath.native().c_str(), std::ios::in );
            strScript = std::move( std::string( (std::istreambuf_iterator<char>( inputFileStream )),
                                       std::istreambuf_iterator<char>() ) );
        }

        if( !strScript.empty() )
        {
            pybind11::exec( strScript );
        }
    }
    catch( std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }
}

static std::unique_ptr< pybind11::scoped_interpreter > g_embedded_python_guard;

void initialisePythonBindings( const std::string& strDatabaseFile )
{
    if( strDatabaseFile.empty() )
    {
        ERR( "Missing database file path for python bindings" );
        throw std::runtime_error( "Missing database file path for python bindings" );
    }
    const boost::filesystem::path databaseFilePath( strDatabaseFile );
    if( !boost::filesystem::exists( databaseFilePath ) )
    {
        ERR( "Cannot locate file: " << databaseFilePath.string() );
        throw std::runtime_error( "Missing database file path for python bindings" );
    } 
    
    g_embedded_python_guard = std::make_unique< pybind11::scoped_interpreter >();
    
    g_pEGRefType = std::make_shared< eg::PythonEGReferenceType >( strDatabaseFile );
}

namespace eg
{
    pybind11::dict createPythonContext( eg::Event ev )
    {
        using namespace pybind11::literals;
        return pybind11::dict( "this"_a=ev, "root"_a=get_root() );
    }
}

std::vector< std::function< void() > > loadPythonScripts( const std::vector< std::string >& scripts, const std::string& strDatabaseFile )
{
    initialisePythonBindings( strDatabaseFile );
    
    std::vector< std::function< void() > > pythonFunctions;
    if( !scripts.empty() )
    {
        for( const std::string& strPythonScript : scripts )
        {
            pythonFunctions.push_back( std::bind( &runPythonScript, strPythonScript ) );
        }
    }
    return pythonFunctions;
}

)";
    os << pszPythonRun;
	
	boost::filesystem::path targetFilePath;
	if( cmdLine.targetFileName.empty() )
    {
        targetFilePath = cmdLine.strBuildDir / std::string( "python_bindings.cpp" );
    }
    else
    {
        targetFilePath = cmdLine.strBuildDir / cmdLine.targetFileName;
    }
    
    boost::filesystem::updateFileIfChanged( targetFilePath, os.str() );
    
    return 0;
}