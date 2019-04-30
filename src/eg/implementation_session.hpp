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


#ifndef IMPLEMENTATION_SESSION_18_02_2019
#define IMPLEMENTATION_SESSION_18_02_2019

#include "session.hpp"
#include "concrete.hpp"
#include "derivation.hpp"
#include "layout.hpp"
#include "identifiers.hpp"

namespace eg
{
    class ImplementationSession : public CreatingSession
    {
    public:
        ImplementationSession( const IndexedFile::FileIDtoPathMap& files );
        
        const abstract::Root* getTreeRoot() const;
        const concrete::Action* getInstanceRoot() const;
        const DerivationAnalysis& getDerivationAnalysis() const;
        const Layout& getLayout() const;
        
        using Invocations = std::vector< const InvocationSolution* >;
        void getInvocations( std::size_t szTranslationUnitID, Invocations& invocations ) const;
        
        void fullProgramAnalysis();
    private:
        void recurseInstances( std::vector< Buffer* >& buffers, 
                Layout::DimensionMap& dimensionMap, std::size_t szSize, const concrete::Action* pAction );
    };
    
    class ReadSession : public Session
    {
    public:
        ReadSession( const boost::filesystem::path& filePath );
        
        const abstract::Root* getTreeRoot() const;
        const concrete::Action* getInstanceRoot() const;
        const DerivationAnalysis& getDerivationAnalysis() const;
        const Identifiers& getIdentifiers() const;
        const Layout& getLayout() const;
        //const abstract::Action* getAbstractAction( TypeID id ) const;
        const concrete::Action* getConcreteAction( TypeID id ) const;
        
        using Invocations = std::vector< const InvocationSolution* >;
        void getInvocations( std::size_t szTranslationUnitID, Invocations& invocations ) const;
    };
}

#endif //IMPLEMENTATION_SESSION_18_02_2019

