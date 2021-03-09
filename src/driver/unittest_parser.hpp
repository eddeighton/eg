
#ifndef UNITTEST_PARSER_8_MARCH_2021
#define UNITTEST_PARSER_8_MARCH_2021

#include "boost/filesystem/path.hpp"

#include <string>
#include <vector>
#include <ostream>

namespace doc
{
    using Identifier = std::vector< std::string >;

    struct UnitTest
    {
        using Vector = std::vector< UnitTest >;
        
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
        std::vector< File > m_files;
    };

    void print( std::ostream& os, const UnitTest& unitTest, bool bShowMarkDown, bool bShowCode );
    void print( std::ostream& os, const UnitTest::Vector& unitTests, bool bShowMarkDown, bool bShowCode );

    bool parseIdentifier( const std::string& strContents, Identifier& identifier, std::ostream& osError );
    bool parseSection( const std::string& strContents, UnitTest::File::Section& section, std::ostream& osError );
    bool parseFileSections( const std::string& strContents, UnitTest::File::Section::Vector& sections, std::ostream& osError );

}

std::ostream& operator<<( std::ostream& os, const doc::Identifier& identifier );
    
#endif //UNITTEST_PARSER_8_MARCH_2021