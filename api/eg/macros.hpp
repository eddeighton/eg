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

#ifndef EG_MACROS_24_04_2019
#define EG_MACROS_24_04_2019

#include "common.hpp"
#include "event.hpp"
#include "clock.hpp"

#include <boost/current_function.hpp>

#include <sstream>
#include <string>

#define EG_DO_STUFF_AND_REQUIRE_SEMI_COLON( stuff ) do{ stuff } while( (void)0,0 )
    
#define LOG( __msg )\
    EG_DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream _os_log_;\
        _os_log_ << __msg;\
        std::string __str = _os_log_.str();\
        events::put( "log", clock::cycle( 0 ), __str.data(), __str.size() + 1 );\
        )
        
#define ERR( __msg )\
    EG_DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream _os_err;\
        _os_err << __FILE__ << ":" << __LINE__ << " " << __msg << "\n"; \
        std::string __str = _os_err.str();\
        events::put( "error", clock::cycle( 0 ), __str.data(), __str.size() + 1 );\
        )
        
#define TEST( __expr )\
    EG_DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::string __str = #__expr;\
        if( !( __expr ) )\
        {\
            events::put( "fail", clock::cycle( 0 ), __str.data(), __str.size() + 1 );\
        }\
        else\
        {\
            events::put( "pass", clock::cycle( 0 ), __str.data(), __str.size() + 1 );\
        }\
    )
        
#define TEST_MSG( __expr, __msg )\
    EG_DO_STUFF_AND_REQUIRE_SEMI_COLON( \
        std::ostringstream _os_test_;\
        _os_test_ << #__expr << " " << __msg;\
        std::string __str = _os_test_.str();\
        if( !( __expr ) )\
        {\
            events::put( "fail", clock::cycle( 0 ), __str.data(), __str.size() + 1 );\
        }\
        else\
        {\
            events::put( "pass", clock::cycle( 0 ), __str.data(), __str.size() + 1 );\
        }\
    )
    
#endif //EG_MACROS_24_04_2019