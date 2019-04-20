
#ifndef CLANG_UTILS_19_04_2019
#define CLANG_UTILS_19_04_2019

#include "eg/eg.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/AST/Type.h"

#pragma warning( pop ) 

#include <optional>

namespace eg
{
    class InterfaceSession;
    class OperationsSession;
}

namespace clang
{
    std::optional< eg::EGTypeID > getEGTypeID( ASTContext* pASTContext, QualType type );

    const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters );
    
    bool getContextTypes( ASTContext* pASTContext, QualType contextType, std::vector< eg::EGTypeID >& contextTypes );
    
    bool getTypePathTypes( ASTContext* pASTContext, QualType typePath, std::vector< eg::EGTypeID >& typePathTypes );
    
    std::optional< int > getConstant( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, 
        const SourceLocation& loc, const std::string& strConstantName );
    
    QualType getVoidType( ASTContext* pASTContext );
    QualType getBooleanType( ASTContext* pASTContext );
    QualType getIntType( ASTContext* pASTContext );
    
    QualType getTypeTrait( ASTContext* pASTContext, Sema* pSema, 
        DeclContext* pDeclContext, const SourceLocation& loc, const std::string& strTypeName );
    QualType getType( ASTContext* pASTContext, Sema* pSema, const std::string& strTypeName, const std::string& strTypeParam, 
        DeclContext*& pDeclContext, SourceLocation& loc, bool bLast );
        
    void interfaceAnalysis( ASTContext* pASTContext, Sema* pSema, eg::InterfaceSession& session );
    void operationsAnalysis( ASTContext* pASTContext, Sema* pSema, eg::OperationsSession& session );
}

#endif //CLANG_UTILS_19_04_2019