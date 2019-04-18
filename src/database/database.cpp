

#include "database/database.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/AST/Type.h"

#pragma warning( pop ) 

#include <sstream>

namespace
{
    enum Mode
    {
        eInterface,
        eOperations,
        eImplementation
    };
    Mode currentMode;
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
    }
    EGDB_EXPORT void initialiseMode_Interface( const char* strDatabasePath )
    {
        currentMode = eInterface;
    }
    EGDB_EXPORT void initialiseMode_Operations( const char* strDatabasePath, 
        const char* strTranslationUnitDatabasePath, unsigned uiTranslationUnitID )
    {
        currentMode = eOperations;
    }
    EGDB_EXPORT void initialiseMode_Implementation()
    {
        currentMode = eImplementation;
    }
    EGDB_EXPORT void runFinalAnalysis()
    {
    }

    EGDB_EXPORT bool isEGEnabled()
    {
        return true;
    }
    EGDB_EXPORT bool isEGType( const clang::QualType& type )
    {
        return false;
    }
    EGDB_EXPORT bool isPossibleEGType( const clang::QualType& type )
    {
        return false;
    }
    EGDB_EXPORT bool isPossibleEGTypeIdentifier( const clang::Token& token )
    {
        return false;
    }

    EGDB_EXPORT void getInvocationOperationType( const clang::QualType& typePathType, bool bHasArguments, clang::QualType& operationType )
    {
        
    }
    
    EGDB_EXPORT void getInvocationResultType( const clang::QualType& baseType, clang::QualType& resultType )
    {
          //QualType resultType = BaseType;
          //if( !BaseType->isDependentType() )
          //{
          //}
    
              //const IdentifierInfo* pBaseTypeID = BaseType.getBaseTypeIdentifier();
              //if( pBaseTypeID == Context.getEGInvocationTypeName() )
              //{
              //      assert( pDatabase );
              //  
              //      const TemplateSpecializationType* pTemplateType = 
              //          BaseType->getAs<TemplateSpecializationType>();
              //          
              //      assert( pTemplateType );
              //      assert( pTemplateType->getNumArgs() == 3U );
              //      
              //      resultType = pDatabase->getInvocationResultType(
              //          pTemplateType->getArg( 0U ).getAsType(),
              //          pTemplateType->getArg( 1U ).getAsType(),
              //          pTemplateType->getArg( 2U ).getAsType() );  
              //}
              //else
              //{
              //    assert( false );
              //    llvm_unreachable("invalid eg result type");
              //}
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

        void eg_getInvocationOperationType( const QualType& typePathType, bool bHasArguments, QualType& operationType )
        {
            
        }
        
        void eg_getInvocationResultType( const QualType& baseType, QualType& resultType )
        {
        }

    }
}