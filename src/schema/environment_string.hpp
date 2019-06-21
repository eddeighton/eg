

#ifndef ENVIRONMENT_STRING_11_06_2017
#define ENVIRONMENT_STRING_11_06_2017

#include <string>
#include <ostream>
#include <map>
#include <functional>

using StringLookup = std::function< std::string( const std::string& ) >;

void expand( const std::string& str, StringLookup lookup, std::ostream& os );
std::string expand( const std::string& str, StringLookup lookup );

#endif //ENVIRONMENT_STRING_11_06_2017