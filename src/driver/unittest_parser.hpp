
#ifndef UNITTEST_PARSER_8_MARCH_2021
#define UNITTEST_PARSER_8_MARCH_2021

#include "boost/filesystem/path.hpp"

#include <string>
#include <vector>
#include <ostream>
#include <utility>

namespace doc
{
    using Identifier = std::vector< std::string >;
    
    struct Folder
    {
        std::string strOrder, strName;
    };

    struct UnitTest
    {
        using Vector = std::vector< UnitTest >;
        using Ptr = UnitTest*;
        using PtrVector = std::vector< Ptr >;
        using PtrVectorVector = std::vector< PtrVector >;
        
        struct Event
        {
            using Vector = std::vector< Event >;
            enum Type
            {
                eStart,
                eStop,
                eLog,
                eError,
                ePass,
                eFail,
                eOther,
                TOTAL_EVENT_TYPES
            };
            
            Type eventType;
            std::size_t timestamp;
            std::string strValue;
            std::size_t instance;
            std::size_t type;
            
        };
        
        UnitTest( const boost::filesystem::path& rootPath, const boost::filesystem::path& testPath );
        
        struct File
        {
            struct Section
            {
                using Vector = std::vector< Section >;
                
                Identifier m_identifier;
                std::string m_strMarkdown;
                std::string m_code;
            };
            
            boost::filesystem::path m_filePath;
            std::vector< Section > m_sections;
        };

        boost::filesystem::path m_directory;
        Identifier m_ordering, m_headings;
        std::vector< File > m_files;
        Event::Vector m_events;
    };

    void print( std::ostream& os, const UnitTest& unitTest, bool bShowMarkDown, bool bShowCode );
    void print( std::ostream& os, const UnitTest::Vector& unitTests, bool bShowMarkDown, bool bShowCode );

    bool parseFolder( const std::string& strContents, Folder& identifier, std::ostream& osError );
    bool parseIdentifier( const std::string& strContents, Identifier& identifier, std::ostream& osError );
    bool parseSection( const std::string& strContents, UnitTest::File::Section& section, std::ostream& osError );
    bool parseFileSections( const std::string& strContents, UnitTest::File::Section::Vector& sections, std::ostream& osError );

}

std::ostream& operator<<( std::ostream& os, const doc::Identifier& identifier );
    
#endif //UNITTEST_PARSER_8_MARCH_2021