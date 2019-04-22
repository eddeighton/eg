


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
