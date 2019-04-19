
#include "clang_utils.hpp"

#include "eg.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Lex/Token.h"
#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"
#include "clang/AST/Type.h"
#include "clang/AST/ASTContext.h"
#include "clang/Sema/Ownership.h"
#include "clang/Sema/ParsedTemplate.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/Lookup.h"

#pragma warning( pop ) 

namespace clang
{

    std::optional< eg::EGTypeID > getEGTypeID( QualType type )
    {
        std::optional< eg::EGTypeID > result;
        
        if( type.getTypePtrOrNull() && !type->isDependentType() )
        {
            QualType canonicalType = type.getCanonicalType();
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
    const IdentifierInfo* getReadOperation( ASTContext* pASTContext )
    {
        return getOperationIdentifier( pASTContext, "Read" );
    }
    const IdentifierInfo* getWriteOperation( ASTContext* pASTContext )
    {
        return getOperationIdentifier( pASTContext, "Write" );
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
                    assert( false );
                }
            }
            else
            {
                return nullptr;
            }
        }
        
        if( ::eg::isOperationName( pBaseTypeID->getName() ) )
        {
            return getOperationIdentifier( pASTContext, pBaseTypeID->getName() );
        }
        else if( bHasParameters )
        {
            return getWriteOperation( pASTContext );
        }
        else
        {
            return getReadOperation( pASTContext );
        }
    }
    
    bool getContextTypes( ASTContext* pASTContext, QualType contextType, std::vector< eg::EGTypeID >& contextTypes )
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
                if( std::optional< eg::EGTypeID > egTypeID = getEGTypeID( canonicalType ) )
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
    
    bool getTypePathTypes( ASTContext* pASTContext, QualType typePath, std::vector< eg::EGTypeID >& typePathTypes)
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
            if( std::optional< eg::EGTypeID > egTypeID = getEGTypeID( canonicalType ) )
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
}
