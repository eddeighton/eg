

#include "driver/unittest_parser.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

using namespace std::string_literals;

TEST( IdentifierParser, Identifier_Empty )
{
    std::ostringstream osError;
    Identifier identifier;
    const bool bResult = parseIdentifier( "", identifier, osError );
    ASSERT_TRUE( !bResult );
}

TEST( IdentifierParser, Identifier_SingleValid )
{
    std::vector< std::string > validStrings = 
    {
        "a"s,
        "A"s,
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"s,
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"s,
    };
    
    for( const std::string& str : validStrings )
    {
        std::ostringstream osError;
        Identifier identifier;
        const bool bResult = parseIdentifier( str, identifier, osError );
        ASSERT_TRUE( bResult );
        ASSERT_EQ( identifier.size(), 1U );
        ASSERT_EQ( identifier.back(), str );
    }
}

TEST( IdentifierParser, Identifier_SingleInValid )
{
    std::vector< std::string > validStrings = 
    {
        ""s,
        " "s,
        "_"s,
        "_A"s,
        "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"s,
        "_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"s,
        "1a"s,
        "A$"s,
        "ab cdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"s,
        "aaaaaaaaa\naaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"s,
    };
    
    for( const std::string& str : validStrings )
    {
        std::ostringstream osError;
        Identifier identifier;
        const bool bResult = parseIdentifier( str, identifier, osError );
        ASSERT_FALSE( bResult ) << str << " : " << identifier;
    }
}

TEST( IdentifierParser, Identifier_Multi_Valid )
{
    std::vector< std::pair< std::string, Identifier > > validStrings = 
    {
        {"a.a"s,            Identifier{ "a"s, "a"s } },
        {"a.b.c.d.e"s,      Identifier{ "a"s, "b"s, "c"s, "d"s, "e"s } },
        {"a_.b_.c_.d_.e_"s, Identifier{ "a_"s, "b_"s, "c_"s, "d_"s, "e_"s } }
    };
    
    for( const std::pair< std::string, Identifier >& p : validStrings )
    {
        std::ostringstream osError;
        Identifier identifier;
        const bool bResult = parseIdentifier( p.first, identifier, osError );
        ASSERT_TRUE( bResult );
        ASSERT_EQ( identifier, p.second );
    }
}

TEST( IdentifierParser, Identifier_Multi_InValid )
{
    std::vector< std::string > validStrings = 
    {
        ".a"s,
        "a..b"s,
        "a."s,
        "a. b"s,
        "a .b"s
    };
    
    for( const std::string& str : validStrings )
    {
        std::ostringstream osError;
        Identifier identifier;
        const bool bResult = parseIdentifier( str, identifier, osError );
        ASSERT_FALSE( bResult ) << str;
    }
}

TEST( SectionParser, Section_Valid )
{
    const std::vector< std::pair< std::string, UnitTest::File::Section > > sections = 
    {
        { "/*a.b.c HelloWorld*/foobar"s, UnitTest::File::Section{ Identifier{ "a"s, "b"s, "c"s }, " HelloWorld"s, "foobar"s } }
    };
    
    for( const std::pair< std::string, UnitTest::File::Section >& p : sections )
    {
        UnitTest::File::Section section;
        std::ostringstream osError;
        const bool bResult = parseSection( p.first, section, osError );
        ASSERT_TRUE( bResult );
        ASSERT_EQ( section.m_identifier, p.second.m_identifier );
        ASSERT_EQ( section.m_strMarkdown, p.second.m_strMarkdown );
        ASSERT_EQ( section.m_code, p.second.m_code );
    }
}

TEST( UnitTestParser, Empty )
{
    const std::string strEmpty = 
    R"()";
    
    std::ostringstream osError;
    UnitTest::File::Section::Vector sections;
    const bool bResult = parseFileSections( strEmpty, sections, osError );
    ASSERT_TRUE( bResult );
    ASSERT_EQ( sections.size(), 0U );
}


TEST( UnitTestParser, NoSection )
{
    const std::string strNoSection = 
    R"(
        action Actual
        {
            function code() : void
            {
                std::cout << "hello world" << std::endl;
            }
        }
    )";
    
    std::ostringstream osError;
    UnitTest::File::Section::Vector sections;
    const bool bResult = parseFileSections( strNoSection, sections, osError );
    ASSERT_TRUE( bResult );
    ASSERT_EQ( sections.size(), 0U );
}

TEST( UnitTestParser, SingleSection )
{
    const std::string strSingleSection = 
    R"(
        stuff...
        /*a.b.c.d
            
            Title
            =====
            
            Markdown stuff...
        */
        action Actual
        {
            function code() : void
            {
                std::cout << "hello world" << std::endl;
            }
        }
    )";

    std::ostringstream osError;
    UnitTest::File::Section::Vector sections;
    const bool bResult = parseFileSections( strSingleSection, sections, osError );    
    ASSERT_TRUE( bResult );
    ASSERT_EQ( sections.size(), 1U );
}



TEST( UnitTestParser, MultiSections )
{
    const std::string strSingleSection = 
    R"(
        stuff...
        /*a.b.c.d
            
            Title
            =====
            
            Markdown stuff...
        */
        action Actual
        {
            function code() : void
            {
                std::cout << "hello world" << std::endl;
            }
        }
        /*a.b.c.e
            
            Title
            =====
            
            Markdown stuff...
        */
        action SecondThing
        {
            function code() : void
            {
                std::cout << "hello world" << std::endl;
            }
        }
    )";

    std::ostringstream osError;
    UnitTest::File::Section::Vector sections;
    const bool bResult = parseFileSections( strSingleSection, sections, osError );    
    ASSERT_TRUE( bResult );
    ASSERT_EQ( sections.size(), 2U );
}
