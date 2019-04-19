
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
        
        std::vector< const abstract::Element* > fromEGTypeID( EGTypeID typeID, bool bForceDecl );
        const InvocationSolution* getInvocation( const InvocationSolution::Context& context,
            const InvocationSolution::TypePath& typePath, EGTypeID operationType, const std::vector< eg::EGTypeID >& implicitTypePath );
        
        void operationsAnalysis();
    private:
        const boost::filesystem::path m_tuPath;
        
        using InvocationID = std::tuple< InvocationSolution::Context, InvocationSolution::TypePath, EGTypeID >;
        using InvocationMap = std::map< InvocationID, InvocationSolution* >;
        InvocationMap m_invocations;
        
        const DerivationAnalysis* m_pDerivationAnalysis;
        const Identifiers* m_pIdentifiers;
    };


}

#endif //OPERATIONS_SESSION_19_04_2019