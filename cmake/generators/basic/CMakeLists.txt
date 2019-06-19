cmake_minimum_required(VERSION 2.8)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

include_directories( ${EG_ROOT_DIR}/src/generators )

#get the sources
set( BASIC_HOST_SOURCE_DIR ${EG_ROOT_DIR}/src/generators/basic )

set( BASIC_HOST_SOURCES  
                ${EG_ROOT_DIR}/src/generators/generators.hpp
                ${EG_ROOT_DIR}/src/generators/generators.cpp
                ${BASIC_HOST_SOURCE_DIR}/main.cpp )

source_group( src FILES ${BASIC_HOST_SOURCES} )

add_executable( basic_host 
					${COMMON_INCLUDE_HEADERS}
					${BASIC_HOST_SOURCES}
				)
		
set_target_properties( basic_host PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( basic_host )
link_eg( basic_host )
#link_eventlog( basic_host )

install( TARGETS basic_host DESTINATION bin)
