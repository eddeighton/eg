
#ifndef DATABASE_HEADER_15_04_2019
#define DATABASE_HEADER_15_04_2019


#ifdef EG_DATABASE_SHARED_MODULE
    #define EGDB_EXPORT __declspec( dllexport )
#elif EG_DATABASE_STATIC_LIB
    #define EGDB_EXPORT
#else
    #define EGDB_EXPORT __declspec( dllimport )
#endif


namespace clang
{
    class QualType;
    class ASTContext;
    class Sema;
    class Token;
}

//EG Database Interface
namespace eg
{
    using EGChar = char;
    
    EGDB_EXPORT const EGChar* getTypePathString();
    EGDB_EXPORT const EGChar* getInvocationString();
    EGDB_EXPORT const EGChar* getVariantString();
    EGDB_EXPORT const EGChar* getInvokeString();
    EGDB_EXPORT const EGChar* getResultTypeTrait();

    EGDB_EXPORT void initialise( clang::ASTContext* pASTContext, clang::Sema* pSema );
    EGDB_EXPORT void initialiseMode_Interface( const EGChar* strDatabasePath );
    EGDB_EXPORT void initialiseMode_Operations( const EGChar* strDatabasePath, 
        const EGChar* strTranslationUnitDatabasePath, unsigned uiTranslationUnitID );
    EGDB_EXPORT void initialiseMode_Implementation();
    EGDB_EXPORT void runFinalAnalysis();

    EGDB_EXPORT bool isEGEnabled();
    EGDB_EXPORT bool isEGType( const clang::QualType& type );
    EGDB_EXPORT bool isPossibleEGType( const clang::QualType& type );
    EGDB_EXPORT bool isPossibleEGTypeIdentifier( const clang::Token& token );
    EGDB_EXPORT void getInvocationOperationType( const clang::QualType& typePathType, bool bHasArguments, clang::QualType& operationType );
    EGDB_EXPORT void getInvocationResultType( const clang::QualType& baseType, clang::QualType& resultType );
}




#endif //DATABASE_HEADER_15_04_2019