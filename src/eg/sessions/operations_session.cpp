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


#include "eg_compiler/sessions/operations_session.hpp"

namespace eg
{


    OperationsSession::OperationsSession( const boost::filesystem::path& treePath,
            const boost::filesystem::path& tuPath, 
            IndexedObject::FileID fileID )
        :   CreatingSession( IndexedFile::FileIDtoPathMap{{ IndexedObject::MASTER_FILE, treePath }}, fileID ),
            m_tuPath( tuPath ),
            m_pDerivationAnalysis( one_cst< DerivationAnalysis >( getMaster() ) ),
            m_invocations( *this, *m_pDerivationAnalysis )
    {
        m_pIdentifiers = one_cst< Identifiers >( getMaster() );
    }

    void OperationsSession::store() const
    {
        CreatingSession::store( m_tuPath );
    }
    
    const Identifiers& OperationsSession::getIdentifiers() const
    {
        return *one< Identifiers >( getObjects( IndexedObject::MASTER_FILE ) );
    }
    
    const InvocationSolution* OperationsSession::getInvocation( 
        const InvocationSolution::InvocationID& invocationID, const std::vector< eg::TypeID >& implicitTypePath )
    {
        return m_invocations.getInvocation( invocationID, implicitTypePath );
    }
}