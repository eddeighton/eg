
#ifndef CODE_GEN_18_04_2019
#define CODE_GEN_18_04_2019


#include "abstract.hpp"
#include "implementation_session.hpp"

#include <string>
#include <cstddef>
#include <vector>
#include <ostream>

namespace eg
{

    
    std::string getInterfaceType( const std::string& strType );
    std::string getBaseTraitType( std::size_t szIndex );
    std::string getInterfaceInstantiationType( const std::string& strType, int iDepth );
    
    void generateIncludeHeader( std::ostream& os, abstract::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser );
    
    void generateInterface( std::ostream& os, abstract::Root* pRoot );

    void generateOperationSource( std::ostream& os, abstract::Root* pRoot );

    void generateImplementationSource( std::ostream& os, const ReadSession& program, 
        std::size_t szTranslationUnitID, const std::vector< std::string >& dependencies );
}

#endif //CODE_GEN_18_04_2019