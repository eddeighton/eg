#ifndef ESCAPE_17_MAY_2015
#define ESCAPE_17_MAY_2015

#include <string>
#include <ostream>

namespace Common
{

void escapeString( const std::string& strInput, std::ostream& os );
std::string escapeString( const std::string& strInput );

}

#endif //ESCAPE_17_MAY_2015