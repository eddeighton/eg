
#ifndef CLANG_UTILS_19_04_2019
#define CLANG_UTILS_19_04_2019

#include "eg.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/AST/Type.h"

#pragma warning( pop ) 

#include <optional>

namespace clang
{
    std::optional< eg::EGTypeID > getEGTypeID( QualType type );

    const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters );
    
    bool getContextTypes( ASTContext* pASTContext, QualType contextType, std::vector< eg::EGTypeID >& contextTypes );
    
    bool getTypePathTypes( ASTContext* pASTContext, QualType typePath, std::vector< eg::EGTypeID >& typePathTypes );
}

#endif //CLANG_UTILS_19_04_2019