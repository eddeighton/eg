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


#ifndef CLANG_UTILS_19_04_2019
#define CLANG_UTILS_19_04_2019

#include "eg_compiler/eg.hpp"

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
    std::optional< eg::TypeID > getEGTypeID( ASTContext* pASTContext, QualType type );

    const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters );
    
    bool getContextTypes( ASTContext* pASTContext, QualType contextType, std::vector< eg::TypeID >& contextTypes );
    
    bool getTypePathTypes( ASTContext* pASTContext, QualType typePath, std::vector< eg::TypeID >& typePathTypes );
    
    std::optional< int > getConstant( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, 
        const SourceLocation& loc, const std::string& strConstantName );
    
    QualType getVariantType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
        const std::vector< QualType >& typeParameters );
        
    //QualType getIteratorType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
    //    const clang::QualType& interfaceType, const char* pszIteratorTypeName );
        
    QualType getIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
        const clang::QualType& interfaceType, const char* pszIteratorTypeName );
        
    QualType getMultiIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
        const clang::QualType& interfaceType, std::size_t szTargetTypes, const char* pszIteratorTypeName );
    
    QualType getVoidType( ASTContext* pASTContext );
    QualType getBooleanType( ASTContext* pASTContext );
    QualType getIntType( ASTContext* pASTContext );
    QualType getUIntType( ASTContext* pASTContext );
    
    QualType getTypeTrait( ASTContext* pASTContext, Sema* pSema, 
        DeclContext* pDeclContext, const SourceLocation& loc, const std::string& strTypeName );
    QualType getType( ASTContext* pASTContext, Sema* pSema, const std::string& strTypeName, const std::string& strTypeParam, 
        DeclContext*& pDeclContext, SourceLocation& loc, bool bLast );
        
    bool interfaceAnalysis( ASTContext* pASTContext, Sema* pSema, eg::InterfaceSession& session );
    bool operationsAnalysis( ASTContext* pASTContext, Sema* pSema, eg::OperationsSession& session );
    
    struct DeclLocType
    {
        DeclContext* pContext = nullptr;
        SourceLocation loc;
        QualType type;
    };
    DeclLocType getNestedDeclContext( ASTContext* pASTContext, Sema* pSema, 
        DeclContext* pDeclContext, SourceLocation loc, const std::string& str, bool bIsTemplate );
}

#endif //CLANG_UTILS_19_04_2019