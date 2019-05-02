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


#ifndef OPERATIONS_SESSION_19_04_2019
#define OPERATIONS_SESSION_19_04_2019

#include "session.hpp"
#include "derivation.hpp"
#include "identifiers.hpp"

namespace eg
{

    class OperationsSession : public CreatingSession
    {
    public:

        OperationsSession( const boost::filesystem::path& treePath,
            const boost::filesystem::path& tuPath, 
            IndexedObject::FileID fileID );
        
        void store() const;
        
        std::vector< const abstract::Element* > fromEGTypeID( TypeID typeID, bool bForceDecl );
        const InvocationSolution* getInvocation( const InvocationSolution::Context& context,
            const InvocationSolution::TypePath& typePath, OperationID operationType, const std::vector< eg::TypeID >& implicitTypePath );
        
    private:
        const boost::filesystem::path m_tuPath;
        
        using InvocationID = std::tuple< InvocationSolution::Context, InvocationSolution::TypePath, TypeID >;
        using InvocationMap = std::map< InvocationID, InvocationSolution* >;
        InvocationMap m_invocations;
        
        const DerivationAnalysis* m_pDerivationAnalysis;
        const Identifiers* m_pIdentifiers;
    };


}

#endif //OPERATIONS_SESSION_19_04_2019