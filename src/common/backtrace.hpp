//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.


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

