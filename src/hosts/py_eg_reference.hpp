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


#ifndef PYTHON_EG_REFERENCE_TYPE_22_04_2019
#define PYTHON_EG_REFERENCE_TYPE_22_04_2019

//#include "eg_common.hpp" //rely on the precompiled header for this

//#include "eg_runtime/eg_runtime.hpp" //rely on the precompiled header for this

//#include <pybind11/pybind11.h> //rely on the precompiled header for this

//#include <vector> //rely on the precompiled header for this

extern eg::TimeStamp getTimestamp( eg::TypeID typeID, eg::Instance instance );
extern eg::ActionState getState( eg::TypeID typeID, eg::Instance instance );

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
    
    PyObject* create( reference ref );
    
    HostEvaluator& getEvaluator() const { return m_evaluator; }
private:
    HostEvaluator& m_evaluator;
    PyTypeObject* m_pTypeObject;
    std::vector< PyGetSetDef > m_pythonAttributesData;
    std::vector< const char* > m_identities;
};

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
            if( ( ::getState( m_pRange->getType( m_subRange ), m_position ) != action_stopped ) || 
                ( ::getStopCycle( m_pRange->getType( m_subRange ), m_position ) >= clock::cycle() ) )
                break;
            
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

#endif //PYTHON_EG_REFERENCE_TYPE_22_04_2019