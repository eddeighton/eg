


#include <gtest/gtest.h>

#include "driver/environment_string.hpp"


TEST( EnvironmentString, Basic )
{
    std::ostringstream os;
    expand( "test", {}, os );
    ASSERT_EQ( "test", os.str() );
}


TEST( EnvironmentString, BasicMissingString )
{
    std::ostringstream os;
    ASSERT_THROW( expand( "test ${THING}", {}, os ), std::runtime_error );
}


TEST( EnvironmentString, BasicMatch )
{
    std::ostringstream os;
    expand( "test ${THING} the end", { { "THING", "FOUND" } }, os );
    ASSERT_EQ( "test FOUND the end", os.str() );
}

TEST( EnvironmentString, BasicMatch2 )
{
    std::ostringstream os;
    expand( "This ${T2} is ${T1} a string", { { "T1", "F1" }, { "T2", "F2" } }, os );
    ASSERT_EQ( "This F2 is F1 a string", os.str() );
}

TEST( EnvironmentString, BasicMatch3 )
{
    std::ostringstream os;
    expand( "${EG_INSTALL}/third_party/${LIBRARY}", { { "EG_INSTALL", "W:/SomePlace/Somewhere" }, { "LIBRARY", "CoolLib/Stuff" } }, os );
    ASSERT_EQ( "W:/SomePlace/Somewhere/third_party/CoolLib/Stuff", os.str() );
}


TEST( EnvironmentString, BasicMatchBackSlash )
{
    std::ostringstream os;
    expand( "${EG_INSTALL}\\third_party\\${LIBRARY}", { { "EG_INSTALL", "W:\\SomePlace\\Somewhere" }, { "LIBRARY", "CoolLib\\Stuff" } }, os );
    ASSERT_EQ( "W:\\SomePlace\\Somewhere\\third_party\\CoolLib\\Stuff", os.str() );
}
