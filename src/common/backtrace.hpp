
/*
Copyright Deighton Systems Limited (c) 2016
*/

#ifndef BACKTRACE_04_AUGUST_2016
#define BACKTRACE_04_AUGUST_2016

#include <ostream>

#include <boost/assert.hpp>

namespace Common
{
    
    void disableDebugErrorPrompts();

    void msvcr_debugAssert( size_t uiType, const char* pszFile, size_t uiLine, const char* pszFunction, const char* pszMsg );

    void getBackTrace( std::ostream& os );

    void debug_break();
}

namespace boost
{
    void assertion_failed( char const * expr, char const * function, char const * file, long line );
    void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line);
}

#endif //BACKTRACE_04_AUGUST_2016

