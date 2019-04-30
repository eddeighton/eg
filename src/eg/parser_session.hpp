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