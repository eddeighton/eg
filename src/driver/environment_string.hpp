

#ifndef ENVIRONMENT_STRING_11_06_2017
#define ENVIRONMENT_STRING_11_06_2017

#include <string>
#include <ostream>
#include <map>

using EnvironmentDict = std::map< std::string, std::string >;

void expand( const std::string& str, const EnvironmentDict& env, std::ostream& os );
std::string expand( const std::string& str, const EnvironmentDict& env );

#endif //ENVIRONMENT_STRING_11_06_2017