
#ifndef EG_PARSER_18_04_2019
#define EG_PARSER_18_04_2019

#include "session.hpp"

#include "llvm/ADT/IntrusiveRefCntPtr.h"

#include <vector>
#include <memory>
#include <map>


namespace clang
{
    class DiagnosticsEngine;
    class FileManager;
}

namespace eg
{
    class Identifiers;
    
    namespace abstract
    {
        class Element;
        class Root;
    }

    namespace input
    {
        class Element;
        class Root;
    }
        
    class ParserSession : public CreatingSession
    {
    public:
        ParserSession();
        
        void parse( const std::vector< boost::filesystem::path >& egSourceCodeFiles, 
            std::shared_ptr< clang::FileManager > pFileManager,
            llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine );
            
        const abstract::Root* buildAbstractTree();
        
        const abstract::Root* getTreeRoot() const { return eg::root_cst< eg::abstract::Root >( getMaster() ); }
        
        const Identifiers* getIdentifiers() const { return eg::one_cst< eg::Identifiers >( getMaster() ); }
    private:
        using FileElementMap = std::map< boost::filesystem::path, input::Root* >;
        void buildTree( const FileElementMap& fileMap, abstract::Element*, input::Element* );
    };

}


#endif //EG_PARSER_18_04_2019