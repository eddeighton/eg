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



#include "parser_session.hpp"

#include "../input.hpp"
#include "../interface.hpp"
#include "../identifiers.hpp"

//disable clang warnings
#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Basic/FileManager.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/BitmaskEnum.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/MemoryBufferCache.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Lex/Token.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/HeaderSearchOptions.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/ModuleLoader.h"
#include "clang/Lex/PreprocessorOptions.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseDiagnostic.h"

#pragma warning( pop ) 

#include <boost/algorithm/string.hpp>

namespace eg
{

#define EG_PARSER_ERROR( msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        Diags->Report( Tok.getLocation(), clang::diag::err_eg_generic_error ) << msg;\
        std::ostringstream _os; \
        _os << msg; \
        THROW_RTE( "Parser error: " << _os.str() );)


    //cannibalised version of clang parser for parsing eg source code
    class Parser
    {
        clang::Preprocessor &PP;
        
        clang::SourceManager& sm;
        
        clang::LangOptions& languageOptions;
        
        clang::HeaderSearch& headerSearch;

        llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > Diags;

        clang::DiagnosticBuilder Diag( clang::SourceLocation Loc, unsigned DiagID )
        {
            return Diags->Report( Loc, DiagID );
        }
        clang::DiagnosticBuilder Diag( const clang::Token &Tok, unsigned DiagID )
        {
            return Diag( Tok.getLocation(), DiagID );
        }
        clang::DiagnosticBuilder Diag( unsigned DiagID )
        {
            return Diag( Tok, DiagID );
        }
        
        bool getSourceText( clang::SourceLocation startLoc, clang::SourceLocation endLoc, std::string& str )
        {
            bool bInvalid = false;
            const clang::SourceRange range( startLoc, endLoc );
            clang::CharSourceRange charRange = clang::CharSourceRange::getCharRange( range );
            str = clang::Lexer::getSourceText( charRange, sm, languageOptions, &bInvalid );
            //sort out carriage returns
            boost::replace_all( str, "\r\n", "\n" );
            
            return !bInvalid;
        }
        

        /// Tok - The current token we are peeking ahead.  All parsing methods assume
        /// that this is valid.
        clang::Token Tok;

        // PrevTokLocation - The location of the token we previously
        // consumed. This token is used for diagnostics where we expected to
        // see a token following another token (e.g., the ';' at the end of
        // a statement).
        clang::SourceLocation PrevTokLocation;

        unsigned short ParenCount = 0, BracketCount = 0, BraceCount = 0;
        unsigned short MisplacedModuleBeginCount = 0;

        std::vector< unsigned short > braceStack;
        /// ScopeCache - Cache scopes to reduce malloc traffic.
        //enum { ScopeCacheSize = 16 };
        //unsigned NumCachedScopes;
        //Scope *ScopeCache[ScopeCacheSize];

        struct AngleBracketTracker
        {
            /// Flags used to rank candidate template names when there is more than one
            /// '<' in a scope.
            enum Priority : unsigned short
            {
                /// A non-dependent name that is a potential typo for a template name.
                PotentialTypo = 0x0,
                /// A dependent name that might instantiate to a template-name.
                DependentName = 0x2,

                /// A space appears before the '<' token.
                SpaceBeforeLess = 0x0,
                /// No space before the '<' token
                NoSpaceBeforeLess = 0x1,

                LLVM_MARK_AS_BITMASK_ENUM(/*LargestValue*/ DependentName )
            };

            struct Loc
            {
                clang::SourceLocation LessLoc;
                AngleBracketTracker::Priority Priority;
                unsigned short ParenCount, BracketCount, BraceCount;

                bool isActive( Parser &P ) const
                {
                    return P.ParenCount == ParenCount && P.BracketCount == BracketCount &&
                        P.BraceCount == BraceCount;
                }

                bool isActiveOrNested( Parser &P ) const
                {
                    return isActive( P ) || P.ParenCount > ParenCount ||
                        P.BracketCount > BracketCount || P.BraceCount > BraceCount;
                }
            };

            llvm::SmallVector< Loc, 8> Locs;

            /// Add an expression that might have been intended to be a template name.
            /// In the case of ambiguity, we arbitrarily select the innermost such
            /// expression, for example in 'foo < bar < baz', 'bar' is the current
            /// candidate. No attempt is made to track that 'foo' is also a candidate
            /// for the case where we see a second suspicious '>' token.
            void add( Parser &P, clang::SourceLocation LessLoc, Priority Prio )
            {
                if( !Locs.empty() && Locs.back().isActive( P ) )
                {
                    if( Locs.back().Priority <= Prio )
                    {
                        Locs.back().LessLoc = LessLoc;
                        Locs.back().Priority = Prio;
                    }
                }
                else
                {
                    Locs.push_back( { LessLoc, Prio,
                                    P.ParenCount, P.BracketCount, P.BraceCount } );
                }
            }

            /// Mark the current potential missing template location as having been
            /// handled (this happens if we pass a "corresponding" '>' or '>>' token
            /// or leave a bracket scope).
            void clear( Parser &P )
            {
                while( !Locs.empty() && Locs.back().isActiveOrNested( P ) )
                    Locs.pop_back();
            }

            /// Get the current enclosing expression that might hve been intended to be
            /// a template name.
            Loc *getCurrent( Parser &P )
            {
                if( !Locs.empty() && Locs.back().isActive( P ) )
                    return &Locs.back();
                return nullptr;
            }
        };
        AngleBracketTracker AngleBrackets;

    public:
        Parser( clang::Preprocessor& PP, 
                clang::SourceManager& sourceManager,
                clang::LangOptions& languageOptions,
                clang::HeaderSearch& headerSearch,
                llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > Diags )
            :   PP( PP ),
                sm( sourceManager ),
                languageOptions( languageOptions ),
                headerSearch( headerSearch ),
                Diags( Diags )
        {
            PP.EnterMainSourceFile();

        }

        const clang::Token &getCurToken() const { return Tok; }

        const clang::Token &NextToken()
        {
            return PP.LookAhead( 0 );
        }

        clang::SourceLocation ConsumeToken()
        {
            assert( !isTokenSpecial() &&
                    "Should consume special tokens with Consume*Token" );
            PrevTokLocation = Tok.getLocation();
            PP.Lex( Tok );
            return PrevTokLocation;
        }

        bool TryConsumeToken( clang::tok::TokenKind Expected )
        {
            if( Tok.isNot( Expected ) )
                return false;
            assert( !isTokenSpecial() &&
                    "Should consume special tokens with Consume*Token" );
            PrevTokLocation = Tok.getLocation();
            PP.Lex( Tok );
            return true;
        }

        clang::SourceLocation ConsumeAnyToken( bool ConsumeCodeCompletionTok = false )
        {
            if( isTokenParen() )
                return ConsumeParen();
            if( isTokenBracket() )
                return ConsumeBracket();
            if( isTokenBrace() )
                return ConsumeBrace();
            if( isTokenStringLiteral() )
                return ConsumeStringToken();
            //if (Tok.is(clang::tok::code_completion))
            //  return ConsumeCodeCompletionTok ? ConsumeCodeCompletionToken()
            //                                  : handleUnexpectedCodeCompletionToken();
            if( Tok.isAnnotation() )
                return ConsumeAnnotationToken();
            return ConsumeToken();
        }

        bool isTokenParen() const
        {
            return Tok.isOneOf( clang::tok::l_paren, clang::tok::r_paren );
        }
        /// isTokenBracket - Return true if the cur token is '[' or ']'.
        bool isTokenBracket() const
        {
            return Tok.isOneOf( clang::tok::l_square, clang::tok::r_square );
        }
        /// isTokenBrace - Return true if the cur token is '{' or '}'.
        bool isTokenBrace() const
        {
            return Tok.isOneOf( clang::tok::l_brace, clang::tok::r_brace );
        }
        /// isTokenStringLiteral - True if this token is a string-literal.
        bool isTokenStringLiteral() const
        {
            return clang::tok::isStringLiteral( Tok.getKind() );
        }
        /// isTokenSpecial - True if this token requires special consumption methods.
        bool isTokenSpecial() const
        {
            return isTokenStringLiteral() || isTokenParen() || isTokenBracket() ||
                isTokenBrace() || Tok.is( clang::tok::code_completion ) || Tok.isAnnotation();
        }
        void UnconsumeToken( clang::Token &Consumed )
        {
            clang::Token Next = Tok;
            PP.EnterToken( Consumed );
            PP.Lex( Tok );
            PP.EnterToken( Next );
        }

        clang::SourceLocation ConsumeAnnotationToken()
        {
            assert( Tok.isAnnotation() && "wrong consume method" );
            clang::SourceLocation Loc = Tok.getLocation();
            PrevTokLocation = Tok.getAnnotationEndLoc();
            PP.Lex( Tok );
            return Loc;
        }

        /// ConsumeParen - This consume method keeps the paren count up-to-date.
        ///
        clang::SourceLocation ConsumeParen()
        {
            assert( isTokenParen() && "wrong consume method" );
            if( Tok.getKind() == clang::tok::l_paren )
                ++ParenCount;
            else if( ParenCount )
            {
                AngleBrackets.clear( *this );
                --ParenCount;       // Don't let unbalanced )'s drive the count negative.
            }
            PrevTokLocation = Tok.getLocation();
            PP.Lex( Tok );
            return PrevTokLocation;
        }

        /// ConsumeBracket - This consume method keeps the bracket count up-to-date.
        ///
        clang::SourceLocation ConsumeBracket()
        {
            assert( isTokenBracket() && "wrong consume method" );
            if( Tok.getKind() == clang::tok::l_square )
                ++BracketCount;
            else if( BracketCount )
            {
                AngleBrackets.clear( *this );
                --BracketCount;     // Don't let unbalanced ]'s drive the count negative.
            }

            PrevTokLocation = Tok.getLocation();
            PP.Lex( Tok );
            return PrevTokLocation;
        }

        /// ConsumeBrace - This consume method keeps the brace count up-to-date.
        ///
        clang::SourceLocation ConsumeBrace()
        {
            assert( isTokenBrace() && "wrong consume method" );
            if( Tok.getKind() == clang::tok::l_brace )
                ++BraceCount;
            else if( BraceCount )
            {
                AngleBrackets.clear( *this );
                --BraceCount;     // Don't let unbalanced }'s drive the count negative.
            }

            PrevTokLocation = Tok.getLocation();
            PP.Lex( Tok );
            return PrevTokLocation;
        }

        /// ConsumeStringToken - Consume the current 'peek token', lexing a new one
        /// and returning the token kind.  This method is specific to strings, as it
        /// handles string literal concatenation, as per C99 5.1.1.2, translation
        /// phase #6.
        clang::SourceLocation ConsumeStringToken()
        {
            assert( isTokenStringLiteral() &&
                    "Should only consume string literals with this method" );
            PrevTokLocation = Tok.getLocation();
            PP.Lex( Tok );
            return PrevTokLocation;
        }

        /// Consume the current code-completion token.
        ///
        /// This routine can be called to consume the code-completion token and
        /// continue processing in special cases where \c cutOffParsing() isn't
        /// desired, such as token caching or completion with lookahead.
        clang::SourceLocation ConsumeCodeCompletionToken()
        {
            assert( Tok.is( clang::tok::code_completion ) );
            PrevTokLocation = Tok.getLocation();
            PP.Lex( Tok );
            return PrevTokLocation;
        }

        bool isEofOrEom()
        {
            clang::tok::TokenKind Kind = Tok.getKind();
            return Kind == clang::tok::eof || Kind == clang::tok::annot_module_begin ||
                Kind == clang::tok::annot_module_end || Kind == clang::tok::annot_module_include;
        }

        /// Identifiers which have been declared within a tentative parse.
        llvm::SmallVector< clang::IdentifierInfo*, 8> TentativelyDeclaredIdentifiers;

        class TentativeParsingAction
        {
            Parser &P;
            clang::Token PrevTok;
            size_t PrevTentativelyDeclaredIdentifierCount;
            unsigned short PrevParenCount, PrevBracketCount, PrevBraceCount;
            bool isActive;

        public:
            explicit TentativeParsingAction( Parser& p ) : P( p )
            {
                PrevTok = P.Tok;
                PrevTentativelyDeclaredIdentifierCount =
                    P.TentativelyDeclaredIdentifiers.size();
                PrevParenCount = P.ParenCount;
                PrevBracketCount = P.BracketCount;
                PrevBraceCount = P.BraceCount;
                P.PP.EnableBacktrackAtThisPos();
                isActive = true;
            }
            void Commit()
            {
                assert( isActive && "Parsing action was finished!" );
                P.TentativelyDeclaredIdentifiers.resize(
                    PrevTentativelyDeclaredIdentifierCount );
                P.PP.CommitBacktrackedTokens();
                isActive = false;
            }
            void Revert()
            {
                assert( isActive && "Parsing action was finished!" );
                P.PP.Backtrack();
                P.Tok = PrevTok;
                P.TentativelyDeclaredIdentifiers.resize(
                    PrevTentativelyDeclaredIdentifierCount );
                P.ParenCount = PrevParenCount;
                P.BracketCount = PrevBracketCount;
                P.BraceCount = PrevBraceCount;
                isActive = false;
            }
            ~TentativeParsingAction()
            {
                assert( !isActive && "Forgot to call Commit or Revert!" );
            }
        };
        /// A TentativeParsingAction that automatically reverts in its destructor.
        /// Useful for disambiguation parses that will always be reverted.
        class RevertingTentativeParsingAction
            : private Parser::TentativeParsingAction
        {
        public:
            RevertingTentativeParsingAction( Parser &P )
                : Parser::TentativeParsingAction( P ) {}
            ~RevertingTentativeParsingAction() { Revert(); }
        };

        class GreaterThanIsOperatorScope
        {
            bool &GreaterThanIsOperator;
            bool OldGreaterThanIsOperator;
        public:
            GreaterThanIsOperatorScope( bool &GTIO, bool Val )
                : GreaterThanIsOperator( GTIO ), OldGreaterThanIsOperator( GTIO )
            {
                GreaterThanIsOperator = Val;
            }

            ~GreaterThanIsOperatorScope()
            {
                GreaterThanIsOperator = OldGreaterThanIsOperator;
            }
        };

        bool GreaterThanIsOperator;
        class BalancedDelimiterTracker : public GreaterThanIsOperatorScope
        {
            Parser& P;
            clang::tok::TokenKind Kind, Close, FinalToken;
            clang::SourceLocation ( Parser::*Consumer )( );
            clang::SourceLocation LOpen, LClose;

            unsigned short &getDepth()
            {
                switch( Kind )
                {
                    case clang::tok::l_brace: return P.BraceCount;
                    case clang::tok::l_square: return P.BracketCount;
                    case clang::tok::l_paren: return P.ParenCount;
                    default: llvm_unreachable( "Wrong token kind" );
                }
            }

            //bool diagnoseOverflow();
            bool diagnoseMissingClose()
            {
                THROW_RTE( "diagnoseMissingClose" );
            }

        public:
            BalancedDelimiterTracker( Parser& p, clang::tok::TokenKind k,
                                      clang::tok::TokenKind FinalToken = clang::tok::semi )
                : GreaterThanIsOperatorScope( p.GreaterThanIsOperator, true ),
                P( p ), Kind( k ), FinalToken( FinalToken )
            {
                switch( Kind )
                {
                    default: llvm_unreachable( "Unexpected balanced token" );
                    case clang::tok::l_brace:
                        Close = clang::tok::r_brace;
                        Consumer = &Parser::ConsumeBrace;
                        break;
                    case clang::tok::l_paren:
                        Close = clang::tok::r_paren;
                        Consumer = &Parser::ConsumeParen;
                        break;

                    case clang::tok::l_square:
                        Close = clang::tok::r_square;
                        Consumer = &Parser::ConsumeBracket;
                        break;
                }
            }

            clang::SourceLocation getOpenLocation() const { return LOpen; }
            clang::SourceLocation getCloseLocation() const { return LClose; }
            clang::SourceRange getRange() const { return clang::SourceRange( LOpen, LClose ); }

            bool consumeOpen()
            {
                if( !P.Tok.is( Kind ) )
                    return true;

                //if (getDepth() < P.getLangOpts().BracketDepth) 
                {
                    LOpen = ( P.*Consumer )( );
                    return false;
                }

                //return diagnoseOverflow();
            }

            bool expectAndConsume( unsigned DiagID = clang::diag::err_expected,
                                   const char *Msg = "",
                                   clang::tok::TokenKind SkipToTok = clang::tok::unknown );
            bool consumeClose()
            {
                if( P.Tok.is( Close ) )
                {
                    LClose = ( P.*Consumer )( );
                    return false;
                }
                else if( P.Tok.is( clang::tok::semi ) && P.NextToken().is( Close ) )
                {
                    clang::SourceLocation SemiLoc = P.ConsumeToken();
                    P.Diag( SemiLoc, clang::diag::err_unexpected_semi )
                        << Close << clang::FixItHint::CreateRemoval( clang::SourceRange( SemiLoc, SemiLoc ) );
                    LClose = ( P.*Consumer )( );
                    return false;
                }

                return diagnoseMissingClose();
            }
            void skipToEnd();
        };

        /// Control flags for SkipUntil functions.
        enum SkipUntilFlags
        {
            StopAtSemi = 1 << 0,  ///< Stop skipping at semicolon
            /// Stop skipping at specified token, but don't skip the token itself
            StopBeforeMatch = 1 << 1,
            StopAtCodeCompletion = 1 << 2 ///< Stop at code completion
        };

        friend constexpr SkipUntilFlags operator|( SkipUntilFlags L,
                                                   SkipUntilFlags R )
        {
            return static_cast<SkipUntilFlags>( static_cast<unsigned>( L ) |
                                                static_cast<unsigned>( R ) );
        }

        /// SkipUntil - Read tokens until we get to the specified token, then consume
        /// it (unless StopBeforeMatch is specified).  Because we cannot guarantee
        /// that the token will ever occur, this skips to the next token, or to some
        /// likely good stopping point.  If Flags has StopAtSemi flag, skipping will
        /// stop at a ';' character.
        ///
        /// If SkipUntil finds the specified token, it returns true, otherwise it
        /// returns false.
        bool SkipUntil( clang::tok::TokenKind T,
                        SkipUntilFlags Flags = static_cast<SkipUntilFlags>( 0 ) )
        {
            return SkipUntil( llvm::makeArrayRef( T ), Flags );
        }
        bool SkipUntil( clang::tok::TokenKind T1, clang::tok::TokenKind T2,
                        SkipUntilFlags Flags = static_cast<SkipUntilFlags>( 0 ) )
        {
            clang::tok::TokenKind TokArray[] = { T1, T2 };
            return SkipUntil( TokArray, Flags );
        }
        bool SkipUntil( clang::tok::TokenKind T1, clang::tok::TokenKind T2, clang::tok::TokenKind T3,
                        SkipUntilFlags Flags = static_cast<SkipUntilFlags>( 0 ) )
        {
            clang::tok::TokenKind TokArray[] = { T1, T2, T3 };
            return SkipUntil( TokArray, Flags );
        }

        static bool HasFlagsSet( Parser::SkipUntilFlags L, Parser::SkipUntilFlags R )
        {
            return ( static_cast<unsigned>( L ) & static_cast<unsigned>( R ) ) != 0;
        }
        bool SkipUntil( llvm::ArrayRef<clang::tok::TokenKind> Toks,
                        SkipUntilFlags Flags = static_cast<SkipUntilFlags>( 0 ) )
        {
            using namespace clang;
            // We always want this function to skip at least one token if the first token
            // isn't T and if not at EOF.
            bool isFirstTokenSkipped = true;
            while( 1 )
            {
                // If we found one of the tokens, stop and return true.
                for( unsigned i = 0, NumToks = Toks.size(); i != NumToks; ++i )
                {
                    if( Tok.is( Toks[ i ] ) )
                    {
                        if( HasFlagsSet( Flags, StopBeforeMatch ) )
                        {
                            // Noop, don't consume the token.
                        }
                        else
                        {
                            ConsumeAnyToken();
                        }
                        return true;
                    }
                }

                // Important special case: The caller has given up and just wants us to
                // skip the rest of the file. Do this without recursing, since we can
                // get here precisely because the caller detected too much recursion.
                if( Toks.size() == 1 && Toks[ 0 ] == tok::eof &&
                    !HasFlagsSet( Flags, StopAtSemi ) &&
                    !HasFlagsSet( Flags, StopAtCodeCompletion ) )
                {
                    while( Tok.isNot( tok::eof ) )
                        ConsumeAnyToken();
                    return true;
                }

                switch( Tok.getKind() )
                {
                    case tok::eof:
                        // Ran out of tokens.
                        return false;

                    case tok::annot_pragma_openmp:
                    case tok::annot_pragma_openmp_end:
                        // Stop before an OpenMP pragma boundary.
                    case tok::annot_module_begin:
                    case tok::annot_module_end:
                    case tok::annot_module_include:
                        // Stop before we change submodules. They generally indicate a "good"
                        // place to pick up parsing again (except in the special case where
                        // we're trying to skip to EOF).
                        return false;

                        //case tok::code_completion:
                        //  if (!HasFlagsSet(Flags, StopAtCodeCompletion))
                        //    handleUnexpectedCodeCompletionToken();
                        //  return false;

                    case tok::l_paren:
                        // Recursively skip properly-nested parens.
                        ConsumeParen();
                        if( HasFlagsSet( Flags, StopAtCodeCompletion ) )
                            SkipUntil( tok::r_paren, StopAtCodeCompletion );
                        else
                            SkipUntil( tok::r_paren );
                        break;
                    case tok::l_square:
                        // Recursively skip properly-nested square brackets.
                        ConsumeBracket();
                        if( HasFlagsSet( Flags, StopAtCodeCompletion ) )
                            SkipUntil( tok::r_square, StopAtCodeCompletion );
                        else
                            SkipUntil( tok::r_square );
                        break;
                    case tok::l_brace:
                        // Recursively skip properly-nested braces.
                        ConsumeBrace();
                        if( HasFlagsSet( Flags, StopAtCodeCompletion ) )
                            SkipUntil( tok::r_brace, StopAtCodeCompletion );
                        else
                            SkipUntil( tok::r_brace );
                        break;

                        // Okay, we found a ']' or '}' or ')', which we think should be balanced.
                        // Since the user wasn't looking for this token (if they were, it would
                        // already be handled), this isn't balanced.  If there is a LHS token at a
                        // higher level, we will assume that this matches the unbalanced token
                        // and return it.  Otherwise, this is a spurious RHS token, which we skip.
                    case tok::r_paren:
                        if( ParenCount && !isFirstTokenSkipped )
                            return false;  // Matches something.
                        ConsumeParen();
                        break;
                    case tok::r_square:
                        if( BracketCount && !isFirstTokenSkipped )
                            return false;  // Matches something.
                        ConsumeBracket();
                        break;
                    case tok::r_brace:
                        if( BraceCount && !isFirstTokenSkipped )
                            return false;  // Matches something.
                        ConsumeBrace();
                        break;

                    case tok::semi:
                        if( HasFlagsSet( Flags, StopAtSemi ) )
                            return false;
                        LLVM_FALLTHROUGH;
                    default:
                        // Skip this token.
                        ConsumeAnyToken();
                        break;
                }
                isFirstTokenSkipped = false;
            }
        }
        
        
        //begin of actual parsing routines for eg grammar
        void parse_opaque_until_identifier_semi( ParserSession& session, input::Opaque* pOpaque )
        {
            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc   = Tok.getEndLoc();
            
            clang::Token next = NextToken();
            while( !next.is( clang::tok::semi ) )
            {
                endLoc = Tok.getEndLoc();
                ConsumeToken();
                next = NextToken();
            }
            
            VERIFY_RTE( getSourceText( startLoc, endLoc, pOpaque->m_str ) );
        }

        void parse_dimension( ParserSession& session, input::Dimension* pDimension )
        {
            pDimension->m_pType = session.construct< input::Opaque >();
            parse_opaque_until_identifier_semi( session, pDimension->m_pType );
            
            if( Tok.is( clang::tok::identifier ) )
            {
                clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                pDimension->m_strIdentifier = pIdentifier->getName();
                ConsumeToken();
            }
            else
            {
                EG_PARSER_ERROR( "Expected identifier" );
            }
            
            if( !TryConsumeToken( clang::tok::semi ) )
            {
                //Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
                EG_PARSER_ERROR( "Expected semicolon" );
            }
        }
        
        void parse_include( ParserSession& session, input::Include* pInclude )
        {
            if( Tok.is( clang::tok::identifier ) )
            {
                clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                pInclude->m_strIdentifier = pIdentifier->getName();
                ConsumeToken();
            }

            BalancedDelimiterTracker T( *this, clang::tok::l_paren );

            T.consumeOpen();

            clang::SourceLocation startLoc = Tok.getLocation();
            clang::SourceLocation endLoc = Tok.getEndLoc();

            while( !Tok.is( clang::tok::r_paren ) )
            {
                endLoc = Tok.getEndLoc();
                ConsumeAnyToken();
            }

            std::string strFile;
            VERIFY_RTE( getSourceText( startLoc, endLoc, strFile ) );
            strFile.erase( std::remove( strFile.begin(), strFile.end(), '\"' ), strFile.end() );
            if( !strFile.empty() )
            {
                const bool bIsAngled = strFile[ 0 ] == '<';
                const clang::DirectoryLookup *CurDir;
                if( const clang::FileEntry* pIncludeFile = PP.LookupFile(
                    clang::SourceLocation(), strFile,
                    /*isAngled=*/bIsAngled, /*FromDir=*/nullptr, /*FromFile=*/nullptr, CurDir,
                    /*SearchPath=*/nullptr, /*RelativePath=*/nullptr,
                    /*SuggestedModule=*/nullptr, /*IsMapped=*/nullptr) )
                {
                    pInclude->setIncludeFilePath( pIncludeFile->tryGetRealPathName() );
                }
                else
                {
                    pInclude->setIncludeFilePath( strFile );
                }
            }
            else
            {
                EG_PARSER_ERROR( "Include file is empty" );
            }

            T.consumeClose();

            if( !TryConsumeToken( clang::tok::semi ) )
            {
                //Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
                EG_PARSER_ERROR( "expected semicolon" );
            }
        }
        
        void parse_using( ParserSession& session, input::Using* pUsing )
        {
            if( Tok.is( clang::tok::kw_using ) )
            {
                ConsumeToken();
            }
            
            if( Tok.is( clang::tok::identifier ) )
            {
                clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                pUsing->m_strIdentifier = pIdentifier->getName();
                ConsumeToken();
            }
            else
            {
                EG_PARSER_ERROR( "Expected identifier" );
            }
            
            if( Tok.is( clang::tok::equal ) )
            {
                ConsumeToken();
            }
            else
            {
                EG_PARSER_ERROR( "Expected equals after using" );
            }
            
            //parse the type
            {
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                
                while( !Tok.is( clang::tok::semi ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeToken();
                }
                
                pUsing->m_pType = session.construct< input::Opaque >();
                VERIFY_RTE( getSourceText( startLoc, endLoc, pUsing->m_pType->m_str ) );
            }
            
            if( !TryConsumeToken( clang::tok::semi ) )
            {
                //Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
                EG_PARSER_ERROR( "Expected semicolon" );
            }
        }
        
        bool try_consume_template( clang::SourceLocation& endLoc )
        {
            endLoc = Tok.getEndLoc();
            if( TryConsumeToken( clang::tok::less ) )
            {
                int iArrowCounter = 1;

                while( !isEofOrEom() )
                {
                    endLoc = Tok.getEndLoc();
                    if( Tok.is( clang::tok::less ) )
                    {
                        ConsumeToken();
                        ++iArrowCounter;
                    }
                    else if( Tok.is( clang::tok::greater ) )
                    {
                        ConsumeToken();
                        --iArrowCounter;
                    }
                    else
                    {
                        ConsumeToken();
                    }
                    if( 0 == iArrowCounter )
                    {
                        break;
                    }
                }
                VERIFY_RTE( iArrowCounter == 0 );
                return true;
            }
            return false;
        }

        void parse_template( ParserSession& session, input::Action* pAction  )
        {
            if( Tok.is( clang::tok::kw_template ) )
            {
                ConsumeToken();
                pAction->m_bIsTemplate = true;
                clang::SourceLocation endLoc;
                if( !try_consume_template( endLoc ) )
                {
                    //error
                    //Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
                    EG_PARSER_ERROR( "template arrow missing" );
                }
            }
        }
        
        input::Action* constructAction( ParserSession& session, input::Action* pParentAction )
        {
            input::Action* pNewAction = session.construct< input::Action >();
            pNewAction->m_pBody = session.construct< input::Opaque >();
            pNewAction->m_elements.push_back( pNewAction->m_pBody );
            pNewAction->m_pBody->m_bSemantic = false;
            pParentAction->m_elements.push_back( pNewAction );
            return pNewAction;
        }

        void parse_action( ParserSession& session, input::Action* pAction, const boost::filesystem::path& egSourceFile )
        {
            while( Tok.is( clang::tok::coloncolon ) )
            {
                ConsumeToken();

                if( Tok.is( clang::tok::identifier ) )
                {
                    clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();

                    input::Action* pNestedAction = pAction->findAction( pIdentifier->getName() );
                    if( !pNestedAction )
                    {
                        pNestedAction = constructAction( session, pAction );
                        pNestedAction->m_strIdentifier = pIdentifier->getName();
                    }
                    pAction = pNestedAction;
                    ConsumeToken();
                }
                else
                {
                    EG_PARSER_ERROR( "Expected identifier" );
                }
            }
            
            while( Tok.is( clang::tok::comment ) )
            {
                ConsumeToken();
            }

            //parse optional size specifier
            if( Tok.is( clang::tok::l_square ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_square );
                T.consumeOpen();
                
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();
                
                while( !isEofOrEom() && !Tok.is( clang::tok::r_square ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                
                {
                    input::Opaque* pOpaque = session.construct< input::Opaque >();
                    VERIFY_RTE( getSourceText( startLoc, endLoc, pOpaque->m_str ) );
                    pAction->m_pSize = pOpaque;
                }
                
                //SkipUntil( clang::tok::r_square, StopBeforeMatch );
                T.consumeClose();
            }
            
            while( Tok.is( clang::tok::comment ) )
            {
                ConsumeToken();
            }

            //parse optional argument list
            if( Tok.is( clang::tok::l_paren ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_paren );
                T.consumeOpen();
                
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();
                
                while( !isEofOrEom() && !Tok.is( clang::tok::r_paren ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                
                {
                    input::Opaque* pOpaque = session.construct< input::Opaque >();
                    VERIFY_RTE( getSourceText( startLoc, endLoc, pOpaque->m_str ) );
                    pAction->m_pParams = pOpaque;
                }
                
                //SkipUntil( clang::tok::r_paren, StopBeforeMatch );
                T.consumeClose();
            }
            
            while( Tok.is( clang::tok::comment ) )
            {
                ConsumeToken();
            }
            
            //parse optional inheritance list
            if( Tok.is( clang::tok::colon ) )
            {
                bool bFoundComma = true;
                while( bFoundComma )
                {
                    ConsumeAnyToken();
                    bFoundComma = false;
                    
                    clang::SourceLocation startLoc = Tok.getLocation();
                    clang::SourceLocation endLoc   = Tok.getEndLoc();
                    ConsumeAnyToken();
                    
                    while( !isEofOrEom() && !Tok.isOneOf( clang::tok::semi, clang::tok::comma, clang::tok::l_brace ) )
                    {
                        if( !try_consume_template( endLoc ) )
                        {
                            endLoc = Tok.getEndLoc();
                            ConsumeAnyToken();
                        }
                    }
                    
                    {
                        input::Opaque* pOpaque = session.construct< input::Opaque >();
                        VERIFY_RTE( getSourceText( startLoc, endLoc, pOpaque->m_str ) );
                        pAction->m_inheritance.push_back( pOpaque );
                    }
                    
                    if( Tok.is( clang::tok::comma ) )
                        bFoundComma = true;
                
                }
            }
            
            while( Tok.is( clang::tok::comment ) )
            {
                ConsumeToken();
            }

            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();

                parse_action_body( session, pAction, egSourceFile );

                T.consumeClose();
            }
            else if( Tok.is( clang::tok::semi ) )
            {
                ConsumeToken();
            }
            else
            {
                EG_PARSER_ERROR( "Expected semicolon" );
            }

        }

        void parse_action_body( ParserSession& session, input::Action* pAction, const boost::filesystem::path& egSourceFile )
        {
            bool bActionDefinition = false;
            
            braceStack.push_back( BraceCount );

            while( !isEofOrEom() )
            {
                if( Tok.is( clang::tok::kw_template ) )
                {
                    input::Action* pNestedAction = constructAction( session, pAction );
                    parse_template( session, pNestedAction );
                    if( Tok.is( clang::tok::kw_action ) )
                    {
                        ConsumeToken();

                        if( Tok.is( clang::tok::identifier ) )
                        {
                            clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                            pNestedAction->m_strIdentifier = pIdentifier->getName();
                            ConsumeToken();
                        }
                        else
                        {
                            EG_PARSER_ERROR( "Expected identifier" );
                        }
                        parse_action( session, pNestedAction, egSourceFile );
                    }
                    else
                    {
                        EG_PARSER_ERROR( "Missing action" );
                    }
                }
                else if( Tok.is( clang::tok::kw_action ) )
                {
                    ConsumeToken();

                    if( Tok.is( clang::tok::identifier ) )
                    {
                        clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                        input::Action* pNestedAction = pAction->findAction( pIdentifier->getName() );
                        if( !pNestedAction )
                        {
                            pNestedAction = constructAction( session, pAction );
                            pNestedAction->m_strIdentifier = pIdentifier->getName();
                        }
                        ConsumeToken();
                        parse_action( session, pNestedAction, egSourceFile );
                    }
                    else
                    {
                        EG_PARSER_ERROR( "Expected identifier" );
                    }
                }
                else if( Tok.is( clang::tok::kw_abstract ) )
                {
                    ConsumeToken();
                    //consume optional action keyword
                    bool bIsLink = false;
                    if( Tok.is( clang::tok::kw_link ) )
                    {
                        ConsumeToken();
                        bIsLink = true;
                    }
                    
                    //if( Tok.is( clang::tok::kw_action ) )
                    //{
                    //    ConsumeToken();
                    //}

                    if( Tok.is( clang::tok::identifier ) )
                    {
                        clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                        input::Action* pNestedAction = pAction->findAction( pIdentifier->getName() );
                        if( !pNestedAction )
                        {
                            pNestedAction = constructAction( session, pAction );
                            pNestedAction->m_strIdentifier = pIdentifier->getName();
                            pNestedAction->m_bLink = bIsLink;
                            pNestedAction->m_bAbstract = true;
                        }
                        else
                        {
                            VERIFY_RTE_MSG( pNestedAction->m_bAbstract, "Action inconsistently declared abstract" );
                        }
                        ConsumeToken();
                        parse_action( session, pNestedAction, egSourceFile );
                    }
                    else
                    {
                        EG_PARSER_ERROR( "Expected identifier" );
                    }
                }
                else if( Tok.is( clang::tok::kw_link ) )
                {
                    ConsumeToken();
                    //consume optional action keyword
                    bool bIsAbstract = false;
                    //if( Tok.is( clang::tok::kw_abstract ) )
                    //{
                    //    ConsumeToken();
                    //    bIsAbstract = true;
                    //}

                    if( Tok.is( clang::tok::identifier ) )
                    {
                        clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                        input::Action* pNestedAction = pAction->findAction( pIdentifier->getName() );
                        if( !pNestedAction )
                        {
                            pNestedAction = constructAction( session, pAction );
                            pNestedAction->m_strIdentifier = pIdentifier->getName();
                            pNestedAction->m_bLink = true;
                            pNestedAction->m_bAbstract = bIsAbstract;
                        }
                        else
                        {
                            VERIFY_RTE_MSG( pNestedAction->m_bAbstract, "Action inconsistently declared abstract" );
                        }
                        ConsumeToken();
                        parse_action( session, pNestedAction, egSourceFile );
                    }
                    else
                    {
                        EG_PARSER_ERROR( "Expected identifier" );
                    }
                }
                else if( Tok.is( clang::tok::kw_dim ) )
                {
                    ConsumeToken();
                    input::Dimension* pDimension = session.construct< input::Dimension >();
                    pAction->m_elements.push_back( pDimension );
                    parse_dimension( session, pDimension );
                }
                else if( Tok.is( clang::tok::kw_include ) )
                {
                    ConsumeToken();
                    input::Include* pInclude = session.construct< input::Include >();
                    pAction->m_elements.push_back( pInclude );
                    parse_include( session, pInclude );
                }
                else if( Tok.is( clang::tok::kw_using ) )
                {
                    ConsumeToken();
                    input::Using* pUsing = session.construct< input::Using >();
                    pAction->m_elements.push_back( pUsing );
                    parse_using( session, pUsing );
                }
                else if( Tok.is( clang::tok::r_brace ) && ( BraceCount == braceStack.back() ) )
                {
                    //leave the r_brace to be consumed by parent
                    break;
                }
                else
                {
                    clang::SourceLocation startLoc = Tok.getLocation();
                    clang::SourceLocation endLoc   = Tok.getEndLoc();
                    ConsumeAnyToken();
                    
                    while( !isEofOrEom() && 
                            !Tok.isOneOf
                            ( 
                                clang::tok::kw_action, 
                                clang::tok::kw_abstract, 
                                clang::tok::kw_dim, 
                                clang::tok::kw_link, 
                                clang::tok::kw_include,
                                clang::tok::kw_using,
                                clang::tok::kw_template 
                             ) && 
                        !( ( BraceCount == braceStack.back() ) && Tok.is( clang::tok::r_brace ) )
                        )
                    {
                        if( !Tok.isOneOf( clang::tok::comment, clang::tok::eof, clang::tok::eod, clang::tok::code_completion ) )
                        {
                            bActionDefinition = true;
                        }
                        
                        endLoc = Tok.getEndLoc();
                        ConsumeAnyToken();
                    }
                    
                    {
                        std::string strBodyPart;
                        VERIFY_RTE( getSourceText( startLoc, endLoc, strBodyPart ) );
                        
                        if( !strBodyPart.empty() )
                        {
                            std::ostringstream os;
                            os << pAction->m_pBody->m_str << "\n" << strBodyPart;
                            pAction->m_pBody->m_str = os.str();
                        }
                    }
                }
            }
            
            VERIFY_RTE( BraceCount == braceStack.back() );
            braceStack.pop_back();
            
            if( bActionDefinition )
            {
                if( pAction->m_definitionFile )
                {
                    if( pAction->m_definitionFile == egSourceFile )
                    {
                        THROW_RTE( "Action: " << pAction->getIdentifier() << " multiply defined in: " << egSourceFile );
                    }
                    else
                    {
                        THROW_RTE( "Action: " << pAction->getIdentifier() << " multiply defined in: " <<
                           pAction->m_definitionFile.value() << " and " << egSourceFile );
                    }
                }
                if( pAction->isAbstract() )
                {
                    THROW_RTE( "Action: " << pAction->getIdentifier() << " is abstract but has definition: " << egSourceFile );
                }
                pAction->m_definitionFile = egSourceFile;
            }
        }
        
        input::Root* parse_root( ParserSession& session, const boost::filesystem::path& egSourceFile, bool bMainRoot )
        {
            input::Root* pRoot = nullptr;
            //if main file then reuse root
            if( bMainRoot )
            {
                pRoot = oneOpt< input::Root >( session.getNewObjects() );
                if( !pRoot )
                {
                    pRoot = session.construct< input::Root >();
                    pRoot->m_pBody = session.construct< input::Opaque >();
                    pRoot->m_elements.push_back( pRoot->m_pBody );
                    pRoot->m_pBody->m_bSemantic = false;
                }
                parse_action_body( session, pRoot, egSourceFile );
            }
            else
            {
                pRoot = session.construct< input::Root >();
                pRoot->m_pBody = session.construct< input::Opaque >();
                pRoot->m_elements.push_back( pRoot->m_pBody );
                pRoot->m_pBody->m_bSemantic = false;
                
                pRoot->m_includePath = egSourceFile;
                parse_action_body( session, pRoot, egSourceFile );
            }
            
            return pRoot;
        }
    };

    struct Stuff
    {
        std::unique_ptr< clang::SourceManager > pSourceManager;

        std::shared_ptr< clang::HeaderSearchOptions > headerSearchOptions;

        clang::LangOptions languageOptions;

        clang::LangOptions createEGLangOpts()
        {
            clang::LangOptions LangOpts;
            LangOpts.CPlusPlus = 1;
            LangOpts.CPlusPlus11 = 1;
            LangOpts.CPlusPlus14 = 1;
            LangOpts.CPlusPlus17 = 1;
            LangOpts.CPlusPlus2a = 1;
            LangOpts.LineComment = 1;
            LangOpts.CXXOperatorNames = 1;
            LangOpts.Bool = 1;
            //LangOpts.ObjC = 1;
            //LangOpts.MicrosoftExt = 1;    // To get kw___try, kw___finally.
            LangOpts.DeclSpecKeyword = 1; // To get __declspec.
            LangOpts.WChar = 1;           // To get wchar_t
            LangOpts.EG = 1;              //enable eg
            return LangOpts;
        }

        std::unique_ptr< clang::HeaderSearch > pHeaderSearch;

        std::unique_ptr< clang::TrivialModuleLoader > pModuleLoader;

        std::shared_ptr< clang::PreprocessorOptions > pPreprocessorOptions;

        llvm::IntrusiveRefCntPtr< clang::MemoryBufferCache > pPCMCache;

        std::shared_ptr< clang::Preprocessor > pPreprocessor;

        std::shared_ptr< clang::TargetOptions > pTargetOptions;

        inline std::shared_ptr< clang::TargetOptions > getTargetOptions()
        {
            std::shared_ptr< clang::TargetOptions > pTargetOptions =
                std::make_shared< clang::TargetOptions >();

            llvm::Triple triple;
            triple.setArch( llvm::Triple::x86 );
            triple.setVendor( llvm::Triple::PC );
            triple.setOS( llvm::Triple::Win32 );

            pTargetOptions->Triple = triple.normalize();
            return pTargetOptions;
        }

        std::shared_ptr< clang::TargetInfo > pTargetInfo;

        Stuff( std::shared_ptr< clang::FileManager > pFileManager,
               llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine,
               const boost::filesystem::path& egSourceFile )

            : pSourceManager( std::make_unique< clang::SourceManager >( *pDiagnosticsEngine, *pFileManager ) ),

            headerSearchOptions( std::make_shared< clang::HeaderSearchOptions >() ),

            languageOptions( createEGLangOpts() ),

            pHeaderSearch( std::make_unique< clang::HeaderSearch >(
                headerSearchOptions, *pSourceManager, *pDiagnosticsEngine, languageOptions, nullptr ) ),

            pModuleLoader( std::make_unique< clang::TrivialModuleLoader >() ),

            pPreprocessorOptions( std::make_shared< clang::PreprocessorOptions >() ),

            pPCMCache( new clang::MemoryBufferCache ),

            pPreprocessor( std::make_shared< clang::Preprocessor >(
                pPreprocessorOptions, *pDiagnosticsEngine, languageOptions,
                *pSourceManager, *pPCMCache, *pHeaderSearch, *pModuleLoader, /*PTHMgr*/nullptr,
                /*OwnsHeaderSearch=*/false, clang::TU_Complete ) ),

            pTargetOptions( getTargetOptions() ),

            pTargetInfo( clang::TargetInfo::CreateTargetInfo( *pDiagnosticsEngine, pTargetOptions ) )
        {
            const clang::FileEntry* pFileEntry =
                pFileManager->getFile( egSourceFile.string(), true, false );

            clang::FileID fileID =
                pSourceManager->getOrCreateFileID( pFileEntry, clang::SrcMgr::C_User );

            pSourceManager->setMainFileID( fileID );

            pPreprocessor->SetCommentRetentionState( true, true );
            pPreprocessor->Initialize( *pTargetInfo );
        }

    };
    

    input::Root* parse( const boost::filesystem::path& egSourceFile,
                std::shared_ptr< clang::FileManager > pFileManager,
                llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine,
                ParserSession& session, bool bMainRoot )
    {
        //check file exists
        if( !boost::filesystem::exists( egSourceFile ) )
        {
            THROW_RTE( "File not found: " << egSourceFile );
        }
        
        Stuff stuff( pFileManager, pDiagnosticsEngine, egSourceFile );

        Parser parser( *stuff.pPreprocessor, 
            *stuff.pSourceManager, 
            stuff.languageOptions, 
            *stuff.pHeaderSearch,
            pDiagnosticsEngine );
        parser.ConsumeToken();
        return parser.parse_root( session, egSourceFile, bMainRoot );
    }


    ParserSession::ParserSession()
        :   CreatingSession( IndexedFile::FileIDtoPathMap{}, IndexedObject::MASTER_FILE )
    {
        
    }

    void ParserSession::parse( const std::vector< boost::filesystem::path >& egSourceCodeFiles, 
        std::shared_ptr< clang::FileManager > pFileManager,
        llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine )
    {
        std::set< boost::filesystem::path > includePaths;
        for( const boost::filesystem::path& filePath : egSourceCodeFiles )
        {
            input::Root* pRoot = ::eg::parse( filePath, pFileManager, pDiagnosticsEngine, *this, true ); //parse main root
            includePaths.insert( filePath );
        }
        
        std::set< boost::filesystem::path > newIncludePaths;
        
        //greedy parse all includes 
        do
        {
            for( const boost::filesystem::path& includePath : newIncludePaths )
            {
                ::eg::parse( includePath, pFileManager, pDiagnosticsEngine, *this, false ); //parse include - non-main root
                includePaths.insert( includePath );
            }
            newIncludePaths.clear();
            
            std::vector< input::Include* > includes = many< input::Include >( getMaster() );
            for( input::Include* pInclude : includes )
            {
                if( pInclude->isEGInclude() )
                {
                    const boost::filesystem::path& filePath = pInclude->getIncludeFilePath();
                    if( includePaths.find( filePath ) == includePaths.end() )
                        newIncludePaths.insert( filePath );
                }
            }
        }while( !newIncludePaths.empty() );
    }
    
    
    interface::Element* addChild( ParserSession& session, interface::Element* pParent, input::Element* pElement )
    {
        interface::Element* pNewNode = nullptr;
        switch( pElement->getType() )
        {
            case eInputOpaque:         pNewNode = session.construct< interface::Opaque >(    pParent, pElement ); break;
            case eInputDimension:      pNewNode = session.construct< interface::Dimension >( pParent, pElement ); break;
            case eInputInclude:        pNewNode = session.construct< interface::Include >(   pParent, pElement ); break;
            case eInputUsing:          pNewNode = session.construct< interface::Using >(     pParent, pElement ); break;
            case eInputAction:         pNewNode = session.construct< interface::Action >(    pParent, pElement ); break;
            case eInputRoot:           pNewNode = session.construct< interface::Root >(      pParent, pElement ); break;
            default:
                THROW_RTE( "Unsupported type" );
                break;
        }
        VERIFY_RTE( pNewNode );
        return pNewNode;
    }
    
    void ParserSession::buildTree( const FileElementMap& fileMap, interface::Element* pParentNode, 
        input::Element* pElement, const boost::filesystem::path& includeDefinitionFile, bool bInIncludeTree )
    {
        switch( pElement->getType() )
        {
            case eInputOpaque    :
                {
                    //do nothing
                }
                break;
            case eInputDimension :
                {
                    //do nothing
                }
                break;
            case eInputInclude   :
                THROW_RTE( "unreachable" );
                break;
            case eInputUsing     :
                {
                    //do nothing
                }
                break;
            case eInputAction    :
            case eInputRoot      :
                {
                    input::Action* pAction = dynamic_cast< input::Action* >( pElement );
                    VERIFY_RTE( pAction );
                    
                    for( input::Element* pChildElement : pAction->getElements() )
                    {
                        if( input::Include* pInclude = dynamic_cast< input::Include* >( pChildElement ) )
                        {
                            if( pInclude->isEGInclude() )
                            {
                                FileElementMap::const_iterator iFind = 
                                    fileMap.find( pInclude->getIncludeFilePath() );
                                VERIFY_RTE( iFind != fileMap.end() );
                                input::Root* pIncludedRoot = iFind->second;
                                
                                if( pInclude->getIdentifier().empty() )
                                {
                                    //if the include has no identifier then insert the included root elements
                                    buildTree( fileMap, pParentNode, pIncludedRoot, includeDefinitionFile, true );
                                }
                                else
                                {
                                    //otherwise insert the root with the include identifier
                                    interface::Element* pChild = addChild( *this, pParentNode, pIncludedRoot );
                                    pChild->pIncludeIdentifier = pInclude;
                                    buildTree( fileMap, pChild, pIncludedRoot, includeDefinitionFile, true );
                                } 
                            }
                            else
                            {
                                addChild( *this, pParentNode, pChildElement );
                            }
                        }
                        else if( input::Action* pElementAction = dynamic_cast< input::Action* >( pChildElement ) )
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement );
                            if( bInIncludeTree )
                            {
                                //if the action is defined then set the definition file to the include definition file
                                if( pElementAction->getDefinitionFile() )
                                {
                                    ( (interface::Action*)pChild )->setDefinitionFile( includeDefinitionFile );
                                }
                                buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, true );
                            }
                            else
                            {
                                //if the action is defined then use its definition file for any include sub trees
                                if( pElementAction->getDefinitionFile() )
                                {
                                    ( (interface::Action*)pChild )->setDefinitionFile( pElementAction->getDefinitionFile().value() );
                                    buildTree( fileMap, pChild, pChildElement, pElementAction->getDefinitionFile().value(), false );
                                }
                                else
                                {
                                    buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, bInIncludeTree );
                                }
                            }
                        }
                        else
                        {
                            interface::Element* pChild = addChild( *this, pParentNode, pChildElement );
                            buildTree( fileMap, pChild, pChildElement, includeDefinitionFile, bInIncludeTree );
                        }
                    }
                }
                break;
            default:
                THROW_RTE( "Unsupported type" );
                break;
        }
    }
    
    void ParserSession::buildAbstractTree()
    {
        interface::Root* pMasterRoot = construct< interface::Root >();
        
        std::vector< input::Root* > roots = many< input::Root >( getMaster() );
        VERIFY_RTE( !roots.empty() );
        
        input::Root* pInputMainRoot = nullptr;
        
        FileElementMap fileMap;
        for( input::Root* pRootElement : roots )
        {
            if( std::optional< boost::filesystem::path > includePathOpt = pRootElement->getIncludePath() )
            {
                fileMap.insert( std::make_pair( includePathOpt.value(), pRootElement ) );
            }
            else
            {
                VERIFY_RTE( !pInputMainRoot );
                pInputMainRoot = pRootElement;
                if( !pInputMainRoot->getDefinitionFile() )
                {
                    THROW_RTE( "Root has no definition" );
                }
            }
        }
        
        VERIFY_RTE( pInputMainRoot );
        interface::Element* pChild = addChild( *this, pMasterRoot, pInputMainRoot );
        ( (interface::Action*)pChild )->setDefinitionFile( pInputMainRoot->getDefinitionFile().value() );
        
        buildTree( fileMap, pChild, pInputMainRoot, pInputMainRoot->getDefinitionFile().value(), false );
        
        //create the identifiers object
        Identifiers* pIdentifiers = construct< Identifiers >();
        pIdentifiers->populate( getMaster() );
    }
    
    
    
    IncrementalParserSession::IncrementalParserSession( const boost::filesystem::path& treePath )
    {
        VERIFY_RTE( m_fileID == IndexedObject::MASTER_FILE );
        ObjectFactoryImpl objectFactory;
        IndexedFile::load( objectFactory, m_fileMap, treePath, m_fileID );
        
        IndexedFile* pRootFile = m_fileMap[ m_fileID ];
        VERIFY_RTE( pRootFile );
        
        m_newObjects.swap( pRootFile->getObjects() );
        delete pRootFile;
        m_fileMap.clear();
    }

    bool IncrementalParserSession::update( const ParserSession& parse )
    {
        interface::Root* pOldRoot = eg::root< eg::interface::Root >( getMaster() );
        return pOldRoot->update( parse.getTreeRoot() );
    }
    
} //namespace eg
