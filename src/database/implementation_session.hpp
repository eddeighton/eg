
#ifndef IMPLEMENTATION_SESSION_18_02_2019
#define IMPLEMENTATION_SESSION_18_02_2019

#include "session.hpp"
#include "concrete.hpp"
#include "derivation.hpp"
#include "layout.hpp"

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
        const Layout& getLayout() const;
        
        using Invocations = std::vector< const InvocationSolution* >;
        void getInvocations( std::size_t szTranslationUnitID, Invocations& invocations ) const;
    };
}

#endif //IMPLEMENTATION_SESSION_18_02_2019

