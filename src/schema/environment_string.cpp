

#include "environment_string.hpp"

#include "common/assert_verify.hpp"

#include <sstream>
#include <regex>
#include <iterator>

void expand( const std::string& str, StringLookup lookup, std::ostream& os )
{
    //https://stackoverflow.com/questions/1902681/expand-file-names-that-have-environment-variables-in-their-path
    static std::regex envStringRegex( "\\$\\{([^}]+)\\}" );
    
    std::string::const_iterator iStrIter = str.begin();
    
    for( std::sregex_iterator iter( str.begin(), str.end(), envStringRegex );
        iter != std::sregex_iterator(); ++iter )
    {
        auto i    = iter->begin();
        auto iEnd = iter->end();
        
        if( std::distance( i, iEnd ) != 2U )
        {
            THROW_RTE( "Invalid submatch for string: " << str );
        }
        
        std::string::const_iterator matchBegin  = i->first;
        std::string::const_iterator matchEnd    = i->second;
        
        std::copy( iStrIter, matchBegin, std::ostream_iterator< char >( os ) );
        
        ++i;
        
        os << lookup( i->str() );
        
        iStrIter = matchEnd;
    }
    
    std::copy( iStrIter, str.end(), std::ostream_iterator< char >( os ) );
}


std::string expand( const std::string& str, StringLookup lookup )
{
    std::ostringstream os;
    expand( str, lookup, os );
    return os.str();
}