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

#include "common/escape.hpp"

#include <sstream>
#include <iterator>

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/karma.hpp>

#include "common/assert_verify.hpp"

namespace Common
{

namespace karma = boost::spirit::karma;

template <typename OutputIterator>
struct escaped_string
  : karma::grammar< OutputIterator, std::string() >
{
    escaped_string()
      : escaped_string::base_type(esc_str)
    {
        esc_char.add('\a', "\\a")('\b', "\\b")('\f', "\\f")('\n', "\\n")
                    ('\r', "\\r")('\t', "\\t")('\v', "\\v")('\\', "\\\\")
                    ('\'', "\\\'")('\"', "\\\"")
            ;

        esc_str =   karma::lit('"') 
                << *(esc_char | karma::print | "\\x" << karma::hex) 
                <<  karma::lit('"')
            ;
    }

    karma::rule<OutputIterator, std::string()> esc_str;
    karma::symbols<char, char const*> esc_char;
};

void escapeString( const std::string& strInput, std::ostream& os )
{
  	typedef std::ostream_iterator< char > sink_type;
  	sink_type sink( os, nullptr );

    Common::escaped_string< sink_type > theGrammar;
    if( !boost::spirit::karma::generate( sink, theGrammar, strInput ) )
    {
    	THROW_RTE( "Generating escaped string failed for:" << strInput );
    }
}

std::string escapeString( const std::string& strInput )
{
    std::ostringstream os;
    escapeString( strInput, os );
	return os.str();
}	

}