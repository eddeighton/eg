#include "structures.hpp"
static std::unique_ptr< pybind11::scoped_interpreter > g_embedded_python_guard;
//Action functions
extern void root_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Line< void > root_Line_starter( eg::Instance _gid );
extern void root_Line_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Line< void >::__eg_Move< void > root_Line_Move_starter( eg::Instance _gid );
extern void root_Line_Move_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Line< void >::__eg_Up< void > root_Line_Up_starter( eg::Instance _gid );
extern void root_Line_Up_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Line< void >::__eg_Down< void > root_Line_Down_starter( eg::Instance _gid );
extern void root_Line_Down_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Line< void >::__eg_Left< void > root_Line_Left_starter( eg::Instance _gid );
extern void root_Line_Left_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Line< void >::__eg_Right< void > root_Line_Right_starter( eg::Instance _gid );
extern void root_Line_Right_stopper( eg::Instance _gid );
extern __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > root_Line_Sequencer_starter( eg::Instance _gid );
extern void root_Line_Sequencer_stopper( eg::Instance _gid );
extern __eg_root< void > get_root();



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
    HostFunctions( const std::string& strDatabaseFile, pybind11::module module_eg );
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
    pybind11::module m_module_eg;
    Stack::WeakPtr m_pStack;
    std::shared_ptr< eg::EGRuntime > m_pRuntime;
};



    
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

    PythonEGReferenceType( HostFunctions& evaluator );
    
    PyObject* create( reference ref );
    
    HostFunctions& getEvaluator() const { return m_evaluator; }
private:
    HostFunctions& m_evaluator;
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

PythonEGReferenceType::PythonEGReferenceType( HostFunctions& evaluator )
    :   m_evaluator( evaluator )
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
                    ( ::getStopCycle( m_pRange->getType( m_subRange ), m_position ) == clock::cycle() ) )
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

//Python Interface Converters
std::shared_ptr< eg::PythonEGReferenceType > g_pEGRefType;
namespace pybind11
{
    namespace detail
    {
        template <> struct type_caster< __eg_root< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Line< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Line< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Line< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Line< void >::__eg_Move< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Line< void >::__eg_Move< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Line< void >::__eg_Move< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Line< void >::__eg_Up< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Line< void >::__eg_Up< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Line< void >::__eg_Up< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Line< void >::__eg_Down< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Line< void >::__eg_Down< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Line< void >::__eg_Down< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Line< void >::__eg_Left< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Line< void >::__eg_Left< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Line< void >::__eg_Left< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Line< void >::__eg_Right< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Line< void >::__eg_Right< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Line< void >::__eg_Right< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
        template <> struct type_caster< __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > >
        {
        public:
            PYBIND11_TYPE_CASTER( __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >, _("pyeg.reference"));
        
            bool load( handle src, bool )
            {
                const eg::PythonEGReference* pEGReference =
                    eg::PythonEGReferenceType::getReference( src.ptr() );
                value.data = pEGReference->getEGReference();
                return !PyErr_Occurred();
            }
        
            static handle cast( __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > src, return_value_policy /* policy */, handle /* parent */)
            {
                return g_pEGRefType->create( src.data );
            }
        };
    }   //namespace detail
} // namespace pybind11

//Python Interface
void root_pause( eg::Instance instance )
{
    if( g_root[ instance ].g_root_state == ::eg::action_running )
        g_root[ instance ].g_root_state = ::eg::action_paused;
}
void root_resume( eg::Instance instance )
{
    if( g_root[ instance ].g_root_state == ::eg::action_paused )
        g_root[ instance ].g_root_state = ::eg::action_running;
}
bool root_done( eg::Instance instance )
{
    eg::Iterator iter;
    iter = eg::Iterator( g_root[ instance ].g_rootLine_ring_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Line_pause( eg::Instance instance )
{
    if( g_root_Line[ instance ].g_root_Line_state == ::eg::action_running )
        g_root_Line[ instance ].g_root_Line_state = ::eg::action_paused;
}
void root_Line_resume( eg::Instance instance )
{
    if( g_root_Line[ instance ].g_root_Line_state == ::eg::action_paused )
        g_root_Line[ instance ].g_root_Line_state = ::eg::action_running;
}
bool root_Line_done( eg::Instance instance )
{
    eg::Iterator iter;
    iter = eg::Iterator( g_root_Line[ instance ].g_root_LineMove_ring_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Line[ instance ].g_root_LineUp_ring_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Line[ instance ].g_root_LineDown_ring_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Line[ instance ].g_root_LineLeft_ring_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Line[ instance ].g_root_LineRight_ring_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    iter = eg::Iterator( g_root_Line[ instance ].g_root_LineSequencer_ring_iter.load() );
    if( iter.full || ( iter.head != iter.tail ) ) return false;
    return true;
}
void root_Line_Move_pause( eg::Instance instance )
{
    if( g_root_Line_Move[ instance ].g_root_Line_Move_state == ::eg::action_running )
        g_root_Line_Move[ instance ].g_root_Line_Move_state = ::eg::action_paused;
}
void root_Line_Move_resume( eg::Instance instance )
{
    if( g_root_Line_Move[ instance ].g_root_Line_Move_state == ::eg::action_paused )
        g_root_Line_Move[ instance ].g_root_Line_Move_state = ::eg::action_running;
}
bool root_Line_Move_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Line_Up_pause( eg::Instance instance )
{
    if( g_root_Line_Up[ instance ].g_root_Line_Up_state == ::eg::action_running )
        g_root_Line_Up[ instance ].g_root_Line_Up_state = ::eg::action_paused;
}
void root_Line_Up_resume( eg::Instance instance )
{
    if( g_root_Line_Up[ instance ].g_root_Line_Up_state == ::eg::action_paused )
        g_root_Line_Up[ instance ].g_root_Line_Up_state = ::eg::action_running;
}
bool root_Line_Up_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Line_Down_pause( eg::Instance instance )
{
    if( g_root_Line_Down[ instance ].g_root_Line_Down_state == ::eg::action_running )
        g_root_Line_Down[ instance ].g_root_Line_Down_state = ::eg::action_paused;
}
void root_Line_Down_resume( eg::Instance instance )
{
    if( g_root_Line_Down[ instance ].g_root_Line_Down_state == ::eg::action_paused )
        g_root_Line_Down[ instance ].g_root_Line_Down_state = ::eg::action_running;
}
bool root_Line_Down_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Line_Left_pause( eg::Instance instance )
{
    if( g_root_Line_Left[ instance ].g_root_Line_Left_state == ::eg::action_running )
        g_root_Line_Left[ instance ].g_root_Line_Left_state = ::eg::action_paused;
}
void root_Line_Left_resume( eg::Instance instance )
{
    if( g_root_Line_Left[ instance ].g_root_Line_Left_state == ::eg::action_paused )
        g_root_Line_Left[ instance ].g_root_Line_Left_state = ::eg::action_running;
}
bool root_Line_Left_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Line_Right_pause( eg::Instance instance )
{
    if( g_root_Line_Right[ instance ].g_root_Line_Right_state == ::eg::action_running )
        g_root_Line_Right[ instance ].g_root_Line_Right_state = ::eg::action_paused;
}
void root_Line_Right_resume( eg::Instance instance )
{
    if( g_root_Line_Right[ instance ].g_root_Line_Right_state == ::eg::action_paused )
        g_root_Line_Right[ instance ].g_root_Line_Right_state = ::eg::action_running;
}
bool root_Line_Right_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}
void root_Line_Sequencer_pause( eg::Instance instance )
{
    if( g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_state == ::eg::action_running )
        g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_state = ::eg::action_paused;
}
void root_Line_Sequencer_resume( eg::Instance instance )
{
    if( g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_state == ::eg::action_paused )
        g_root_Line_Sequencer[ instance ].g_root_Line_Sequencer_state = ::eg::action_running;
}
bool root_Line_Sequencer_done( eg::Instance instance )
{
    eg::Iterator iter;
    return true;
}


void python_sleep_cycle()
{
    eg::sleep();
}
    

//Embedded Python Module
PYBIND11_EMBEDDED_MODULE( pyeg, module ) 
{
    module.def( "root", get_root );
    module.def( "sleep", python_sleep_cycle );
    module.def( "wait", eg::wait );
}

HostFunctions::HostFunctions( const std::string& strDatabaseFile, pybind11::module module_eg )
    :   m_pRuntime( std::shared_ptr< eg::EGRuntime >( 
            eg::constructRuntime( *this, strDatabaseFile.c_str() ) ) ),
        m_module_eg( module_eg )
{
}
//HostFunctionAccessor
eg::reference HostFunctions::dereferenceDimension( const eg::reference& action, const eg::TypeID& dimensionType )
{
    throw std::runtime_error( "Unreachable" );
}
void HostFunctions::doRead( const eg::reference& reference, eg::TypeID dimensionType )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        switch( dimensionType )
        {
            case 80:
                pStack->m_result = pybind11::cast( g_root[ reference.instance ].m_screen );
                break;
            case 119:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_start );
                break;
            case 121:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_end );
                break;
            case 123:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_colour );
                break;
            case 125:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_width );
                break;
            case 127:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_Distance );
                break;
            case 129:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_Speed );
                break;
            case 131:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_Sequence );
                break;
            case 134:
                pStack->m_result = pybind11::cast( g_root_Line_Move[ reference.instance ].m_dir );
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doWrite( const eg::reference& reference, eg::TypeID dimensionType )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
        switch( dimensionType )
        {
            case 80:
                 g_root[ reference.instance ].m_screen = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
                 break;
            case 119:
                 g_root_Line[ reference.instance ].m_start = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
                 break;
            case 121:
                 g_root_Line[ reference.instance ].m_end = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
                 break;
            case 123:
                 g_root_Line[ reference.instance ].m_colour = pybind11::cast< struct glm::tvec3<float, glm::highp> >( args[ 0 ] );
                 break;
            case 125:
                 g_root_Line[ reference.instance ].m_width = pybind11::cast< float >( args[ 0 ] );
                 break;
            case 127:
                 g_root_Line[ reference.instance ].m_Distance = pybind11::cast< float >( args[ 0 ] );
                 break;
            case 129:
                 g_root_Line[ reference.instance ].m_Speed = pybind11::cast< float >( args[ 0 ] );
                 break;
            case 131:
                 g_root_Line[ reference.instance ].m_Sequence = pybind11::cast< class std::vector<int, class std::allocator<int> > >( args[ 0 ] );
                 break;
            case 134:
                 g_root_Line_Move[ reference.instance ].m_dir = pybind11::cast< struct glm::tvec2<float, glm::highp> >( args[ 0 ] );
                 break;
            default:
                break;
        }
    }
}
void HostFunctions::doCall( const eg::reference& reference, eg::TypeID actionType )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
        switch( actionType )
        {
            case 74:
                {
                    __eg_root< void >::__eg_Line< void > ref = root_Line_starter( reference.instance );
                    if( ref )
                    {
                            ref();
                        root_Line_stopper( ref.data.instance );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 83:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Move< void > ref = root_Line_Move_starter( reference.instance );
                    if( ref )
                    {
                            ref();
                        root_Line_Move_stopper( ref.data.instance );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 89:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Up< void > ref = root_Line_Up_starter( reference.instance );
                    if( ref )
                    {
                            ref();
                        root_Line_Up_stopper( ref.data.instance );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 95:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Down< void > ref = root_Line_Down_starter( reference.instance );
                    if( ref )
                    {
                            ref();
                        root_Line_Down_stopper( ref.data.instance );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 101:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Left< void > ref = root_Line_Left_starter( reference.instance );
                    if( ref )
                    {
                            ref();
                        root_Line_Left_stopper( ref.data.instance );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 107:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Right< void > ref = root_Line_Right_starter( reference.instance );
                    if( ref )
                    {
                            ref();
                        root_Line_Right_stopper( ref.data.instance );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 113:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > ref = root_Line_Sequencer_starter( reference.instance );
                    if( ref )
                    {
                            ref();
                        root_Line_Sequencer_stopper( ref.data.instance );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doStart( const eg::reference& reference, eg::TypeID actionType )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        pybind11::args args = pybind11::reinterpret_borrow< pybind11::args >( pStack->args );
        switch( actionType )
        {
            case 74:
                {
                    __eg_root< void >::__eg_Line< void > ref = root_Line_starter( reference.instance );
                    if( ref )
                    {
                        std::function< void() > functor = std::bind( &__eg_root< void >::__eg_Line< void >::operator(), ref);
                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                        (
                            std::allocator_arg,
                            boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                            [ functor, ref ]()
                            {
                                try
                                {
                                    functor();
                                }
                                catch( eg::termination_exception )
                                {
                                }
                                root_Line_stopper( ref.data.instance );
                            }
                        );
                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 83:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Move< void > ref = root_Line_Move_starter( reference.instance );
                    if( ref )
                    {
                        std::function< void() > functor = std::bind( &__eg_root< void >::__eg_Line< void >::__eg_Move< void >::operator(), ref);
                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                        (
                            std::allocator_arg,
                            boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                            [ functor, ref ]()
                            {
                                try
                                {
                                    functor();
                                }
                                catch( eg::termination_exception )
                                {
                                }
                                root_Line_Move_stopper( ref.data.instance );
                            }
                        );
                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 89:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Up< void > ref = root_Line_Up_starter( reference.instance );
                    if( ref )
                    {
                        std::function< void() > functor = std::bind( &__eg_root< void >::__eg_Line< void >::__eg_Up< void >::operator(), ref);
                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                        (
                            std::allocator_arg,
                            boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                            [ functor, ref ]()
                            {
                                try
                                {
                                    functor();
                                }
                                catch( eg::termination_exception )
                                {
                                }
                                root_Line_Up_stopper( ref.data.instance );
                            }
                        );
                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 95:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Down< void > ref = root_Line_Down_starter( reference.instance );
                    if( ref )
                    {
                        std::function< void() > functor = std::bind( &__eg_root< void >::__eg_Line< void >::__eg_Down< void >::operator(), ref);
                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                        (
                            std::allocator_arg,
                            boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                            [ functor, ref ]()
                            {
                                try
                                {
                                    functor();
                                }
                                catch( eg::termination_exception )
                                {
                                }
                                root_Line_Down_stopper( ref.data.instance );
                            }
                        );
                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 101:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Left< void > ref = root_Line_Left_starter( reference.instance );
                    if( ref )
                    {
                        std::function< void() > functor = std::bind( &__eg_root< void >::__eg_Line< void >::__eg_Left< void >::operator(), ref);
                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                        (
                            std::allocator_arg,
                            boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                            [ functor, ref ]()
                            {
                                try
                                {
                                    functor();
                                }
                                catch( eg::termination_exception )
                                {
                                }
                                root_Line_Left_stopper( ref.data.instance );
                            }
                        );
                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 107:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Right< void > ref = root_Line_Right_starter( reference.instance );
                    if( ref )
                    {
                        std::function< void() > functor = std::bind( &__eg_root< void >::__eg_Line< void >::__eg_Right< void >::operator(), ref);
                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                        (
                            std::allocator_arg,
                            boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                            [ functor, ref ]()
                            {
                                try
                                {
                                    functor();
                                }
                                catch( eg::termination_exception )
                                {
                                }
                                root_Line_Right_stopper( ref.data.instance );
                            }
                        );
                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            case 113:
                {
                    __eg_root< void >::__eg_Line< void >::__eg_Sequencer< void > ref = root_Line_Sequencer_starter( reference.instance );
                    if( ref )
                    {
                        std::function< void() > functor = std::bind( &__eg_root< void >::__eg_Line< void >::__eg_Sequencer< void >::operator(), ref);
                        getFiber( ref.data.type, ref.data.instance ) = boost::fibers::fiber
                        (
                            std::allocator_arg,
                            boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE ),
                            [ functor, ref ]()
                            {
                                try
                                {
                                    functor();
                                }
                                catch( eg::termination_exception )
                                {
                                }
                                root_Line_Sequencer_stopper( ref.data.instance );
                            }
                        );
                        getFiber( ref.data.type, ref.data.instance ).properties< eg::fiber_props >().setReference( ref.data );
                    }
                    pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( ref.data ) );
                }
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doStop( const eg::reference& reference )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        switch( reference.type )
        {
            case 67:
                root_stopper( reference.instance );
                break;
            case 74:
                root_Line_stopper( reference.instance );
                break;
            case 83:
                root_Line_Move_stopper( reference.instance );
                break;
            case 89:
                root_Line_Up_stopper( reference.instance );
                break;
            case 95:
                root_Line_Down_stopper( reference.instance );
                break;
            case 101:
                root_Line_Left_stopper( reference.instance );
                break;
            case 107:
                root_Line_Right_stopper( reference.instance );
                break;
            case 113:
                root_Line_Sequencer_stopper( reference.instance );
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doPause( const eg::reference& reference )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        switch( reference.type )
        {
            case 67:
                root_pause( reference.instance );
                break;
            case 74:
                root_Line_pause( reference.instance );
                break;
            case 83:
                root_Line_Move_pause( reference.instance );
                break;
            case 89:
                root_Line_Up_pause( reference.instance );
                break;
            case 95:
                root_Line_Down_pause( reference.instance );
                break;
            case 101:
                root_Line_Left_pause( reference.instance );
                break;
            case 107:
                root_Line_Right_pause( reference.instance );
                break;
            case 113:
                root_Line_Sequencer_pause( reference.instance );
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doResume( const eg::reference& reference )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        switch( reference.type )
        {
            case 67:
                root_resume( reference.instance );
                break;
            case 74:
                root_Line_resume( reference.instance );
                break;
            case 83:
                root_Line_Move_resume( reference.instance );
                break;
            case 89:
                root_Line_Up_resume( reference.instance );
                break;
            case 95:
                root_Line_Down_resume( reference.instance );
                break;
            case 101:
                root_Line_Left_resume( reference.instance );
                break;
            case 107:
                root_Line_Right_resume( reference.instance );
                break;
            case 113:
                root_Line_Sequencer_resume( reference.instance );
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doDone( const eg::reference& reference )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        switch( reference.type )
        {
            case 67:
                pStack->m_result = pybind11::cast( root_done( reference.instance ) );
                break;
            case 74:
                pStack->m_result = pybind11::cast( root_Line_done( reference.instance ) );
                break;
            case 83:
                pStack->m_result = pybind11::cast( root_Line_Move_done( reference.instance ) );
                break;
            case 89:
                pStack->m_result = pybind11::cast( root_Line_Up_done( reference.instance ) );
                break;
            case 95:
                pStack->m_result = pybind11::cast( root_Line_Down_done( reference.instance ) );
                break;
            case 101:
                pStack->m_result = pybind11::cast( root_Line_Left_done( reference.instance ) );
                break;
            case 107:
                pStack->m_result = pybind11::cast( root_Line_Right_done( reference.instance ) );
                break;
            case 113:
                pStack->m_result = pybind11::cast( root_Line_Sequencer_done( reference.instance ) );
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doWaitAction( const eg::reference& reference )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( reference ) );
    }
}
void HostFunctions::doWaitDimension( const eg::reference& reference, eg::TypeID dimensionType )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        switch( dimensionType )
        {
            case 80:
                break;
            case 119:
                break;
            case 121:
                break;
            case 123:
                break;
            case 125:
                break;
            case 127:
                break;
            case 129:
                break;
            case 131:
                break;
            case 134:
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doGetAction( const eg::reference& reference )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        pStack->m_result = pybind11::reinterpret_borrow< pybind11::object >( g_pEGRefType->create( reference ) );
    }
}
void HostFunctions::doGetDimension( const eg::reference& reference, eg::TypeID dimensionType )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        switch( dimensionType )
        {
            case 80:
                pStack->m_result = pybind11::cast( g_root[ reference.instance ].m_screen );
                break;
            case 119:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_start );
                break;
            case 121:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_end );
                break;
            case 123:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_colour );
                break;
            case 125:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_width );
                break;
            case 127:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_Distance );
                break;
            case 129:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_Speed );
                break;
            case 131:
                pStack->m_result = pybind11::cast( g_root_Line[ reference.instance ].m_Sequence );
                break;
            case 134:
                pStack->m_result = pybind11::cast( g_root_Line_Move[ reference.instance ].m_dir );
                break;
            default:
                break;
        }
    }
}
void HostFunctions::doRange( eg::EGRangeDescriptionPtr pRange )
{
    if( Stack::SharedPtr pStack = m_pStack.lock() )
    {
        pStack->m_result = pybind11::make_iterator( eg::PythonIterator( *g_pEGRefType, pRange, false ), eg::PythonIterator( *g_pEGRefType, pRange, true ) );
    }
}
void HostFunctions::getIdentities( std::vector< const char* >& identities )
{
    m_pRuntime->getIdentities( identities );
}
eg::TypeID HostFunctions::getTypeID( const char* pszIdentity )
{
    return m_pRuntime->getTypeID( pszIdentity );
}
PyObject* HostFunctions::invoke( const eg::reference& reference, const std::vector< eg::TypeID >& typePath, PyObject *args, PyObject *kwargs )
{
    if( reference.type != 0 )
    {
        Stack::SharedPtr pStack = std::make_shared< Stack >( args, kwargs );
        m_pStack = pStack;
        pybind11::args pyArgs = pybind11::reinterpret_borrow< pybind11::args >( args );
        m_pRuntime->invoke( reference, typePath, pyArgs.size() != 0 );
        if( pStack->m_result )
        {
            pybind11::handle h = pStack->m_result;
            h.inc_ref();
            return h.ptr();
        }
    }
    else
    {
        throw std::runtime_error( "Invalid reference used in invocation" );
    }
    Py_INCREF( Py_None );
    return Py_None;
}



void runPythonScript( const std::string& strPythonFile, const std::string& strDatabaseFile )
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
            pybind11::module pyeg_module = pybind11::module::import( "pyeg" );

            HostFunctions hostFunctions( strDatabaseFile, pyeg_module );
            
            if( !g_pEGRefType )
                g_pEGRefType = std::make_shared< eg::PythonEGReferenceType >( hostFunctions );

            pybind11::exec( strScript );
        }
    }
    catch( std::exception& e )
    {
        std::cout << e.what() << std::endl;
    }
}

std::vector< std::function< void() > > loadPythonScripts( const std::vector< std::string >& scripts, const std::string& strDatabaseFile )
{
    std::vector< std::function< void() > > pythonFunctions;
    if( !scripts.empty() )
    {
        if( strDatabaseFile.empty() )
        {
            std::cout << "Missing database file path" << std::endl;
            return pythonFunctions;
        }
        const boost::filesystem::path databaseFilePath( strDatabaseFile );
        if( !boost::filesystem::exists( databaseFilePath ) )
        {
            std::cout << "Cannot locate file: " << databaseFilePath.string() << std::endl;
            return pythonFunctions;
        } 
        
        g_embedded_python_guard = std::make_unique< pybind11::scoped_interpreter >();
            
        for( const std::string& strPythonScript : scripts )
        {
            pythonFunctions.push_back( std::bind( &runPythonScript, strPythonScript, strDatabaseFile ) );
        }
    }
    return pythonFunctions;
}

