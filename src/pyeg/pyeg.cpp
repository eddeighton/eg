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




#include "python_include.hpp"


//#define PY_ARRAY_UNIQUE_SYMBOL editor_api
//#include <numpy/arrayobject.h>

//static const char szCapsuleName[] = "editor._cppAPI";
//static const char szCapsuleAttributeName[] = "_cppAPI";

static PyObject* testMethod( PyObject* pSelf, PyObject *args )
{
    
    
    
    
    
    
    
    
    
    
    
    
    Py_INCREF( Py_None );
    return Py_None;
}

static PyMethodDef PyEgMethods[] = 
{
    { "testMethod",    testMethod,    METH_VARARGS,   "Test function" },
     
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef testModule = 
{
   PyModuleDef_HEAD_INIT,
   "pyeg",    /* name of module */
   0,           /* module documentation, may be NULL */
   -1,          /* size of per-interpreter state of the module,
                    or -1 if the module keeps state in global variables. */
   PyEgMethods
};

#ifdef _DEBUG
PyMODINIT_FUNC PyInit_pyegd(void)
#else
PyMODINIT_FUNC PyInit_pyeg(void)
#endif
{
    //import_array(); 
    
    PyObject* pModuleObject = PyModule_Create( &testModule );  
 
    //Editor::Editor* pEditor = new Editor::Editor( pModuleObject );
    
    //PyObject* pCapsule = PyCapsule_New( pEditor, szCapsuleName, &Editor::Editor::destructor );
    
    //PyModule_AddObject( pModuleObject, szCapsuleAttributeName, pCapsule );
    
    
        //void* pVoid = PyCapsule_GetPointer( pObject, szCapsuleName );
    
    return pModuleObject;
}
