cmake_minimum_required(VERSION 2.8)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

include_directories( ${EG_ROOT_DIR}/src/generators )

#get the sources
set( CINDER_HOST_SOURCE_DIR ${EG_ROOT_DIR}/src/generators/cinder )

set( CINDER_HOST_SOURCES 
                ${EG_ROOT_DIR}/src/generators/generators.hpp
                ${EG_ROOT_DIR}/src/generators/generators.cpp
                ${CINDER_HOST_SOURCE_DIR}/main.cpp )

source_group( src FILES ${CINDER_HOST_SOURCES} )

include_directories( ${CINDER_HOST_SOURCE_DIR} )

add_executable( cinder_host 
					${COMMON_INCLUDE_HEADERS}
					${CINDER_HOST_SOURCES}
				)
		
set_target_properties( cinder_host PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( cinder_host )
link_eg( cinder_host )

install( TARGETS cinder_host DESTINATION bin)
