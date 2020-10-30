
#include "parser/parser.hpp"

#include "eg_compiler/sessions/parser_session.hpp"

#include "eg_compiler/eg.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/interface.hpp"
#include "eg_compiler/identifiers.hpp"

#include <boost/config.hpp> // for BOOST_SYMBOL_EXPORT
#include <boost/algorithm/string.hpp>

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
#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/DiagnosticOptions.h"

#pragma warning( pop ) 

namespace eg
{

class ParserDiagnosticSystem
{
public:
    ParserDiagnosticSystem( const boost::filesystem::path& currentPath, std::ostream& os );
    
    class Pimpl;
    std::shared_ptr< Pimpl > m_pImpl;
};
	
class ParserDiagnosticSystem::Pimpl
{
	class EGDiagConsumer : public clang::DiagnosticConsumer 
	{
		std::ostream& m_outStream;
	public:
		EGDiagConsumer( std::ostream& os )
			:	m_outStream( os )
		{
			
		}
		
		virtual void anchor()
		{
		}

		void HandleDiagnostic( clang::DiagnosticsEngine::Level DiagLevel,
							const clang::Diagnostic &Info) override 
		{
			llvm::SmallString< 100 > msg;
			Info.FormatDiagnostic( msg );
			std::string str = msg.str();
			m_outStream << str << "\n";
		}
	};

public:
	std::shared_ptr< clang::FileManager > m_pFileManager;
	
	llvm::IntrusiveRefCntPtr< clang::DiagnosticOptions > m_pDiagnosticOptions;
	llvm::IntrusiveRefCntPtr< clang::DiagnosticIDs > m_pDiagnosticIDs;
	llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > m_pDiagnosticsEngine;
		
	Pimpl( const boost::filesystem::path& currentPath, std::ostream& os )
		:	m_pFileManager( std::make_shared< clang::FileManager >( clang::FileSystemOptions{ currentPath.string() } ) ),
		
			m_pDiagnosticOptions( new clang::DiagnosticOptions() ),
			m_pDiagnosticIDs( new clang::DiagnosticIDs() ),
			m_pDiagnosticsEngine( 
				new clang::DiagnosticsEngine( 
					m_pDiagnosticIDs, 
					m_pDiagnosticOptions, 
					new EGDiagConsumer( os ) ) )
	{
	}
};
	
ParserDiagnosticSystem::ParserDiagnosticSystem( const boost::filesystem::path& currentPath, std::ostream& os )
	:	m_pImpl( new Pimpl( currentPath, os ) )
{
	
}

std::shared_ptr< clang::FileManager >
	get_clang_fileManager( ParserDiagnosticSystem& diagnosticSystem )
{
	return diagnosticSystem.m_pImpl->m_pFileManager;
}

llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine >
	get_llvm_diagnosticEngine( ParserDiagnosticSystem& diagnosticSystem )
{
	return diagnosticSystem.m_pImpl->m_pDiagnosticsEngine;
}

#define EG_PARSER_ERROR( _msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream _os; \
        _os << _msg; \
        Diags->Report( Tok.getLocation(), clang::diag::err_eg_generic_error ) << _os.str();\
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

        EG_PARSER_CALLBACK* m_pCallback;
    public:
        Parser( EG_PARSER_CALLBACK* pCallback,
                clang::Preprocessor& PP, 
                clang::SourceManager& sourceManager,
                clang::LangOptions& languageOptions,
                clang::HeaderSearch& headerSearch,
                llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > Diags )
            :   m_pCallback( pCallback ),
                PP( PP ),
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

            /*
            unsigned short &getDepth()
            {
                switch( Kind )
                {
                    case clang::tok::l_brace: return P.BraceCount;
                    case clang::tok::l_square: return P.BracketCount;
                    case clang::tok::l_paren: return P.ParenCount;
                    default: llvm_unreachable( "Wrong token kind" );
                }
            }*/

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
        
        
        
        ////////////////////////////////////////////////////////////////////////////////////////
        ////////////////////////////////////////////////////////////////////////////////////////
        //high level parsing utility functions
        
        void parse_identifier( std::string& strIdentifier )
        {
            if( Tok.is( clang::tok::identifier ) )
            {
                clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                strIdentifier = pIdentifier->getName();
                ConsumeToken();
            }
            else
            {
                EG_PARSER_ERROR( "Expected identifier" );
            }
        }
        
        void parse_visibility( ParserSession& session, input::Visibility* pVisibility )
        {
            if( Tok.is( clang::tok::kw_public ) )
            {
                pVisibility->m_visibility = eg::eVisPublic;
                ConsumeToken();
            }
            else if( Tok.is( clang::tok::kw_private ) )
            {
                pVisibility->m_visibility = eg::eVisPrivate;
                ConsumeToken();
            }
            else
            {
                EG_PARSER_ERROR( "Expected public or private token" );
            }
            if( !TryConsumeToken( clang::tok::colon ) )
            {
                //Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
                EG_PARSER_ERROR( "Expected colon" );
            }
        }
        
        void parse_semicolon()
        {
            if( !TryConsumeToken( clang::tok::semi ) )
            {
                //Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
                EG_PARSER_ERROR( "Expected semicolon" );
            }
        }
        
        void parse_argumentList( ParserSession& session, input::Opaque*& pArguments )
        {
            if( Tok.is( clang::tok::l_paren ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_paren );
                T.consumeOpen();
                
                if( !Tok.is( clang::tok::r_paren ) )
                {
                    clang::SourceLocation startLoc = Tok.getLocation();
                    clang::SourceLocation endLoc   = Tok.getEndLoc();
                    ConsumeAnyToken();
                    
                    while( !isEofOrEom() && !Tok.is( clang::tok::r_paren ) )
                    {
                        endLoc = Tok.getEndLoc();
                        ConsumeAnyToken();
                    }
                    pArguments = session.construct< input::Opaque >();
                    if( !getSourceText( startLoc, endLoc, pArguments->m_str ) )
                    {
                        EG_PARSER_ERROR( "Error parsing argument list" );
                    }
                }
                else
                {
                    //generate empty parameter list since found empty parens
                    pArguments = session.construct< input::Opaque >();
                }
                
                T.consumeClose();
            }
        }
        
        void parse_comment()
        {
            while( Tok.is( clang::tok::comment ) )
            {
                ConsumeToken();
            }
        }
        
            
        void parse_returnType( ParserSession& session, input::Opaque*& pReturnType )
        {
            if( Tok.is( clang::tok::colon ) )
            {
                ConsumeAnyToken();
                
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();
                
                while( !isEofOrEom() && !Tok.isOneOf( clang::tok::semi, clang::tok::comma, clang::tok::l_brace ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                
                {
                    pReturnType = session.construct< input::Opaque >();
                    if( !getSourceText( startLoc, endLoc, pReturnType->m_str ) )
                    {
                        EG_PARSER_ERROR( "Error parsing return type" );
                    }
                }
            }
        }
        
        void parse_inheritance( ParserSession& session, std::vector< input::Opaque* >& inheritance )
        {
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
                        endLoc = Tok.getEndLoc();
                        ConsumeAnyToken();
                    }
                    
                    {
                        input::Opaque* pInheritance = session.construct< input::Opaque >();
                        if( !getSourceText( startLoc, endLoc, pInheritance->m_str ) )
                        {
                            EG_PARSER_ERROR( "Error parsing inheritance" );
                        }
                        inheritance.push_back( pInheritance );
                    }
                    
                    if( Tok.is( clang::tok::comma ) )
                        bFoundComma = true;
                }
            }
        }
        
        void parse_body( ParserSession& session, std::string& strBody )
        {
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                
                braceStack.push_back( BraceCount );
            
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                ConsumeAnyToken();
                
                while( !isEofOrEom() && !( Tok.is( clang::tok::r_brace ) && ( BraceCount == braceStack.back() ) ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeAnyToken();
                }
                
                if( !getSourceText( startLoc, endLoc, strBody ) )
                {
                    EG_PARSER_ERROR( "Error parsing body" );
                }

                if( !BraceCount == braceStack.back() )
                {
                    EG_PARSER_ERROR( "Brace count mismatch" );
                }
                braceStack.pop_back();
                
                T.consumeClose();
            }
        }
        
        void parse_size( ParserSession& session, input::Opaque*& pSize )
        {
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
                    pSize = session.construct< input::Opaque >();
                    if( !getSourceText( startLoc, endLoc, pSize->m_str ) )
                    {
                        EG_PARSER_ERROR( "Error parsing size" );
                    }
                }
                
                T.consumeClose();
            }
        }
        
        
        
        //begin of actual parsing routines for eg grammar
        void parse_dimension( ParserSession& session, input::Dimension* pDimension )
        {
            //dim type identifier;
            {
                pDimension->m_pType = session.construct< input::Opaque >();
                
                clang::SourceLocation startLoc = Tok.getLocation();
                clang::SourceLocation endLoc   = Tok.getEndLoc();
                
                clang::Token next = NextToken();
                while( !next.is( clang::tok::semi ) )
                {
                    endLoc = Tok.getEndLoc();
                    ConsumeToken();
                    next = NextToken();
                }
                
                VERIFY_RTE( getSourceText( startLoc, endLoc, pDimension->m_pType->m_str ) );
            }
            
            parse_identifier( pDimension->m_strIdentifier );
            parse_semicolon();
        }
        
        void parse_include( ParserSession& session, input::Include* pInclude )
        {
            //include name( file );
            //optional identifier
            if( Tok.is( clang::tok::identifier ) )
            {
                parse_identifier( pInclude->m_strIdentifier );
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

            parse_semicolon();
        }
        
        void parse_using( ParserSession& session, input::Using* pUsing )
        {
            //using T = expression;
            parse_identifier( pUsing->m_strIdentifier );
            
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
                if( !getSourceText( startLoc, endLoc, pUsing->m_pType->m_str ) )
                {
                    EG_PARSER_ERROR( "Error parsing using statement" );
                }
            }
            
            if( !TryConsumeToken( clang::tok::semi ) )
            {
                //Diag( Tok.getLocation(), clang::diag::err_expected_less_after ) << "template";
                EG_PARSER_ERROR( "Expected semicolon" );
            }
        }
        
        void parse_export( ParserSession& session, input::Export* pExport )
        {
            //export name( parameter list ) : returnType
            //{
            //  impl;
            //}
            
            //identifier
            parse_identifier( pExport->m_strIdentifier );
            
            //parse optional argument list
            parse_argumentList( session, pExport->m_pParameters );
            
            //return type 
            parse_returnType( session, pExport->m_pReturnType );
            
            parse_comment();
            
            //now get the body
            std::string strBody;
            parse_body( session, strBody );
            
            if( strBody.empty() )
            {
                EG_PARSER_ERROR( "Expected body for export" );
            }
            
            m_pCallback->exportBody( pExport, strBody.c_str() );
        }
        
        input::Context* constructContext( ParserSession& session, input::Context* pParentAction )
        {
            input::Context* pNewAction = session.construct< input::Context >();
            pParentAction->m_elements.push_back( pNewAction );
            pNewAction->m_sourceFile = pParentAction->m_sourceFile;
            return pNewAction;
        }

        void parse_context( ParserSession& session, input::Context* pContext, 
            const boost::filesystem::path& egSourceFile, input::Context::ContextType contextType )
        {
            while( Tok.is( clang::tok::coloncolon ) )
            {
                ConsumeToken();

                if( Tok.is( clang::tok::identifier ) )
                {
                    clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();

                    input::Context* pNestedContext = pContext->findContext( pIdentifier->getName() );
                    if( !pNestedContext )
                    {
                        pNestedContext = constructContext( session, pContext );
                        pNestedContext->m_strIdentifier = pIdentifier->getName();
                    }
                    pContext = pNestedContext;
                    ConsumeToken();
                }
                else
                {
                    EG_PARSER_ERROR( "Expected identifier" );
                }
            }
            
            //set the context type
            pContext->m_contextType = contextType;
            
            parse_comment();
            
            //parse optional argument list
            {
                input::Opaque* pParameters = nullptr;
                parse_argumentList( session, pParameters );
                if( pContext->m_pParams && pParameters )
                {
                    EG_PARSER_ERROR( "Parameters multiply defined" );
                }
                else if( pParameters )
                {
                    pContext->m_pParams = pParameters;
                }
            }
            
            parse_comment();
                                
            //parse optional size specifier
            {
                input::Opaque* pSize = nullptr;
                parse_size( session, pSize );
                if( pContext->m_pSize && pSize )
                {
                    EG_PARSER_ERROR( "Size multiply defined" );
                }
                else if( pSize )
                {
                    pContext->m_pSize = pSize;
                }
            }
            
            parse_comment();
            
            //parse optional inheritance list
            {
                std::vector< input::Opaque* > inheritance;
                parse_inheritance( session, inheritance );
                if( !pContext->m_inheritance.empty() && !inheritance.empty() )
                {
                    EG_PARSER_ERROR( "Inheritance or return type multiply defined" );
                }
                else if( !inheritance.empty() )
                {
                    pContext->m_inheritance = inheritance;
                }
            }
            
            parse_comment();

            //parse the body
            if( Tok.is( clang::tok::l_brace ) )
            {
                BalancedDelimiterTracker T( *this, clang::tok::l_brace );
                T.consumeOpen();
                parse_context_body( session, pContext, egSourceFile );
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

            switch( contextType )
            {
                case input::Context::eAbstract :
                case input::Context::eEvent    :
                case input::Context::eFunction :
                case input::Context::eAction   :
                    break;
                case input::Context::eLink     :
                    add_link_base( session, pContext );
                    break;
                case input::Context::eObject   :
                    break;
                default:
                    EG_PARSER_ERROR( "Unknown context type" );
                    
            }
        }
        
        void add_link_base( ParserSession& session, input::Context* pContext )
        {
            if( pContext->m_inheritance.size() != 1U )
            {
                EG_PARSER_ERROR( "Link requires single inheritance" );
            }
            
            bool bFound = false;
            {
                for( input::Element* pExisting : pContext->m_elements )
                {
                    if( input::Dimension* pDim = dynamic_cast< input::Dimension* >( pExisting ) )
                    {
                        if( pDim->getIdentifier() == EG_LINK_DIMENSION )
                        {
                            bFound = true;
                            break;
                        }
                    }
                }
            }
            if( !bFound )
            {
                //add the nested link reference dimension
                input::Dimension* pLinkDimension = session.construct< input::Dimension >();
                pContext->m_elements.push_back( pLinkDimension );
                pLinkDimension->m_pType = pContext->m_inheritance.front();
                pLinkDimension->m_strIdentifier = EG_LINK_DIMENSION;
            }
        }

        void parse_context_body( ParserSession& session, input::Context* pContext, const boost::filesystem::path& egSourceFile )
        {
            bool bActionDefinition = false;
            
            braceStack.push_back( BraceCount );

            while( !isEofOrEom() )
            {
                input::Context::ContextType contextType = 
                    input::Context::eUnknown;
                if( Tok.is( clang::tok::kw_abstract ) )
                    contextType = input::Context::eAbstract;
                else if( Tok.is( clang::tok::kw_event ) )
                    contextType = input::Context::eEvent;
                else if( Tok.is( clang::tok::kw_function ) )
                    contextType = input::Context::eFunction;
                else if( Tok.is( clang::tok::kw_action ) )
                    contextType = input::Context::eAction;
                else if( Tok.is( clang::tok::kw_link ) )
                    contextType = input::Context::eLink;
                else if( Tok.is( clang::tok::kw_object ) )
                    contextType = input::Context::eObject;
                
                if( input::Context::eUnknown != contextType )
                {
                    switch( contextType )
                    {
                        case input::Context::eAbstract :
                        case input::Context::eEvent    :
                        case input::Context::eFunction :
                        case input::Context::eAction   :
                        case input::Context::eLink     :
                        case input::Context::eObject   :
                            {
                                ConsumeToken();

                                if( Tok.is( clang::tok::identifier ) )
                                {
                                    clang::IdentifierInfo* pIdentifier = Tok.getIdentifierInfo();
                                    input::Context* pNestedContext = pContext->findContext( pIdentifier->getName() );
                                    if( !pNestedContext )
                                    {
                                        pNestedContext = constructContext( session, pContext );
                                        pNestedContext->m_strIdentifier = pIdentifier->getName();
                                    }
                                    ConsumeToken();
                                    parse_context( session, pNestedContext, egSourceFile, contextType );
                                }
                                else
                                {
                                    EG_PARSER_ERROR( "Expected identifier" );
                                }
                            }
                            break;
                    }
                }
                else if(  ( Tok.is( clang::tok::kw_const ) && NextToken().is( clang::tok::kw_dim ) ) || 
                            Tok.is( clang::tok::kw_dim ) )
                {
                    bool bIsConst = false;
                    if( Tok.is( clang::tok::kw_const ) )
                    {
                        bIsConst = true;
                        ConsumeToken();
                    }
                    ConsumeToken();
                    input::Dimension* pDimension = session.construct< input::Dimension >();
                    pDimension->m_bIsConst = bIsConst;
                    pContext->m_elements.push_back( pDimension );
                    parse_dimension( session, pDimension );
                }
                else if( Tok.is( clang::tok::kw_include ) )
                {
                    ConsumeToken();
                    input::Include* pInclude = session.construct< input::Include >();
                    pContext->m_elements.push_back( pInclude );
                    parse_include( session, pInclude );
                }
                else if( Tok.is( clang::tok::kw_using ) )
                {
                    ConsumeToken();
                    input::Using* pUsing = session.construct< input::Using >();
                    pContext->m_elements.push_back( pUsing );
                    parse_using( session, pUsing );
                }
                else if( Tok.is( clang::tok::kw_export ) )
                {
                    ConsumeToken();
                    input::Export* pExport = session.construct< input::Export >();
                    pContext->m_elements.push_back( pExport );
                    parse_export( session, pExport );
                }
                else if( Tok.is( clang::tok::kw_public ) || Tok.is( clang::tok::kw_private ) )
                {
                    //ConsumeToken();
                    input::Visibility* pVisibility = session.construct< input::Visibility >();
                    pContext->m_elements.push_back( pVisibility );
                    parse_visibility( session, pVisibility );
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
                            (
                                !( Tok.is( clang::tok::kw_const ) && NextToken().is( clang::tok::kw_dim ) ) &&
                                !Tok.isOneOf
                                ( 
                                    clang::tok::kw_action, 
                                    clang::tok::kw_object, 
                                    clang::tok::kw_function,
                                    clang::tok::kw_event, 
                                    clang::tok::kw_abstract, 
                                    clang::tok::kw_dim, 
                                    clang::tok::kw_link, 
                                    clang::tok::kw_include,
                                    clang::tok::kw_using,
                                    clang::tok::kw_export,
                                    clang::tok::kw_public,
                                    clang::tok::kw_private
                                 ) 
                             )
                             && 
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
                        
                        if( !strBodyPart.empty() && bActionDefinition )
                        {
                            m_pCallback->contextBody( pContext, strBodyPart.c_str() );
                        }
                    }
                }
            }
            
            VERIFY_RTE( BraceCount == braceStack.back() );
            braceStack.pop_back();
            
            if( bActionDefinition )
            {
                if( pContext->m_definitionFile )
                {
                    if( pContext->m_definitionFile == egSourceFile )
                    {
                        EG_PARSER_ERROR( "Action: " << pContext->getIdentifier() << " multiply defined in: " << egSourceFile.string() );
                    }
                    else
                    {
                        EG_PARSER_ERROR( "Action: " << pContext->getIdentifier() << " multiply defined in: " <<
                           pContext->m_definitionFile.value().string() << " and " << egSourceFile.string() );
                    }
                }
                pContext->m_definitionFile = egSourceFile;
            }
        }
		
		void parse_file( ParserSession& session, input::Root* pRoot, const boost::filesystem::path& egSourceFile )
		{
			parse_context_body( session, pRoot, egSourceFile );
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
    

    void parseEGSourceFile( EG_PARSER_CALLBACK* pCallback,
                const boost::filesystem::path& egSourceFile,
				ParserDiagnosticSystem& diagnosticSystem,
                ParserSession& session, input::Root* pRoot )
    {
        std::shared_ptr< clang::FileManager > pFileManager = 
			get_clang_fileManager( diagnosticSystem );
			
		llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine = 
			get_llvm_diagnosticEngine( diagnosticSystem );
			
        //check file exists
        if( !boost::filesystem::exists( egSourceFile ) )
        {
            THROW_RTE( "File not found: " << egSourceFile );
        }
        
        Stuff stuff( pFileManager, pDiagnosticsEngine, egSourceFile );

        Parser parser( pCallback,
            *stuff.pPreprocessor, 
            *stuff.pSourceManager, 
            stuff.languageOptions, 
            *stuff.pHeaderSearch,
            pDiagnosticsEngine );
        parser.ConsumeToken();
		
        parser.parse_file( session, pRoot, egSourceFile );
    }
    
    struct EG_PARSER_IMPL : EG_PARSER_INTERFACE
    {
        virtual void parseEGSourceFile( EG_PARSER_CALLBACK* pCallback,
                    const boost::filesystem::path& egSourceFile,
                    const boost::filesystem::path& cwdPath, std::ostream& osError,
                    ParserSession& session, input::Root* pRoot )
        {
            VERIFY_RTE_MSG( pCallback, "Invalid parser callback" );
            ParserDiagnosticSystem pds( cwdPath, osError );
            ::eg::parseEGSourceFile( pCallback, egSourceFile, pds, session, pRoot );
        }
    };
    
    extern "C" BOOST_SYMBOL_EXPORT EG_PARSER_IMPL g_parserSymbol;
    EG_PARSER_IMPL g_parserSymbol;
    
}
