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


#include "py_eg_reference.hpp"

#include "eg_runtime/eg_runtime.hpp"

#include "common/assert_verify.hpp"

#include <sstream>

namespace eg
{
    
    HostEvaluator::~HostEvaluator()
    {
        
    }
    

    /////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////
    PythonEGReference::PythonEGReference( PythonEGReferenceType& pythonType, const reference& ref ) 
        :   m_pythonType( pythonType ),
            m_reference( ref ) 
    {
        
    }
    
    PyObject* PythonEGReference::get( void* pClosure )
    {
        const char* pszAttributeIdentity = reinterpret_cast< char* >( pClosure );
        const TypeID typeID = m_pythonType.getEvaluator().getTypeID( pszAttributeIdentity );
        if( typeID == 0 )
        {
            THROW_RTE( "Invalid identity" << pszAttributeIdentity );
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
    
    PythonEGReferenceType::PythonEGReferenceType( HostEvaluator& evaluator )
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
    
}