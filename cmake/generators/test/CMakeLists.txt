cmake_minimum_required(VERSION 2.8)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

include_directories( ${EG_ROOT_DIR}/src/generators )

#get the sources
set( TEST_HOST_SOURCE_DIR ${EG_ROOT_DIR}/src/generators/test )

set( TEST_HOST_SOURCES 
                ${EG_ROOT_DIR}/src/generators/generators.hpp
                ${EG_ROOT_DIR}/src/generators/generators.cpp
                ${TEST_HOST_SOURCE_DIR}/main.cpp)

source_group( src FILES ${TEST_HOST_SOURCES} )

add_executable( test_host 
					${COMMON_INCLUDE_HEADERS}
					${TEST_HOST_SOURCES}
				)
		
set_target_properties( test_host PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( test_host )
link_eg( test_host )

install( TARGETS test_host DESTINATION bin)
