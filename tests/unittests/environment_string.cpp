


#include <gtest/gtest.h>

#include "driver/environment_string.hpp"

void expand_wrap( const std::string& str, const std::map< std::string, std::string >& env, std::ostream& os )
{
    ::expand( str, 
        [ &envref = env ]( const std::string& strKey )
        { 
            std::map< std::string, std::string >::const_iterator iFind = envref.find( strKey );
            if( iFind == envref.end() ) throw std::runtime_error( "it cant work captain!" );
            return iFind->second;
        }, os );
}


TEST( EnvironmentString, Basic )
{
    std::ostringstream os;
    expand_wrap( "test", {}, os );
    ASSERT_EQ( "test", os.str() );
}

TEST( EnvironmentString, BasicMissingString )
{
    std::ostringstream os;
    ASSERT_THROW( expand_wrap( "test ${THING}", {}, os ), std::runtime_error );
}

TEST( EnvironmentString, BasicMatch )
{
    std::ostringstream os;
    expand_wrap( "test ${THING} the end", { { "THING", "FOUND" } }, os );
    ASSERT_EQ( "test FOUND the end", os.str() );
}

TEST( EnvironmentString, BasicMatch2 )
{
    std::ostringstream os;
    expand_wrap( "This ${T2} is ${T1} a string", { { "T1", "F1" }, { "T2", "F2" } }, os );
    ASSERT_EQ( "This F2 is F1 a string", os.str() );
}

TEST( EnvironmentString, BasicMatch3 )
{
    std::ostringstream os;
    expand_wrap( "${EG_INSTALL}/third_party/${LIBRARY}", { { "EG_INSTALL", "W:/SomePlace/Somewhere" }, { "LIBRARY", "CoolLib/Stuff" } }, os );
    ASSERT_EQ( "W:/SomePlace/Somewhere/third_party/CoolLib/Stuff", os.str() );
}


TEST( EnvironmentString, BasicMatchBackSlash )
{
    std::ostringstream os;
    expand_wrap( "${EG_INSTALL}\\third_party\\${LIBRARY}", { { "EG_INSTALL", "W:\\SomePlace\\Somewhere" }, { "LIBRARY", "CoolLib\\Stuff" } }, os );
    ASSERT_EQ( "W:\\SomePlace\\Somewhere\\third_party\\CoolLib\\Stuff", os.str() );
}
