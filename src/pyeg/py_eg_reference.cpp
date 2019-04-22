
#include "py_eg_reference.hpp"

#include <sstream>

namespace eg
{

    PythonEGReference::PythonEGReference( PythonEGReferenceType& pythonType, const __eg_reference& ref ) 
        :   m_pythonType( pythonType ),
            m_reference( ref ) 
    {
        
    }
    
    PyObject* PythonEGReference::get( void* pClosure )
    {
        const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
        //convert pszAttributeIdentity to EGTypeID
        
        PyObject* pResult = m_pythonType.create( m_reference );
        {
            PythonEGReference* pNewRef = PythonEGReferenceType::getReference( pResult );
            pNewRef->m_type_path = m_type_path;
            pNewRef->m_type_path.push_back( 0 );
        }
        
        return pResult;
    }
    
    int PythonEGReference::set( void* pClosure, PyObject* pValue )
    {
        const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
        
        return 0;
    }
    
    PyObject* PythonEGReference::str() const
    {
        std::ostringstream os;
        os << "instance: " << m_reference.instance << " type: " << m_reference.type << " timestamp: " << m_reference.timestamp;
        for( std::vector< EGTypeID >::const_iterator 
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
        
        
        
        
        
        Py_INCREF( Py_None );
        return Py_None;
    }

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    typedef struct 
    {
        PyObject_HEAD
        PythonEGReference* pReference;
    }eg_reference_data;
    
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
    
    static char* psz_a = "a";
    static char* psz_b = "b";
    static char* psz_c = "c";
    
    PythonEGReferenceType::PythonEGReferenceType()
    {        
        //generate python array for attributes
        /*{
            for( Attribute::Ptr pAttribute : m_attributes )
            {
                const std::string& strName = pAttribute->getName();
                char* pszNonConstHack = const_cast< char* >( strName.c_str() );
                PyGetSetDef data =
                { 
                    pszNonConstHack, 
                    (getter)type_get, 
                    (setter)type_set, 
                    pszNonConstHack, 
                    (void*)pszNonConstHack
                };
                m_pythonAttributesData.push_back( data );
            }
            m_pythonAttributesData.push_back( PyGetSetDef{ NULL } );
        }*/
        
        
        PyGetSetDef data1 =
        { 
            psz_a, 
            (getter)type_get, 
            (setter)type_set, 
            psz_a, 
            (void*)psz_a
        };
        PyGetSetDef data2 =
        { 
            psz_b, 
            (getter)type_get, 
            (setter)type_set, 
            psz_b, 
            (void*)psz_b
        };
        PyGetSetDef data3 =
        { 
            psz_c, 
            (getter)type_get, 
            (setter)type_set, 
            psz_c, 
            (void*)psz_c
        };
        m_pythonAttributesData.push_back( data1 );
        m_pythonAttributesData.push_back( data2 );
        m_pythonAttributesData.push_back( data3 );
        m_pythonAttributesData.push_back( PyGetSetDef{ NULL } );
        
        
        //generate heap allocated python type...
        std::vector< PyType_Slot > slots;
        {
            slots.push_back( PyType_Slot{ Py_tp_str,        type_str } );
            slots.push_back( PyType_Slot{ Py_tp_repr,       type_str } );
            slots.push_back( PyType_Slot{ Py_tp_dealloc,    type_dealloc } );
            slots.push_back( PyType_Slot{ Py_tp_call,       type_call } );
            slots.push_back( PyType_Slot{ Py_tp_methods,    type_methods } );
            slots.push_back( PyType_Slot{ Py_tp_getset,     m_pythonAttributesData.data() } );
        }
        
        slots.push_back( PyType_Slot{ 0 } );
        
        PyType_Spec spec = 
        { 
            "eg.reference",
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
    
    PyObject* PythonEGReferenceType::create( __eg_reference egReference )
    {
        eg_reference_data* pRootObject = PyObject_New( eg_reference_data, m_pTypeObject );
        PyObject* pPythonObject = PyObject_Init( (PyObject*)pRootObject, m_pTypeObject );
        pRootObject->pReference = new PythonEGReference( *this, egReference );
        Py_INCREF( pPythonObject );
        return pPythonObject;
    }
    
}