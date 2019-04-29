
#ifndef PYTHON_EG_REFERENCE_TYPE_22_04_2019
#define PYTHON_EG_REFERENCE_TYPE_22_04_2019

#include "eg_common.hpp"

#include <pybind11/embed.h> // everything needed for embedding

#include <vector>

namespace eg
{
    
class HostEvaluator
{
public:
    virtual ~HostEvaluator();
    
    virtual void getIdentities( std::vector< const char* >& identities ) = 0;
    virtual TypeID getTypeID( const char* pszIdentity ) = 0;
    virtual PyObject* invoke( const reference& reference, const std::vector< TypeID >& typePath, PyObject *args, PyObject *kwargs ) = 0;
};
    
class PythonEGReferenceType;

class PythonEGReference
{
public:
    PythonEGReference( PythonEGReferenceType& pythonType, const reference& ref );
    
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

class PythonEGReferenceType
{
public:
    static PythonEGReference* getReference( PyObject* pPyObject );

    PythonEGReferenceType( HostEvaluator& evaluator );
    
    PyObject* create( reference szInstanceID );
    
    HostEvaluator& getEvaluator() const { return m_evaluator; }
private:
    HostEvaluator& m_evaluator;
    PyTypeObject* m_pTypeObject;
    std::vector< PyGetSetDef > m_pythonAttributesData;
    std::vector< const char* > m_identities;
};

}

#endif //PYTHON_EG_REFERENCE_TYPE_22_04_2019