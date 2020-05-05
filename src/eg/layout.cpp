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


#include "eg_compiler/layout.hpp"
#include "eg_compiler/concrete.hpp"

namespace eg
{

void DataMember::load( Loader& loader )
{
    m_pDimension = loader.loadObjectRef< concrete::Dimension >();
    loader.load( offset );
    loader.load( name );
    m_pBuffer = loader.loadObjectRef< Buffer >();
    m_pDependency = loader.loadObjectRef< DataMember >();
}

void DataMember::store( Storer& storer ) const
{
    storer.storeObjectRef( m_pDimension );
    storer.store( offset );
    storer.store( name );
    storer.storeObjectRef( m_pBuffer );
    storer.storeObjectRef( m_pDependency );
}

void DataMember::print( std::ostream& os ) const
{
    m_pDimension->printType( os );
}


void DataMember::printVariableAccess( std::ostream& os, const std::string& strIndex ) const
{
    os << getBuffer()->getVariableName() << "[ " << strIndex << " ]." << getName();
}

void DataMember::printDependencyVar( std::ostream& os, const std::string& strIndex ) const
{
    if( m_pDependency )
    {
        const concrete::Dimension_Generated* pGen = 
            dynamic_cast< const concrete::Dimension_Generated* >( m_pDimension );
        
        os << m_pDependency->getBuffer()->getVariableName() << 
            "[ " << strIndex << " / " << pGen->getDependencyDomain() << " ]." << m_pDependency->getName();
    }
}

void DataMember::printAllocation( std::ostream& os, const std::string& strIndex ) const
{
    return m_pDimension->printAllocation( os, *this, strIndex );
}
void DataMember::printDeallocation( std::ostream& os, const std::string& strIndex ) const
{
    return m_pDimension->printDeallocation( os, *this, strIndex );
}
void DataMember::printStart( std::ostream& os, const std::string& strIndex ) const
{
    return m_pDimension->printStart( os, *this, strIndex );
}
void DataMember::printStop( std::ostream& os, const std::string& strIndex ) const
{
    return m_pDimension->printStop( os, *this, strIndex );
}

void DataMember::printEncode( std::ostream& os, const std::string& strIndex ) const
{
    return m_pDimension->printEncode( os, *this, strIndex );
}
void DataMember::printDecode( std::ostream& os, const std::string& strIndex ) const
{
    return m_pDimension->printDecode( os, *this, strIndex );
}

void Buffer::load( Loader& loader )
{
    m_pAction = loader.loadObjectRef< concrete::Action >();
    loader.loadObjectVector( m_dimensions );
    loader.load( size );
    loader.load( stride );
    loader.load( name );
    loader.load( variable );
}

void Buffer::store( Storer& storer ) const
{
    storer.storeObjectRef( m_pAction );
    storer.storeObjectVector( m_dimensions );
    storer.store( size );
    storer.store( stride );
    storer.store( name );
    storer.store( variable );
}
    
void Layout::load( Loader& loader )
{
    loader.loadObjectVector( m_buffers );
    loader.loadObjectMap( m_dimensionMap );
}

void Layout::store( Storer& storer ) const
{
    storer.storeObjectVector( m_buffers );
    storer.storeObjectMap( m_dimensionMap );
}    

} //namespace eg