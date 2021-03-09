

#include "driver/unittest_parser.hpp"

#include <gtest/gtest.h>

#include <iostream>
#include <sstream>

using namespace std::string_literals;

TEST( FolderParser, Folder_Valid )
{
    std::vector< std::pair< std::string, doc::Folder > > validStrings = 
    {
        { "a_b"s, { "a"s, "b"s } },
        { "1_2"s, { "1"s, "2"s } },
        { "abc123_def456"s, { "abc123"s, "def456"s } }
    };
    
    for( const std::pair< std::string, doc::Folder >& p : validStrings )
    {
        std::ostringstream osError;
        doc::Folder folder;
        const bool bResult = doc::parseFolder( p.first, folder, osError );
        ASSERT_TRUE( bResult );
        ASSERT_EQ( folder.strOrder,     p.second.strOrder );
        ASSERT_EQ( folder.strName,      p.second.strName );
    }
}

TEST( FolderParser, Folder_InValid )
{
    std::vector< std::string > validStrings = 
    {
        { ""s },
        { "a"s },
        { "_b"s },
        { "_b_"s },
        { "_2"s },
        { "abc123"s },
        { "___"s }
    };
    
    for( const std::string& p : validStrings )
    {
        std::ostringstream osError;
        doc::Folder folder;
        const bool bResult = doc::parseFolder( p, folder, osError );
        ASSERT_FALSE( bResult );
    }
}

TEST( IdentifierParser, Identifier_Empty )
{
    std::ostringstream osError;
    doc::Identifier identifier;
    const bool bResult = doc::parseIdentifier( "", identifier, osError );
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
        doc::Identifier identifier;
        const bool bResult = doc::parseIdentifier( str, identifier, osError );
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
        doc::Identifier identifier;
        const bool bResult = doc::parseIdentifier( str, identifier, osError );
        ASSERT_FALSE( bResult ) << str << " : " << identifier;
    }
}

TEST( IdentifierParser, Identifier_Multi_Valid )
{
    std::vector< std::pair< std::string, doc::Identifier > > validStrings = 
    {
        {"a.a"s,            doc::Identifier{ "a"s, "a"s } },
        {"a.b.c.d.e"s,      doc::Identifier{ "a"s, "b"s, "c"s, "d"s, "e"s } },
        {"a_.b_.c_.d_.e_"s, doc::Identifier{ "a_"s, "b_"s, "c_"s, "d_"s, "e_"s } }
    };
    
    for( const std::pair< std::string, doc::Identifier >& p : validStrings )
    {
        std::ostringstream osError;
        doc::Identifier identifier;
        const bool bResult = doc::parseIdentifier( p.first, identifier, osError );
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
        doc::Identifier identifier;
        const bool bResult = doc::parseIdentifier( str, identifier, osError );
        ASSERT_FALSE( bResult ) << str;
    }
}

TEST( SectionParser, Section_Valid )
{
    const std::vector< std::pair< std::string, doc::UnitTest::File::Section > > sections = 
    {
        { "/*a.b.c HelloWorld*/foobar"s, doc::UnitTest::File::Section{ doc::Identifier{ "a"s, "b"s, "c"s }, " HelloWorld"s, "foobar"s } }
    };
    
    for( const std::pair< std::string, doc::UnitTest::File::Section >& p : sections )
    {
        doc::UnitTest::File::Section section;
        std::ostringstream osError;
        const bool bResult = doc::parseSection( p.first, section, osError );
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
    doc::UnitTest::File::Section::Vector sections;
    const bool bResult = doc::parseFileSections( strEmpty, sections, osError );
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
    doc::UnitTest::File::Section::Vector sections;
    const bool bResult = doc::parseFileSections( strNoSection, sections, osError );
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
    doc::UnitTest::File::Section::Vector sections;
    const bool bResult = doc::parseFileSections( strSingleSection, sections, osError );    
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
    doc::UnitTest::File::Section::Vector sections;
    const bool bResult = doc::parseFileSections( strSingleSection, sections, osError );    
    ASSERT_TRUE( bResult );
    ASSERT_EQ( sections.size(), 2U );
}
