

#include "clang_plugin/clang_plugin.hpp"

#include "eg/interface_session.hpp"
#include "eg/operations_session.hpp"
#include "eg/identifiers.hpp"
#include "eg/eg.hpp"
#include "eg/codegen.hpp"

#include "common/backtrace.hpp"

#include "clang_utils.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "llvm/Support/Casting.h"

#include "clang/Lex/Token.h"
#include "clang/AST/Type.h"
#include "clang/AST/ASTContext.h"
#include "clang/Sema/Ownership.h"
#include "clang/Sema/ParsedTemplate.h"
#include "clang/Sema/Sema.h"
#include "clang/Sema/Lookup.h"


#pragma warning( pop ) 

#include <sstream>

namespace
{
    enum Mode
    {
        eInterface,
        eOperations,
        eImplementation,
        eOther
    };
    Mode currentMode = eOther;
    
    clang::ASTContext* g_pASTContext = nullptr;
    clang::Sema* g_pSema = nullptr;
    eg::InterfaceSession* g_pInterfaceSession = nullptr;
    eg::OperationsSession* g_pOperationsSession = nullptr;
}

namespace eg
{
    EGDB_EXPORT const EGChar* getTypePathString()
    {
        return "__eg_type_path";
    }
    EGDB_EXPORT const EGChar* getInvocationString()
    {
        return "__eg_invocation";
    }
    EGDB_EXPORT const EGChar* getVariantString()
    {
        return "__eg_variant";
    }
    EGDB_EXPORT const EGChar* getInvokeString()
    {
        return "invoke";
    }
    EGDB_EXPORT const EGChar* getResultTypeTrait()
    {
        return "__eg_result_type";
    }
    
    EGDB_EXPORT void initialise( clang::ASTContext* pASTContext, clang::Sema* pSema )
    {
        g_pASTContext = pASTContext;
        g_pSema = pSema;
    }
    EGDB_EXPORT void initialiseMode_Interface( const char* strDatabasePath )
    {
        currentMode = eInterface;
        g_pInterfaceSession = new InterfaceSession( std::string( strDatabasePath ) );
    }
    EGDB_EXPORT void initialiseMode_Operations( const char* strDatabasePath, 
        const char* strTranslationUnitDatabasePath, unsigned uiTranslationUnitID )
    {
        //Common::debug_break();
        
        currentMode = eOperations;
        g_pOperationsSession = new OperationsSession( 
            std::string( strDatabasePath ), 
            std::string( strTranslationUnitDatabasePath ), uiTranslationUnitID );
    }
    EGDB_EXPORT void initialiseMode_Implementation()
    {
        currentMode = eImplementation;
    }
    EGDB_EXPORT void runFinalAnalysis()
    {
        switch( currentMode )
        {
            case eInterface:
                if( g_pInterfaceSession )
                {
                    clang::interfaceAnalysis( g_pASTContext, g_pSema, *g_pInterfaceSession );
                    g_pInterfaceSession->store();
                    delete g_pInterfaceSession;
                }
                break;
            case eOperations:
                if( g_pOperationsSession )
                {
                    clang::operationsAnalysis( g_pASTContext, g_pSema, *g_pOperationsSession );
                    g_pOperationsSession->store();
                    delete g_pOperationsSession;
                }
                break;
            case eImplementation:
                break;
            case eOther:
            default:
                break;
        }
    }

    EGDB_EXPORT bool isEGEnabled()
    {
        switch( currentMode )
        {
            case eInterface:
            case eOperations:
                return true;
            case eImplementation:
            case eOther:
            default:
                return false;
        }
    }
    EGDB_EXPORT bool isEGType( const clang::QualType& type )
    {
        if( clang::getEGTypeID( g_pASTContext, type ) )
            return true;
        else
            return false;
    }
    EGDB_EXPORT bool isPossibleEGType( const clang::QualType& type )
    {
        if( type.getTypePtrOrNull()  )
        {
            if( type->isDependentType() )
            {
                return true;
            }
            else
            {
                if( clang::getEGTypeID( g_pASTContext, type ) )
                    return true;
                else
                    return false;
            }
        }
        else
        {
            return false;
        }
    }
    EGDB_EXPORT bool isPossibleEGTypeIdentifier( const clang::Token& token )
    {
        if( token.is( clang::tok::identifier ) )
        {
            const clang::IdentifierInfo* pIdentifierInfo = token.getIdentifierInfo();
            if( getOperationName( pIdentifierInfo->getName() ) != HIGHEST_OPERATION_TYPE )
            {
                return true;
            }
            
            const eg::Identifiers* pIdentifiers = nullptr;
            if( g_pInterfaceSession )
            {
                pIdentifiers = one_cst< eg::Identifiers >( g_pInterfaceSession->getAppendingObjects() );
            }
            else if( g_pOperationsSession )
            {
                pIdentifiers = one_cst< eg::Identifiers >( g_pOperationsSession->getMaster() );
            }
            if( pIdentifiers )
            {
                if( pIdentifiers->isElement( pIdentifierInfo->getName() ) )
                {
                    return true;
                }
            }
        }
        else if( token.is( clang::tok::annot_template_id ) )
        {
            const clang::TemplateIdAnnotation *Annot =
                static_cast< const clang::TemplateIdAnnotation *>( 
                    token.getAnnotationValue() );
        }
        else if( token.is( clang::tok::annot_typename ) )
        {
            clang::ParsedType parsedType = clang::ParsedType::getFromOpaquePtr( token.getAnnotationValue() );
            return isPossibleEGType( g_pSema->GetTypeFromParser( parsedType ) );
        }
        else
        {
            
        }
        
        return false;
    }

    EGDB_EXPORT int isPossibleEGTypeIdentifierDecl( const clang::Token& token, bool bIsTypePathParsing )
    {
        bool bResult = false;
        if( token.is( clang::tok::identifier ) )
        {
            const clang::IdentifierInfo* pIdentifierInfo = token.getIdentifierInfo();
            if( getOperationName( pIdentifierInfo->getName() ) != HIGHEST_OPERATION_TYPE )
            {
                bResult = true;
            }
            else
            {
            
                const eg::Identifiers* pIdentifiers = nullptr;
                if( g_pInterfaceSession )
                {
                    pIdentifiers = one_cst< eg::Identifiers >( g_pInterfaceSession->getAppendingObjects() );
                }
                else if( g_pOperationsSession )
                {
                    pIdentifiers = one_cst< eg::Identifiers >( g_pOperationsSession->getMaster() );
                }
                if( pIdentifiers )
                {
                    if( pIdentifiers->isElement( pIdentifierInfo->getName() ) )
                    {
                        bResult = true;
                    }
                }
            }
        }
        else if( token.is( clang::tok::annot_template_id ) )
        {
            const clang::TemplateIdAnnotation *Annot =
                static_cast< const clang::TemplateIdAnnotation *>( 
                    token.getAnnotationValue() );
        }
        else if( token.is( clang::tok::annot_typename ) )
        {
            clang::ParsedType parsedType = clang::ParsedType::getFromOpaquePtr( token.getAnnotationValue() );
            bResult = isPossibleEGType( g_pSema->GetTypeFromParser( parsedType ) );
        }
        else
        {
            
        }
        
        if( bResult )
        {
            if( !bIsTypePathParsing )
            {
                return 0;
            }
            else
            {
                return 1;
            }
        }
        else 
        {
            return 2; //ambiguous
        }
    }
        
    EGDB_EXPORT void getInvocationOperationType( const clang::QualType& typePathType, bool bHasArguments, clang::QualType& operationType )
    {
        //determine the operation type...
        if( const clang::IdentifierInfo* pIdentifierInfo = 
            clang::getOperationID( g_pASTContext, typePathType, bHasArguments ) )
        {
            //return the type
            clang::TagDecl* operatorClassDecl = nullptr;
            {
                clang::LookupResult Result( *g_pSema, pIdentifierInfo, clang::SourceLocation(), clang::Sema::LookupOrdinaryName );
                if( g_pSema->LookupName( Result, g_pSema->getCurScope() ) )
                {
                    operatorClassDecl = llvm::dyn_cast< clang::TagDecl >( Result.getFoundDecl() );
                }
            }
            if( operatorClassDecl )
            {
                operationType = g_pASTContext->getTagDeclType( operatorClassDecl );
            }
            else
            {
                //diagnostic
            }
        }
        else
        {
            
        }
    }
    
    template< class T >
    void uniquify( std::vector< T >& ids )
    {
        std::sort( ids.begin(), ids.end() );
        auto last = std::unique( ids.begin(), ids.end() );
        ids.erase( last, ids.end() );
    }
    
    EGDB_EXPORT void getInvocationResultType( const clang::QualType& type, clang::QualType& resultType )
    {
        if( g_pOperationsSession )
        {
            if( type.getTypePtrOrNull()  )
            {
                if( !type->isDependentType() )
                {
                    if( const clang::IdentifierInfo* pBaseTypeID = type.getBaseTypeIdentifier() )
                    {
                        if( pBaseTypeID == g_pASTContext->getEGInvocationTypeName() )
                        {
                            if( const clang::TemplateSpecializationType* pTemplateType = 
                                type->getAs< clang::TemplateSpecializationType >() )
                            {
                                if( pTemplateType->getNumArgs() == 3U )
                                {
                                    clang::QualType context         = pTemplateType->getArg( 0U ).getAsType();
                                    clang::QualType typePath        = pTemplateType->getArg( 1U ).getAsType();
                                    clang::QualType operationType   = pTemplateType->getArg( 2U ).getAsType();
                                    
                                    std::vector< eg::TypeID > contextTypes;
                                    if( !clang::getContextTypes( g_pASTContext, context, contextTypes ) )
                                    {
                                        //diagnostic...
                                        return;
                                    }
                                    
                                    std::vector< eg::TypeID > typePathTypes;
                                    if( !clang::getTypePathTypes( g_pASTContext, typePath, typePathTypes ) )
                                    {
                                        //diagnostic...
                                        return;
                                    }
                                    
                                    std::optional< eg::TypeID > operationTypeIDOpt = getEGTypeID( g_pASTContext, operationType );
                                    if( !operationTypeIDOpt )
                                    {
                                        //diagnostic...
                                        return;
                                    }
                                    eg::TypeID operationTypeID = operationTypeIDOpt.value();
                                    
                                    std::vector< const abstract::Element* > contexts;
                                    {
                                        for( eg::TypeID typeID : contextTypes )
                                        {
                                            std::vector< const abstract::Element* > result =
                                                g_pOperationsSession->fromEGTypeID( typeID, false );
                                            std::copy( result.begin(), result.end(), std::back_inserter( contexts ) );
                                        }
                                        uniquify( contexts );
                                    }
                                    
                                    std::vector< std::vector< const abstract::Element* > > typePathElements;
                                    {
                                        for( eg::TypeID typeID : typePathTypes )
                                        {
                                            std::vector< const abstract::Element* > result =
                                                g_pOperationsSession->fromEGTypeID( typeID, true );
                                            if( !result.empty() )
                                                typePathElements.push_back( result );
                                        }
                                    }
                                    
                                    if( const InvocationSolution* pSolution = g_pOperationsSession->getInvocation( 
                                        contexts, typePathElements, operationTypeID, typePathTypes ) )
                                    {
                                        //establish the return type
                                        clang::DeclContext* pDeclContext = g_pASTContext->getTranslationUnitDecl();
                                        const InvocationSolution::TargetTypes& targets = pSolution->getTargetTypes();
                                        clang::SourceLocation loc;
                                        
                                        switch( operationTypeID )
                                        {
                                            case id_Imp_NoParams    :
                                            case id_Imp_Params      : 
                                                {
                                                    if( pSolution->isImplicitStarter() )
                                                    {
                                                        const abstract::Element* pTarget = targets.front();
                                                        clang::DeclContext* pDeclContextIter = pDeclContext;
                                                        const std::vector< const abstract::Element* > path = getPath( pTarget );
                                                        for( const abstract::Element* pElementElement : path )
                                                        {
                                                            if( pElementElement == path.back() )
                                                            {
                                                                resultType = clang::getType( g_pASTContext, g_pSema, 
                                                                    getInterfaceType( pElementElement->getIdentifier() ), "void", 
                                                                    pDeclContextIter, loc, true );
                                                            }
                                                            else
                                                            {
                                                                clang::getType( g_pASTContext, g_pSema, 
                                                                    getInterfaceType( pElementElement->getIdentifier() ), "void", 
                                                                    pDeclContextIter, loc, false );
                                                                if( !pDeclContextIter ) break;
                                                            }
                                                        }
                                                    }
                                                    else if( operationTypeID == id_Imp_NoParams )
                                                    {
                                                        const abstract::Element* pTarget = targets.front();
                                                        clang::DeclContext* pDeclContextIter = pDeclContext;
                                                        const std::vector< const abstract::Element* > path = getPath( pTarget );
                                                        for( const abstract::Element* pElementElement : path )
                                                        {
                                                            clang::getType( g_pASTContext, g_pSema, 
                                                                getInterfaceType( pElementElement->getIdentifier() ), "void", 
                                                                pDeclContextIter, loc, false );
                                                            if( !pDeclContextIter ) break;
                                                        }
                                                        if( pDeclContextIter )
                                                            resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "Read" );
                                                    }
                                                    else if( operationTypeID == id_Imp_Params )
                                                    {
                                                        resultType = clang::getVoidType( g_pASTContext );
                                                    }
                                                    else
                                                    {
                                                        //error
                                                    }
                                                }
                                                break;
                                            case id_Get        :
                                                {
                                                    if( targets.size() == 1 )
                                                    {
                                                        const abstract::Element* pTarget = targets.front();
                                                        clang::DeclContext* pDeclContextIter = pDeclContext;
                                                        const std::vector< const abstract::Element* > path = getPath( pTarget );
                                                        for( const abstract::Element* pElementElement : path )
                                                        {
                                                            clang::getType( g_pASTContext, g_pSema, 
                                                                getInterfaceType( pElementElement->getIdentifier() ), "void", 
                                                                pDeclContextIter, loc, false );
                                                            if( !pDeclContextIter ) break;
                                                        }
                                                        if( pDeclContextIter )
                                                            resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "Write" );
                                                    }
                                                }
                                                break;
                                            case id_Update     :
                                                {
                                                    resultType = clang::getVoidType( g_pASTContext );
                                                }
                                                break;
                                            case id_Old        : 
                                                {
                                                    if( targets.size() == 1 )
                                                    {
                                                        const abstract::Element* pTarget = targets.front();
                                                        clang::DeclContext* pDeclContextIter = pDeclContext;
                                                        const std::vector< const abstract::Element* > path = getPath( pTarget );
                                                        for( const abstract::Element* pElementElement : path )
                                                        {
                                                            clang::getType( g_pASTContext, g_pSema, 
                                                                getInterfaceType( pElementElement->getIdentifier() ), "void", 
                                                                pDeclContextIter, loc, false );
                                                            if( !pDeclContextIter ) break;
                                                        }
                                                        if( pDeclContextIter )
                                                            resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "Read" );
                                                    }
                                                }
                                                break;
                                            case id_Stop       : 
                                                {
                                                    resultType = clang::getVoidType( g_pASTContext );
                                                }
                                                break;
                                            case id_Pause      : 
                                                {
                                                    resultType = clang::getVoidType( g_pASTContext );
                                                }
                                                break;
                                            case id_Resume     : 
                                                {
                                                    resultType = clang::getVoidType( g_pASTContext );
                                                }
                                                break;
                                            case id_Defer      : 
                                                {
                                                }
                                                break;
                                            case id_Empty      : 
                                                {
                                                    resultType = clang::getBooleanType( g_pASTContext );
                                                }
                                                break;
                                            case id_Range      : 
                                                {
                                                    if( targets.size() == 1 )
                                                    {
                                                        const abstract::Element* pTarget = targets.front();
                                                        clang::DeclContext* pDeclContextIter = pDeclContext;
                                                        const std::vector< const abstract::Element* > path = getPath( pTarget );
                                                        for( const abstract::Element* pElementElement : path )
                                                        {
                                                            clang::getType( g_pASTContext, g_pSema, 
                                                                getInterfaceType( pElementElement->getIdentifier() ), "void", 
                                                                pDeclContextIter, loc, false );
                                                            if( !pDeclContextIter ) break;
                                                        }
                                                        if( pDeclContextIter )
                                                            resultType = clang::getTypeTrait( g_pASTContext, g_pSema, pDeclContextIter, loc, "EGRangeType" );
                                                    }
                                                }
                                                break;
                                            default:
                                                break;
                                        }
                                        
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        
        
    }
}


namespace clang 
{
    class ASTContext;
    class Sema;
    class QualType;
    class IdentifierInfo;

    namespace clang_eg 
    {
        void eg_initialise( ASTContext* pASTContext, Sema* pSema )
        {
        }
        void eg_initialiseMode_Interface( const char* strDatabasePath )
        {
        }
        void eg_initialiseMode_Operations( const char* strDatabasePath, 
            const char* strTranslationUnitDatabasePath, unsigned uiTranslationUnitID )
        {
        }
        void eg_initialiseMode_Implementation()
        {
        }
        void eg_runFinalAnalysis()
        {
        }

        const char* eg_getTypePathString()
        {
            return eg::getTypePathString();
        }
        const char* eg_getInvocationString()
        {
            return eg::getInvocationString();
        }
        const char* eg_getVariantString()
        {
            return eg::getVariantString();
        }
        const char* eg_getInvokeString()
        {
            return eg::getInvokeString();
        }
        const char* eg_getResultTypeTrait()
        {
            return eg::getResultTypeTrait();
        }

        bool eg_isEGEnabled()
        {
            return false;
        }
        bool eg_isEGType( const QualType& type )
        {
            return false;
        }
        bool eg_isPossibleEGType( const QualType& type )
        {
            return false;
        }
        bool eg_isPossibleEGTypeIdentifier( const Token& token )
        {
            return false;
        }
        int eg_isPossibleEGTypeIdentifierDecl( const Token& token, bool bIsTypePathParsing )
        {
            return false;
        }

        void eg_getInvocationOperationType( const QualType& typePathType, bool bHasArguments, QualType& operationType )
        {
            
        }
        
        void eg_getInvocationResultType( const QualType& baseType, QualType& resultType )
        {
        }

    }
}