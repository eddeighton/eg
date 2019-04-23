
#ifndef PYTHON_EG_REFERENCE_TYPE_22_04_2019
#define PYTHON_EG_REFERENCE_TYPE_22_04_2019

#include "eg_common.hpp"

#include <pybind11/embed.h> // everything needed for embedding

#include <vector>

namespace eg
{
    
class EGRuntime;
    
class PythonEGReferenceType;

class PythonEGReference
{
public:
    PythonEGReference( PythonEGReferenceType& pythonType, const __eg_reference& ref );
    
    PyObject* get( void* pClosure );
    int set( void* pClosure, PyObject* pValue );
    PyObject* str() const;
    PyObject* call( PyObject *args, PyObject *kwargs );
    
    const __eg_reference getEGReference() const { return m_reference; }
private:
    PythonEGReferenceType& m_pythonType;
    __eg_reference m_reference;
    std::vector< EGTypeID > m_type_path;
};

class PythonEGReferenceType
{
public:
    static PythonEGReference* getReference( PyObject* pPyObject );

    PythonEGReferenceType( EGRuntime& runtime );
    
    PyObject* create( __eg_reference szInstanceID );
    
    EGRuntime& getRuntime() const { return m_runtime; }
private:
    EGRuntime& m_runtime;
    PyTypeObject* m_pTypeObject;
    std::vector< PyGetSetDef > m_pythonAttributesData;
    std::vector< std::string > m_identities;
};

}

#endif //PYTHON_EG_REFERENCE_TYPE_22_04_2019