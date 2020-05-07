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


#include "eg_compiler/codegen/codegen.hpp"

#include "eg_compiler/eg.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/interface.hpp"

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>


namespace eg
{

    //generate include directives
    void generateInterfaceIncludes( std::ostream& os, const interface::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser )
    {
        interface::Element::Collector< input::Include > includes;
        pRoot->visit( includes );
        std::set< boost::filesystem::path > systemIncludes, userIncludes;
        std::vector< boost::filesystem::path > systemIncludesOrdered, userIncludesOrdered;
        
        for( const boost::filesystem::path& hostIncludePath : hostIncludesUser )
        {
            if( userIncludes.find( hostIncludePath ) == userIncludes.end() )
            {
                userIncludes.insert( hostIncludePath );
                userIncludesOrdered.push_back( hostIncludePath );
            }
        }
        for( const boost::filesystem::path& hostIncludePath : hostIncludesSystem )
        {
            if( systemIncludes.find( hostIncludePath ) == systemIncludes.end() )
            {
                systemIncludes.insert( hostIncludePath );
                systemIncludesOrdered.push_back( hostIncludePath );
            }
        }
        
        for( const input::Include* pInclude : includes() )
        {
            const boost::filesystem::path& p = pInclude->getIncludeFilePath();
            if( pInclude->isSystemInclude() )
            {
                if( systemIncludes.find( p ) == systemIncludes.end() )
                {
                    systemIncludes.insert( p );
                    systemIncludesOrdered.push_back( p );
                }
            }
            else
            {
                if( userIncludes.find( p ) == userIncludes.end() )
                {
                    userIncludes.insert( p );
                    userIncludesOrdered.push_back( p );
                }
            }
        }
        
        for( const boost::filesystem::path& p : systemIncludesOrdered )
        {
            os << "#include " << p.string() << "\n";
        }
        for( const boost::filesystem::path& p : userIncludesOrdered )
        {
            os << "#include \"" << p.string() << "\"\n";
        }
    }
    
    
    void generateIncludeHeader( std::ostream& os, const interface::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser )
    {
        generateIncludeGuard( os, "INCLUDES" );
        
        generateInterfaceIncludes( os, pRoot, hostIncludesSystem, hostIncludesUser );
        
        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }

}