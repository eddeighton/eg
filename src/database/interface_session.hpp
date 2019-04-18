

#ifndef INTERFACE_SESSION_18_04_2019
#define INTERFACE_SESSION_18_04_2019

#include "session.hpp"

namespace eg
{

    
    class LinkAnalysisSession : public AppendingSession
    {
    public:
        LinkAnalysisSession( const boost::filesystem::path& treePath );
        
        //Instance_Action* getInstanceRoot() const;
        //DerivationAnalysis* getDerivationAnalysisObject();
        //        
        ////IInterfaceAnalysisSession
        //virtual void store() const
        //{
        //    AppendingSession::store();
        //}
        //virtual void release()
        //{
        //    delete this;
        //}
        //virtual void reportLink( IAction* pAction, const EGTypeID targetID );
        //virtual void reportObject( IAction* pAction, const StringView& objectCanonicalTypeString );
        //virtual void reportDependency( IAction* pAction, const StringView& objectCanonicalTypeString );
        //virtual void reportSize( IAction* pAction, std::size_t size );
        //virtual void reportSize( IDimension* pDimension, std::size_t size );
        //virtual void reportDimensionActions( IDimension* pDimension, const std::vector< EGTypeID >& actions );
        //virtual void reportDimensionCanonicalType( IDimension* pDimension, const StringView& objectCanonicalTypeString );
        //virtual StringView getInterfacePrefix() const;
        //virtual StringView getBaseType( std::size_t szIndex ) const;
        //virtual IRoot* getRoot() const;
        
        
        
    protected:        
        //DerivationAnalysis* m_pDerivationAnalysis;
        //std::unique_ptr< boost::filesystem::ofstream > m_pLog;
    };

}

#endif //INTERFACE_SESSION_18_04_2019