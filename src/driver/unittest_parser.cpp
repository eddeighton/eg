
#include "unittest_parser.hpp"

#include "common/grammar.hpp"
#include "common/file.hpp"
#include "common/assert_verify.hpp"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

std::ostream& operator<<( std::ostream& os, const doc::Identifier& identifier )
{
    bool bFirst = true;
    for( const std::string& str : identifier )
    {
        if( !bFirst )
            os << ".";
        else
            bFirst = false;
        os << str;
    }
    return os;
}

BOOST_FUSION_ADAPT_STRUCT
(
    doc::UnitTest::File::Section,
    (doc::Identifier, m_identifier)
    (std::string, m_strMarkdown)
    (std::string, m_code)
)
    
namespace doc
{
    
void print( std::ostream& os, const UnitTest& unitTest, bool bShowMarkDown, bool bShowCode )
{
    os << "Unit Test: " << unitTest.m_directory.string() << "\n";
    for( const UnitTest::File& file : unitTest.m_files )
    {
        os << "  File: " << file.m_filePath.string() << "\n";
        for( const UnitTest::File::Section& section : file.m_sections )
        {
            os << "    Section: " << section.m_identifier << "\n";
            if( bShowMarkDown )
                os << "    Markdown:\n" << section.m_strMarkdown << "\n";
            if( bShowCode )
                os << "    Code:\n" << section.m_code << "\n";
        }
    }
}

void print( std::ostream& os, const UnitTest::Vector& unitTests, bool bShowMarkDown, bool bShowCode )
{
    for( const UnitTest& unitTest : unitTests )
    {
        print( os, unitTest, bShowMarkDown, bShowCode );
    }
}

template< typename Iterator >
class IdentifierGrammar : public boost::spirit::qi::grammar< Iterator, doc::Identifier() >
{
public:
    IdentifierGrammar() : IdentifierGrammar::base_type( m_main_rule, "identifier" )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        using boost::phoenix::insert;
        using boost::phoenix::end;
        using boost::phoenix::begin;

        m_identifier_grammar = lexeme[ char_( "a-zA-Z" )[ push_back( _val, qi::_1 ) ] >> 
                        *(char_( "a-zA-Z0-9_" )[ push_back( _val, qi::_1 ) ] ) ];

        m_main_rule =
            m_identifier_grammar[ push_back( _val, qi::_1 ) ] >> 
            *( char_( '.' ) >> m_identifier_grammar[ push_back( _val, qi::_1 ) ] );
    }

private:
    boost::spirit::qi::rule< Iterator, std::string() > m_identifier_grammar;
public:
    boost::spirit::qi::rule< Iterator, doc::Identifier() > m_main_rule;
};

bool parseIdentifier( const std::string& strContents, Identifier& identifier, std::ostream& osError )
{
    boost::spirit_ext::ParseResult parseResult( strContents.begin(), strContents.end() );
    
    boost::spirit_ext::invoke_parser_noskip
        < 
            IdentifierGrammar< boost::spirit_ext::LinePosIterator >, 
            Identifier
        >
        ( 
            parseResult, 
            identifier, 
            osError 
        );
        
    return parseResult.fullParse();
}


template< typename Iterator >
class SectionGrammar : public boost::spirit::qi::grammar< Iterator, UnitTest::File::Section() >
{
public:
    SectionGrammar() : SectionGrammar::base_type( m_main_rule )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        using boost::phoenix::insert;
        using boost::phoenix::end;
        using boost::phoenix::begin;

        m_main_rule = 
            //read the c comment
            "/*" >> 
                //extract the identifier
                m_identifier_grammar[ at_c< 0 >( _val ) = boost::spirit::qi::_1 ] >> //m_identifier
                
                //then capture the markdown
                *( boost::spirit::ascii::char_ - "*/")
                    [ boost::phoenix::push_back( at_c< 1 >( _val ), boost::spirit::qi::_1 ) ] >> //m_strMarkdown
            "*/" >>
            //get the following code
            +( boost::spirit::ascii::char_ - "/*")
                [ boost::phoenix::push_back( at_c< 2 >( _val ), boost::spirit::qi::_1 ) ]; //m_code
    }
    
private:
    IdentifierGrammar< Iterator > m_identifier_grammar;
public:
    boost::spirit::qi::rule< Iterator, UnitTest::File::Section() > m_main_rule;
};

bool parseSection( const std::string& strContents, UnitTest::File::Section& section, std::ostream& osError )
{
    boost::spirit_ext::ParseResult parseResult( strContents.begin(), strContents.end() );
    
    boost::spirit_ext::invoke_parser_noskip
        < 
            SectionGrammar< boost::spirit_ext::LinePosIterator >, 
            UnitTest::File::Section
        >
        ( 
            parseResult, 
            section, 
            osError 
        );
        
    return parseResult.fullParse();
}

template< typename Iterator >
class SectionVectorGrammar : public boost::spirit::qi::grammar< Iterator, UnitTest::File::Section::Vector() >
{
public:
    SectionVectorGrammar() : SectionVectorGrammar::base_type( m_main_rule )
    {
        using namespace boost::spirit;
        using namespace boost::spirit::qi;
        using namespace boost::phoenix;

        m_main_rule =
            //skip until first c comment
            *( boost::spirit::ascii::char_ - "/*") >>
            *m_section_grammar[ push_back( _val, qi::_1 ) ];
    }

private:
    SectionGrammar< Iterator > m_section_grammar;
public:
    boost::spirit::qi::rule< Iterator, UnitTest::File::Section::Vector() > m_main_rule;
};

bool parseFileSections( const std::string& strContents, UnitTest::File::Section::Vector& sections, std::ostream& osError )
{
    boost::spirit_ext::ParseResult parseResult( strContents.begin(), strContents.end() );
    
    boost::spirit_ext::invoke_parser_noskip
        < 
            SectionVectorGrammar< boost::spirit_ext::LinePosIterator >, 
            UnitTest::File::Section::Vector
        >
        ( 
            parseResult, 
            sections, 
            osError 
        );
        
    return parseResult.bParseSucceeded;
}

}