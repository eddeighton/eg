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


#ifndef DATABASE_HEADER_15_04_2019
#define DATABASE_HEADER_15_04_2019


#ifdef EG_DATABASE_SHARED_MODULE
    #define EGDB_EXPORT __declspec( dllexport )
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
    EGDB_EXPORT int isPossibleEGTypeIdentifierDecl( const clang::Token& token, bool bIsTypePathParsing );
    EGDB_EXPORT void getInvocationOperationType( const clang::QualType& typePathType, bool bHasArguments, clang::QualType& operationType );
    EGDB_EXPORT void getInvocationResultType( const clang::QualType& baseType, clang::QualType& resultType );
}




#endif //DATABASE_HEADER_15_04_2019