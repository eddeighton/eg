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
Copyright Deighton Systems Limited (c) 2015
*/

#ifndef EDSASSERT_20_12_2012
#define EDSASSERT_20_12_2012

#include <stdexcept>
#include <sstream>
#include <exception>

#include <boost/current_function.hpp>

#include "require_semicolon.hpp"
#include "backtrace.hpp"

#ifdef _DEBUG
    
#define DEBUG_BREAK( type, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream _os_x;\
        Common::getBackTrace( _os_x );\
        _os_x << msg << "\n";\
        Common::msvcr_debugAssert( type, __FILE__, __LINE__, BOOST_CURRENT_FUNCTION, _os_x.str().c_str() );\
    )

#define ASSERT_MSG( expr, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        if( !( expr ) )\
        {\
            DEBUG_BREAK( _CRT_ASSERT, #expr << "\n\n" << msg );\
        })

#define ASSERT(expr) ASSERT_MSG( expr, "" )

#define THROW( exceptionType, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        DEBUG_BREAK( _CRT_ERROR, msg );\
        std::ostringstream _os2; \
        Common::getBackTrace( _os2 );\
        _os2 << "FILE " << __FILE__ << " LINE:" << __LINE__  << "\nMSG:" << msg; \
        throw exceptionType( _os2.str() );)

#define TERMINATE_IF_NOT( expression, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        if( !( expression ) )\
        {\
            DEBUG_BREAK( _CRT_ERROR, "Verify of: " << #expression << " failed.\n" << msg );\
            std::terminate();\
        })

#else //_DEBUG

#define ASSERT_MSG( expr, msg ) DO_NOTHING_BUT_REQUIRE_SEMI_COLON
#define ASSERT(expr) DO_NOTHING_BUT_REQUIRE_SEMI_COLON

#define THROW( exceptionType, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream _os; \
        _os << msg; \
        throw exceptionType( _os.str() );)

#define TERMINATE_IF_NOT( expression, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        if( !( expression ) )\
        {\
            std::terminate();\
        })

#endif //_DEBUG

#define THROW_RTE( msg ) THROW( std::runtime_error, msg )

#define VERIFY( expression, exceptionType, msg ) \
    DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        if( !( expression ) )\
        {\
            THROW( exceptionType, "Verify of: " << #expression << " failed.\n" << msg );\
        })

#define VERIFY_RTE_MSG( expression, msg ) VERIFY( expression, std::runtime_error, msg )
#define VERIFY_RTE( expression )  VERIFY( expression, std::runtime_error, "" )

#endif //EDSASSERT_20_12_2012
