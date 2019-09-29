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


#include "clang_utils.hpp"

#include "eg/eg.hpp"
#include "eg/sessions/interface_session.hpp"
#include "eg/codegen/codegen.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Sema/Ownership.h"
#include "clang/Sema/ParsedTemplate.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/Lookup.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Type.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Expr.h"
#include "clang/Lex/Token.h"
#include "clang/Basic/DiagnosticParse.h"

#include "llvm/ADT/APSInt.h"

#pragma warning( pop ) 

#include <iostream>

namespace clang
{

    std::optional< eg::TypeID > getEGTypeID( ASTContext* pASTContext, QualType type )
    {
        std::optional< eg::TypeID > result;
        
        if( type.getTypePtrOrNull() && !type->isDependentType() )
        {
            QualType canonicalType = type.getCanonicalType();
            
            if( const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier() )
            {
                if( pBaseTypeID == pASTContext->getEGTypePathName() )
                {
                    return eg::id_TypePath;
                }
            }
            
            if( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
            {
                if( const CXXRecordDecl* pRecordDecl = dyn_cast<CXXRecordDecl>( pRecordType->getDecl() ) )
                {
                    if( pRecordDecl->hasAttr< EGTypeIDAttr >() )
                    {
                        if( EGTypeIDAttr* pAttr = pRecordDecl->getAttr< EGTypeIDAttr >() )
                        {
                            return pAttr->getId();
                        }
                    }
                }
            }
        }
        
        return result;
    }

    const IdentifierInfo* getOperationIdentifier( ASTContext* pASTContext, const std::string& strName )
    {
        return &pASTContext->Idents.get( strName.c_str() );
    }
    const IdentifierInfo* getImplicitNoParamsOperation( ASTContext* pASTContext )
    {
        return getOperationIdentifier( pASTContext, eg::getOperationString( eg::id_Imp_NoParams ) );
    }
    const IdentifierInfo* getImplicitParamsOperation( ASTContext* pASTContext )
    {
        return getOperationIdentifier( pASTContext, eg::getOperationString( eg::id_Imp_Params ) );
    }
    
    const IdentifierInfo* getOperationID( ASTContext* pASTContext, QualType ty, bool bHasParameters )
    {
        QualType canonicalType = ty.getCanonicalType();
        const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier();
        
        if( !pBaseTypeID )
            return nullptr;
        
        if( pBaseTypeID == pASTContext->getEGTypePathName() )
        {
            const Type* pType = canonicalType.getTypePtr();
            
            if( const TemplateSpecializationType* pTemplateType = 
                canonicalType->getAs< TemplateSpecializationType >() )
            {
                if( pTemplateType->getNumArgs() == 0U )
                    return nullptr;
                        
                const TemplateArgument& lastTemplateArg = 
                    pTemplateType->getArg( pTemplateType->getNumArgs() - 1U );
                QualType t = lastTemplateArg.getAsType();
                return getOperationID( pASTContext, t, bHasParameters );
            }
            else if( const DependentTemplateSpecializationType* pDependentTemplateType = 
                llvm::dyn_cast< const DependentTemplateSpecializationType >( pType ) )
            {
                if( pTemplateType->getNumArgs() == 0U )
                    return nullptr;
                const TemplateArgument& lastTemplateArg = 
                    pTemplateType->getArg( pTemplateType->getNumArgs() - 1U );
                QualType t = lastTemplateArg.getAsType();
                return getOperationID( pASTContext, t, bHasParameters );
            }
            else if( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
            {
                const CXXRecordDecl* pRecordDecl = llvm::dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
                if( !pRecordDecl )
                    return nullptr;

                const auto* Spec = llvm::dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
                if( !Spec )
                    return nullptr;

                const TemplateArgumentList& Args = Spec->getTemplateInstantiationArgs();
                if( Args.size() == 0U )
                    return nullptr;
                
                const TemplateArgument& lastTemplateArg = Args[ Args.size() -1U ];

                if( lastTemplateArg.getKind() == TemplateArgument::Pack )
                {
                    const TemplateArgument& lastTemplatePackArg = lastTemplateArg.pack_elements().back();
                    QualType t = lastTemplatePackArg.getAsType();
                    return getOperationID( pASTContext, t, bHasParameters );
                }
                else if( lastTemplateArg.getKind() == TemplateArgument::Type )
                {
                    QualType t = lastTemplateArg.getAsType();
                    return getOperationID( pASTContext, t, bHasParameters );
                }
                else
                {
                    return nullptr;
                }
            }
            else
            {
                return nullptr;
            }
        }
        
        if( ::eg::getOperationName( pBaseTypeID->getName() ) != ::eg::HIGHEST_OPERATION_TYPE )
        {
            return getOperationIdentifier( pASTContext, pBaseTypeID->getName() );
        }
        else if( bHasParameters )
        {
            return getImplicitParamsOperation( pASTContext );
        }
        else
        {
            return getImplicitNoParamsOperation( pASTContext );
        }
    }
    
    bool getContextTypes( ASTContext* pASTContext, QualType contextType, std::vector< eg::TypeID >& contextTypes )
    {
        QualType canonicalType = contextType.getCanonicalType();
        if( const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier() )
        {
            if( pBaseTypeID == pASTContext->getEGVariantName() )
            {
                const Type* pType = canonicalType.getTypePtr();
                
                if( const  TemplateSpecializationType* pTemplateType = 
                    canonicalType->getAs<  TemplateSpecializationType >() )
                {
                    if( pTemplateType->getNumArgs() == 0U ) 
                        return false;
                    
                    bool bSuccess = false;
                    for( TemplateSpecializationType::iterator 
                        pIter = pTemplateType->begin(),
                        pIterEnd = pTemplateType->end(); pIter != pIterEnd; ++pIter )
                    {
                        if( !getContextTypes( pASTContext, pIter->getAsType(), contextTypes ) )
                            return false;
                        else
                            bSuccess = true;
                    }
                    return bSuccess;
                }
                else if( const DependentTemplateSpecializationType* pDependentTemplateType = 
                    llvm::dyn_cast< const  DependentTemplateSpecializationType >( pType ) )
                {
                    return false;
                }
                else if( const RecordType* pRecordType = canonicalType->getAs<  RecordType >() )
                {
                    const CXXRecordDecl* pRecordDecl = llvm::dyn_cast< CXXRecordDecl >( pRecordType->getDecl() );
                    if( !pRecordDecl )
                        return false;
                
                    const auto* Spec = llvm::dyn_cast< ClassTemplateSpecializationDecl >( pRecordDecl );
                    if( !Spec )
                        return false;
                
                    bool bSuccess = false;
                    const TemplateArgumentList& Args = Spec->getTemplateInstantiationArgs();
                    for( unsigned i = 0; i < Args.size(); ++i )
                    {
                        const TemplateArgument& arg = Args[ i ];
                        if( arg.getKind() == TemplateArgument::Pack )
                        {
                            for( TemplateArgument::pack_iterator 
                                 j = arg.pack_begin(),
                                 jEnd = arg.pack_end(); j!=jEnd; ++j )
                            {
                                const TemplateArgument& packArg = *j;
                                if( !getContextTypes( pASTContext, packArg.getAsType(), contextTypes ) )
                                    return false;
                                else
                                    bSuccess = true;
                            }
                        }
                        else if( arg.getKind() == TemplateArgument::Type )
                        {
                            if( !getContextTypes( pASTContext, arg.getAsType(), contextTypes ) )
                                return false;
                            else
                                bSuccess = true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    return bSuccess;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                if( std::optional< eg::TypeID > egTypeID = getEGTypeID( pASTContext, canonicalType ) )
                {
                    contextTypes.push_back( egTypeID.value() ); 
                    return true;
                }
                else
                {
                    return false;
                }
            }
        }
        else
        {
            return false;
        }
    }
    
    bool getTypePathTypes( ASTContext* pASTContext, QualType typePath, std::vector< eg::TypeID >& typePathTypes)
    {
        QualType canonicalType = typePath.getCanonicalType();
        const IdentifierInfo* pBaseTypeID = canonicalType.getBaseTypeIdentifier();
        if( !pBaseTypeID )
            return false;
        
        if( pBaseTypeID == pASTContext->getEGTypePathName() )
        {
            const Type* pType = canonicalType.getTypePtr();
            
            if( const TemplateSpecializationType* pTemplateType = 
                canonicalType->getAs< TemplateSpecializationType >() )
            {
                if( pTemplateType->getNumArgs() == 0U )
                    return false;
                
                bool bSuccess = false;
                for( TemplateSpecializationType::iterator 
                    pIter = pTemplateType->begin(),
                    pIterEnd = pTemplateType->end(); pIter != pIterEnd; ++pIter )
                {
                    if( !getTypePathTypes( pASTContext, pIter->getAsType(), typePathTypes ) )
                        return false;
                    else
                        bSuccess = true;
                }
                return bSuccess;
            }
            else if( const DependentTemplateSpecializationType* pDependentTemplateType = 
                dyn_cast< const DependentTemplateSpecializationType >( pType ) )
            {
                return false;
            }
            else if( const RecordType* pRecordType = canonicalType->getAs< RecordType >() )
            {
                const CXXRecordDecl* pRecordDecl = dyn_cast<CXXRecordDecl>( pRecordType->getDecl() );
                if( !pRecordDecl )
                    return false;
            
                const auto* Spec = dyn_cast<ClassTemplateSpecializationDecl>( pRecordDecl );
                if( !Spec )
                    return false;
            
                bool bSuccess = false;
                const TemplateArgumentList& Args = Spec->getTemplateInstantiationArgs();
                for( unsigned i = 0; i < Args.size(); ++i )
                {
                    const TemplateArgument& arg = Args[ i ];
                    if( arg.getKind() == TemplateArgument::Pack )
                    {
                        for( TemplateArgument::pack_iterator 
                             j = arg.pack_begin(),
                             jEnd = arg.pack_end(); j!=jEnd; ++j )
                        {
                            const TemplateArgument& packArg = *j;
                            if( !getTypePathTypes( pASTContext, packArg.getAsType(), typePathTypes ) )
                                return false;
                            else
                                bSuccess = true;
                        }
                    }
                    else if( arg.getKind() == TemplateArgument::Type )
                    {
                        if( !getTypePathTypes( pASTContext, arg.getAsType(), typePathTypes ) )
                            return false;
                        else
                            bSuccess = true;
                    }
                    else
                    {
                        return false;
                    }
                }
                return bSuccess;
            }
            else
            {
                return false;
            }
        }
        else
        {
            if( std::optional< eg::TypeID > egTypeID = getEGTypeID( pASTContext, canonicalType ) )
            {
                typePathTypes.push_back( egTypeID.value() ); 
                return true;
            }
            else
            {
                return false;
            }
        }
        
    }
    
    std::optional< int > getConstant( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, 
        const SourceLocation& loc, const std::string& strConstantName )
    {
        IdentifierInfo& identifierInfo = pASTContext->Idents.get( strConstantName );
        LookupResult lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
        {
            NamedDecl* pDecl = lookupResult.getFoundDecl();
            if( VarDecl* pVarDecl = dyn_cast< VarDecl >( pDecl ) )
            {
                if( const clang::Expr* pInitialisationExpr = pVarDecl->getInit() )
                {
                    clang::Expr::EvalResult result;
                    if ( pInitialisationExpr->EvaluateAsInt( result, *pASTContext ) ) 
                    {
                        return result.Val.getInt().getExtValue();
                    }
                }
            }
        }
        return std::optional< int >();
    }
    
    QualType getVariantType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
        const std::vector< QualType >& typeParameters )
    {
        IdentifierInfo& identifierInfo = pASTContext->Idents.get( eg::EG_VARIANT_TYPE );
        LookupResult result( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( result, pDeclContext ) )
        {
            if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
            {
                loc = pDecl->getTemplatedDecl()->getBeginLoc();
                TemplateArgumentListInfo TemplateArgs( loc, loc );
                
                for( QualType qt : typeParameters )
                {
                    TemplateArgs.addArgument( 
                        TemplateArgumentLoc( 
                            TemplateArgument( qt ),
                            pASTContext->getTrivialTypeSourceInfo( qt, loc ) ) );
                }
                
                
                TemplateName templateName( pDecl );
                return pSema->CheckTemplateIdType( templateName, loc, TemplateArgs );
            }
        }
        return QualType();
    }
    
    QualType getIteratorType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
        const clang::QualType& interfaceType, const char* pszIteratorTypeName )
    {
        SourceLocation iterLoc;
        IdentifierInfo& identifierInfo = pASTContext->Idents.get( pszIteratorTypeName );
        LookupResult result( *pSema, &identifierInfo, iterLoc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( result, pDeclContext ) )
        {
            if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
            {
                iterLoc = pDecl->getTemplatedDecl()->getBeginLoc();
                TemplateArgumentListInfo TemplateArgs( iterLoc, iterLoc );
                
                TemplateArgumentLocInfo tali;
                {
                    TemplateArgs.addArgument( 
                        TemplateArgumentLoc( 
                            TemplateArgument( interfaceType ), 
                            pASTContext->getTrivialTypeSourceInfo( interfaceType, loc ) ) );
                }                
                
                TemplateName templateName( pDecl );
                return pSema->CheckTemplateIdType( templateName, iterLoc, TemplateArgs );
            }
        }
        return QualType();
    }
    
    QualType getIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
        const clang::QualType& interfaceType, const char* pszIteratorTypeName )
    {
        QualType iteratorType = 
            getIteratorType( pASTContext, pSema, pDeclContext, loc, interfaceType, pszIteratorTypeName );
        
        SourceLocation rangeLoc;
        IdentifierInfo& rangeIdentifierInfo = pASTContext->Idents.get( eg::EG_RANGE_TYPE );
        LookupResult result( *pSema, &rangeIdentifierInfo, rangeLoc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( result, pDeclContext ) )
        {
            if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
            {
                rangeLoc = pDecl->getTemplatedDecl()->getBeginLoc();
                TemplateArgumentListInfo TemplateArgs( rangeLoc, rangeLoc );
                
                TemplateArgs.addArgument( 
                    TemplateArgumentLoc( 
                        TemplateArgument( iteratorType ), 
                        pASTContext->getTrivialTypeSourceInfo( iteratorType, loc ) ) );
                TemplateName templateName( pDecl );
                return pSema->CheckTemplateIdType( templateName, rangeLoc, TemplateArgs );
            }
        }
                
        return QualType();
    }
        
    QualType getMultiIteratorRangeType( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, SourceLocation loc, 
        const clang::QualType& interfaceType, std::size_t szTargetTypes, const char* pszIteratorTypeName )
    {
        QualType iteratorType = 
            getIteratorType( pASTContext, pSema, pDeclContext, loc, interfaceType, pszIteratorTypeName );
        
        SourceLocation iterLoc;
        IdentifierInfo& identifierInfo = pASTContext->Idents.get( eg::EG_MULTI_ITERATOR_TYPE );
        LookupResult result( *pSema, &identifierInfo, iterLoc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( result, pDeclContext ) )
        {
            if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
            {
                iterLoc = pDecl->getTemplatedDecl()->getBeginLoc();
                TemplateArgumentListInfo TemplateArgs( iterLoc, iterLoc );
                
                TemplateArgumentLocInfo tali;
                {
                    TemplateArgs.addArgument( 
                        TemplateArgumentLoc( 
                            TemplateArgument( iteratorType ), 
                            pASTContext->getTrivialTypeSourceInfo( iteratorType, loc ) ) );
                }
                {
                    const llvm::APInt actualValue( 8U * 4U, static_cast< uint64_t >( szTargetTypes ), false );
                    const llvm::APSInt compileTimeIntValue( actualValue );
                    
                    TemplateArgumentLocInfo tali;
                    TemplateArgs.addArgument( 
                        TemplateArgumentLoc( 
                            TemplateArgument( *pASTContext, compileTimeIntValue, getUIntType( pASTContext ) ), tali ) );
                }
                
                
                TemplateName templateName( pDecl );
                QualType multiIteratorType = pSema->CheckTemplateIdType( templateName, iterLoc, TemplateArgs );
                
                SourceLocation rangeLoc;
                IdentifierInfo& rangeIdentifierInfo = pASTContext->Idents.get( eg::EG_RANGE_TYPE );
                LookupResult result( *pSema, &rangeIdentifierInfo, rangeLoc, Sema::LookupAnyName );
                if( pSema->LookupQualifiedName( result, pDeclContext ) )
                {
                    if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( result.getFoundDecl() ) )
                    {
                        rangeLoc = pDecl->getTemplatedDecl()->getBeginLoc();
                        TemplateArgumentListInfo TemplateArgs( rangeLoc, rangeLoc );
                        
                        TemplateArgs.addArgument( 
                            TemplateArgumentLoc( 
                                TemplateArgument( multiIteratorType ), 
                                pASTContext->getTrivialTypeSourceInfo( multiIteratorType, loc ) ) );
                        TemplateName templateName( pDecl );
                        return pSema->CheckTemplateIdType( templateName, rangeLoc, TemplateArgs );
                    }
                }
                
            }
        }
        return QualType();
    }
    
    QualType getVoidType( ASTContext* pASTContext )
    {
        return pASTContext->VoidTy;
    }
    QualType getBooleanType( ASTContext* pASTContext )
    {
        return pASTContext->BoolTy;
    }
    QualType getIntType( ASTContext* pASTContext )
    {
        return pASTContext->IntTy;
    }
    QualType getUIntType( ASTContext* pASTContext )
    {
        return pASTContext->UnsignedIntTy;
    }
    
    QualType getTypeTrait( ASTContext* pASTContext, Sema* pSema, DeclContext* pDeclContext, const SourceLocation& loc, const std::string& strTypeName )
    {
        IdentifierInfo& identifierInfo = pASTContext->Idents.get( strTypeName );
        LookupResult lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
        {
            if( TypeAliasDecl* pTypeAliasDecl = llvm::dyn_cast<TypeAliasDecl>( lookupResult.getFoundDecl() ) )
            {
                return pASTContext->getTypeDeclType( pTypeAliasDecl );
            }
        }
        pDeclContext = nullptr;
        return QualType();
    }
    
    QualType getType( ASTContext* pASTContext, Sema* pSema, const std::string& strTypeName, const std::string& strTypeParam, 
        DeclContext*& pDeclContext, SourceLocation& loc, bool bLast )
    {
        IdentifierInfo& identifierInfo = pASTContext->Idents.get( strTypeName );
        LookupResult Result( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( Result, pDeclContext ) )
        {
            if( ClassTemplateDecl* pDecl = llvm::dyn_cast< ClassTemplateDecl >( Result.getFoundDecl() ) )
            {
                loc = pDecl->getTemplatedDecl()->getBeginLoc();
                TemplateArgumentListInfo TemplateArgs( loc, loc );
                TemplateArgs.addArgument( 
                    TemplateArgumentLoc( 
                        TemplateArgument( pASTContext->VoidTy ),
                        pASTContext->getTrivialTypeSourceInfo( pASTContext->VoidTy, loc ) ) );

                TemplateName templateName( pDecl );
                QualType templateSpecializationType = 
                    pSema->CheckTemplateIdType( templateName, loc, TemplateArgs );
                if( bLast )
                {
                    return templateSpecializationType;
                }
                else
                {
                    void* InsertPos = nullptr;
                    SmallVector<TemplateArgument, 4> Converted;
                    Converted.push_back( TemplateArgument( pASTContext->VoidTy ) );
                    if( ClassTemplateSpecializationDecl* Decl = 
                        pDecl->findSpecialization( Converted, InsertPos ) )
                    {
                        pDeclContext = Decl;
                        return templateSpecializationType;
                    }
                }
            }
        }
        pDeclContext = nullptr;
        return QualType();
    }
    
    
    struct DeclLocType
    {
        DeclContext* pContext = nullptr;
        SourceLocation loc;
        QualType type;
    };
    DeclLocType getNestedDeclContext( ASTContext* pASTContext, Sema* pSema, 
        DeclContext* pDeclContext, SourceLocation loc, const std::string& str, bool bIsTemplate )
    {
        DeclLocType result;
        
        IdentifierInfo& identifierInfo = pASTContext->Idents.get( str );
        LookupResult lookupResult( *pSema, &identifierInfo, loc, Sema::LookupAnyName );
        if( pSema->LookupQualifiedName( lookupResult, pDeclContext ) )
        {
            if( bIsTemplate )
            {
                ClassTemplateDecl* pDecl = dyn_cast<ClassTemplateDecl>( lookupResult.getFoundDecl() );
                result.loc = pDecl->getTemplatedDecl()->getBeginLoc();
                TemplateArgumentListInfo TemplateArgs( result.loc, result.loc );
                TemplateArgs.addArgument( 
                    TemplateArgumentLoc( 
                        TemplateArgument( pASTContext->VoidTy ),
                        pASTContext->getTrivialTypeSourceInfo( pASTContext->VoidTy, result.loc ) ) );
                        
                TemplateName templateName( pDecl );
                result.type = pSema->CheckTemplateIdType( templateName, result.loc, TemplateArgs );
                    
                void* InsertPos = nullptr;
                SmallVector<TemplateArgument, 4> Converted;
                Converted.push_back( TemplateArgument( pASTContext->VoidTy ) );
                if( ClassTemplateSpecializationDecl* pClassSpecialisationDeclaration = 
                    pDecl->findSpecialization( Converted, InsertPos ) )
                {
                    result.pContext = pClassSpecialisationDeclaration;
                }
            }
            else
            {
                if( CXXRecordDecl* pRecordDecl = dyn_cast<CXXRecordDecl>( lookupResult.getFoundDecl() ) )
                {
                    result.pContext = pRecordDecl;
                    result.loc = pRecordDecl->getBeginLoc();
                    result.type = pASTContext->getTypeDeclType( pRecordDecl );
                }
            }
        }
        return result;
    } 
    
    class AbstractMutator
    {
    public:
        static void setSize( ::eg::interface::Action& action, std::size_t szSize )
        {
            action.m_size = szSize;
        }
        static void setSize( ::eg::interface::Dimension& dimension, std::size_t szSize )
        {
            dimension.m_size = szSize;
        }
        static void setCanonicalType( ::eg::interface::Dimension& dimension, const std::string& strType )
        {
            dimension.m_canonicalType = strType;
        }
        static void setCanonicalType( ::eg::interface::Using& using_, const std::string& strType )
        {
            using_.m_canonicalType = strType;
        }
        static void appendActionTypes( ::eg::interface::Dimension& dimension, ::eg::interface::Action* pAction )
        {
            dimension.m_actionTypes.push_back( pAction );
        }
        static void setInherited( ::eg::interface::Action& action, ::eg::interface::Action* pLink )
        {
            std::vector< ::eg::interface::Action* >::iterator iFind = 
                std::find( action.m_baseActions.begin(), action.m_baseActions.end(), pLink );
            if( iFind == action.m_baseActions.end() )
            {
                action.m_baseActions.push_back( pLink );
            }
        }
        static void setInherited( ::eg::interface::Action& action, const std::string& strType )
        {
            action.m_strBaseType = strType;
        }
        static void setDependency( ::eg::interface::Action& action, const std::string& strType )
        {
            action.m_strDependency = strType;
        }
        
        static void setParameters( ::eg::interface::Action& action, const ArrayRef< ParmVarDecl * >& parameters )
        {
            for( const ParmVarDecl* pParam : parameters )
            {
                action.m_parameterTypes.push_back( pParam->getType().getCanonicalType().getAsString() );
            }
        }
        
    };
    
    bool interfaceAnalysis( ASTContext* pASTContext, Sema* pSema, eg::InterfaceSession& session, 
        eg::interface::Action* pAction, SourceLocation loc, DeclContext* pContext )
    {
        ::eg::IndexedObject::Array& objects = session.getAppendingObjects();
                            
        DeclLocType result = getNestedDeclContext( pASTContext, pSema, 
            pContext, loc, ::eg::getInterfaceType( pAction->getIdentifier() ), true );
        if( result.pContext )
        {
            //get the parameter list
            //if( !dynamic_cast< eg::interface::Root* >( pAction ) )
            
            CXXRecordDecl* pInterface = dyn_cast< CXXRecordDecl >( result.pContext );
            if( !pInterface )
            {
                std::ostringstream os;
                os << "Failed to locate interface type: " << pAction->getFriendlyName();
                pASTContext->getDiagnostics().Report( clang::diag::err_eg_generic_error ) << os.str();
                return false;
            }
            else
            {
                if( pAction->hasDefinition() )
                {
                    CXXMethodDecl* pApplicationOperator = nullptr;
                    for( CXXRecordDecl::method_iterator i = pInterface->method_begin(),
                        iEnd = pInterface->method_end(); i!=iEnd; ++i )
                    {
                        CXXMethodDecl* pMethod = *i;
                        if( pMethod->getNameInfo().getName().getAsString() == "operator()" )
                        {
                            pApplicationOperator = pMethod;
                            break;
                        }
                    }
                
                    if( !pApplicationOperator )
                    {
                        std::ostringstream os;
                        os << "Failed to locate interface member operator() for " << pAction->getFriendlyName();
                        pASTContext->getDiagnostics().Report( clang::diag::err_eg_generic_error ) << os.str();
                        return false;
                    }
                    else
                    {
                        AbstractMutator::setParameters( *pAction, pApplicationOperator->parameters() );
                    }
                }
            }
            
            //get the optional allocation size
            if( std::optional< int > sizeOpt = getConstant( pASTContext, pSema, result.pContext, result.loc, "SIZE" ) )
            {
                AbstractMutator::setSize( *pAction, static_cast< std::size_t >( sizeOpt.value() ) );
            }
            
            //determine the dimension types
            std::vector< ::eg::interface::Dimension* > dimensions;
            pAction->getDimensions( dimensions );
            for( ::eg::interface::Dimension* pDimension : dimensions )
            {
                DeclLocType dimensionResult = getNestedDeclContext( pASTContext, pSema,
                    result.pContext, result.loc, ::eg::getInterfaceType( pDimension->getIdentifier() ), true );
                if( dimensionResult.pContext )
                {
                    //determine the type
                    {
                        QualType typeType = getTypeTrait( pASTContext, pSema, dimensionResult.pContext, dimensionResult.loc, "Type" );
                        
                        QualType typeTypeCanonical = typeType.getCanonicalType();
                        
                        std::vector< eg::TypeID > dimensionTypes;
                        
                        //only attempt this is it has a base type identifier
                        if( typeTypeCanonical.getBaseTypeIdentifier() )
                        {
                            getContextTypes( pASTContext, typeTypeCanonical, dimensionTypes );
                        }
                
                        if( dimensionTypes.empty() || 
                            ( ( dimensionTypes.size() == 1U ) && ( dimensionTypes.front() == 0 ) ) )
                        {
                            AbstractMutator::setCanonicalType( *pDimension, typeTypeCanonical.getAsString() );
                        }
                        else if( !dimensionTypes.empty() )
                        {
                            for( eg::TypeID index : dimensionTypes )
                            {
                                if( index > 0 && index < static_cast< ::eg::TypeID >( objects.size() ) )
                                {
                                    ::eg::IndexedObject* pObject = objects[ index ];
                                    if( ::eg::interface::Action* pAction = dynamic_cast< ::eg::interface::Action* >( pObject ) )
                                    {
                                        AbstractMutator::appendActionTypes( *pDimension, pAction );
                                    }
                                }
                                else
                                {
                                    //diag
                                    std::ostringstream os;
                                    os << "Invalid dimension type: " << pDimension->getFriendlyName();
                                    pASTContext->getDiagnostics().Report( clang::diag::err_eg_generic_error ) << os.str();
                                    return false;
                                }
                            }
                        }
                    }
                    //determine the size
                    if( std::optional< int > sizeOpt = getConstant( pASTContext, pSema, dimensionResult.pContext, dimensionResult.loc, "SIZE" ) )
                    {
                        AbstractMutator::setSize( *pDimension, static_cast< std::size_t >( sizeOpt.value() ) );
                    }
                }
            }
            
            //determine the using types
            std::vector< ::eg::interface::Using* > usings;
            pAction->getUsings( usings );
            for( ::eg::interface::Using* pUsing : usings )
            {
                QualType typeType = getTypeTrait( pASTContext, pSema, result.pContext, result.loc, pUsing->getIdentifier() );
                        
                QualType typeTypeCanonical = typeType.getCanonicalType();
                
                AbstractMutator::setCanonicalType( *pUsing, typeTypeCanonical.getAsString() );
            }
            
            const std::size_t szBaseCount = pAction->getBaseCount();
            for( std::size_t sz = 0; sz != szBaseCount; ++sz )
            {
                const std::string strBaseType = ::eg::getBaseTraitType( sz );
                
                //determine the link types
                DeclLocType linkResult = getNestedDeclContext( pASTContext, pSema,
                    result.pContext, result.loc, strBaseType, true );
                if( linkResult.pContext )
                {
                    //attempt to get the Type Alias
                    QualType typeType = getTypeTrait( pASTContext, pSema, linkResult.pContext, linkResult.loc, "Type" );
                    
                    QualType typeTypeCanonical = typeType.getCanonicalType();
                    
                    if( std::optional< eg::TypeID > iLinkEGTypeIDOpt = getEGTypeID( pASTContext, typeTypeCanonical ) )
                    {
                        const eg::TypeID iLinkEGTypeID = iLinkEGTypeIDOpt.value();
                        if( iLinkEGTypeID > 0 && iLinkEGTypeID < static_cast< ::eg::TypeID >( objects.size() ) )
                        {
                            ::eg::IndexedObject* pObject = objects[ iLinkEGTypeID ];
                            if( ::eg::interface::Action* pLinkedAction = dynamic_cast< ::eg::interface::Action* >( pObject ) )
                            {
                                AbstractMutator::setInherited( *pAction, pLinkedAction );
                            }
                        }
                        else
                        {
                            //error invalid inheritance type
                            std::ostringstream os;
                            os << "Invalid inheritance type for " << pAction->getFriendlyName() << " of " << typeTypeCanonical.getAsString();
                            if( iLinkEGTypeID < 0 )
                            {
                                const eg::TypeID negativeType = -iLinkEGTypeID;
                                if( negativeType > 0 && negativeType < static_cast< ::eg::TypeID >( objects.size() ) )
                                {
                                    if( ::eg::interface::Action* pShouldBeAction = 
                                        dynamic_cast< ::eg::interface::Action* >( objects[ negativeType ] ) )
                                    {
                                        os << " should it be " << pShouldBeAction->getFriendlyName() << "?";
                                    }
                                }
                            }
                            pASTContext->getDiagnostics().Report( clang::diag::err_eg_generic_error ) << os.str();
                            return false;
                        }
                    }
                    else
                    {
                        //report object mapping type
                        const std::string strCanonical = typeTypeCanonical.getAsString();
                        if( !strCanonical.empty() )
                        {
                            AbstractMutator::setInherited( *pAction, strCanonical );
                        
                            //report the dependency
                            {
                                QualType typeType = getTypeTrait( pASTContext, pSema, linkResult.pContext, linkResult.loc, "Dependency" );
                                QualType typeTypeCanonical = typeType.getCanonicalType();
                                const std::string strCanonical = typeTypeCanonical.getAsString();
                                AbstractMutator::setDependency( *pAction, strCanonical );
                            }
                        }
                    }
                }
            }
            //recurse
            {
                std::vector< eg::interface::Action* > actions;
                pAction->getChildActions( actions );
                for( eg::interface::Action* pChildAction : actions )
                {
                    if( !interfaceAnalysis( pASTContext, pSema, session, pChildAction, result.loc, result.pContext ) )
                        return false;
                }
            }
        }
        return true;
    }
    
    bool interfaceAnalysis( ASTContext* pASTContext, Sema* pSema, eg::InterfaceSession& session )
    {
        eg::interface::Root* pRoot = session.getTreeRoot();
        
        std::vector< eg::interface::Action* > actions;
        pRoot->getChildActions( actions );
        
        SourceLocation loc;
        DeclContext* pContext = pASTContext->getTranslationUnitDecl();
        
        for( eg::interface::Action* pAction : actions )
        {
            if( !interfaceAnalysis( pASTContext, pSema, session, pAction, loc, pContext ) )
                return false;
        }
        return true;
    }
    
    bool operationsAnalysis( ASTContext* pASTContext, Sema* pSema, eg::OperationsSession& session )
    {
        return true;
    }
}
