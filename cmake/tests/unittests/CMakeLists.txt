
cmake_minimum_required(VERSION 3.14.2)


#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get gtest
include( ${EG_ROOT_DIR}/cmake/gtest_include.cmake )

#get the unit test utils
#include( ${MODULE_DIR}/test_utils/cmake/test_utils_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#get driver source
set( DRIVER_SRC
    ${EG_ROOT_DIR}/src/driver/environment_string.hpp
    ${EG_ROOT_DIR}/src/driver/environment_string.cpp )

set( EG_UNITTEST_DIR ${EG_ROOT_DIR}/tests/unittests )

file( GLOB UNITTEST_TEST_SOURCES ${EG_UNITTEST_DIR}/*.cpp )
#foreach( F ${DATABASE_TEST_SOURCES})
#    message( STATUS "Found unit test: " ${F} )
#endforeach( F )
source_group( eg/tests FILES ${UNITTEST_TEST_SOURCES} )

add_executable( eg_tests
                    ${DRIVER_SRC}
					${EG_SOURCES}
					${UNITTEST_TEST_SOURCES}
				)

##configure all debug builds to have 'd' postfix
set_target_properties( eg_tests PROPERTIES DEBUG_POSTFIX d )
set_target_properties( eg_tests PROPERTIES FOLDER eg )

link_boost( eg_tests filesystem )
link_boost( eg_tests iostreams )
link_boost( eg_tests serialization )
link_clang( eg_tests )
link_eg( eg_tests )
link_gtest( eg_tests )
link_gtest_main( eg_tests )

install( TARGETS eg_tests DESTINATION bin)
