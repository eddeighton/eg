
cmake_minimum_required(VERSION 2.8)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

include_directories( ${EG_ROOT_DIR}/src/generators )

#get the sources
set( DYNAMIC_HOST_SOURCE_DIR ${EG_ROOT_DIR}/src/generators/dynamic )

set( DYNAMIC_HOST_SOURCES 
                ${EG_ROOT_DIR}/src/generators/generators.hpp
                ${EG_ROOT_DIR}/src/generators/generators.cpp
                ${DYNAMIC_HOST_SOURCE_DIR}/main.cpp
                ${DYNAMIC_HOST_SOURCE_DIR}/dynamic_interface.cpp )

source_group( src FILES ${DYNAMIC_HOST_SOURCES} )

include_directories( ${DYNAMIC_HOST_SOURCE_DIR} )

add_executable( dynamic_host 
					${COMMON_INCLUDE_HEADERS}
					${DYNAMIC_HOST_SOURCES}
				)
		
set_target_properties( dynamic_host PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( dynamic_host )
link_eg( dynamic_host )

install( TARGETS dynamic_host DESTINATION bin)
