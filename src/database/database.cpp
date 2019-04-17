

#include "database/database.hpp"

#include "clang/AST/Type.h"


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
    const EGChar* getTypePathString()
    {
        return "__eg_type_path";
    }
    const EGChar* getInvocationString()
    {
        return "__eg_invocation";
    }
    const EGChar* getVariantString()
    {
        return "__eg_variant";
    }
    const EGChar* getInvokeString()
    {
        return "invoke";
    }
    const EGChar* getResultTypeTrait()
    {
        return "__eg_result_type";
    }
    
    
    void initialise( clang::ASTContext* pASTContext, clang::Sema* pSema )
    {
    }
    void initialiseMode_Interface( const char* strDatabasePath )
    {
        currentMode = eInterface;
    }
    void initialiseMode_Operations( const char* strDatabasePath, 
        const char* strTranslationUnitDatabasePath, unsigned uiTranslationUnitID )
    {
        currentMode = eOperations;
    }
    void initialiseMode_Implementation()
    {
        currentMode = eImplementation;
    }
    void runFinalAnalysis()
    {
    }

    bool isEGEnabled()
    {
        return true;
    }
    bool isEGType( const clang::QualType& type )
    {
        return false;
    }
    bool isPossibleEGType( const clang::QualType& type )
    {
        return false;
    }
    bool isPossibleEGTypeIdentifier( const clang::Token& token )
    {
        return false;
    }

    void getInvocationOperationType( const clang::QualType& typePathType, bool bHasArguments, clang::QualType& operationType )
    {
        
    }
    
    void getInvocationResultType( const clang::QualType& baseType, clang::QualType& resultType )
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