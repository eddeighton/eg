cmake_minimum_required(VERSION 2.8)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#get the sources
set( BASIC_HOST_SOURCE_DIR ${EG_ROOT_DIR}/src/hosts/cinder )
file( GLOB BASIC_HOST_SOURCE_HEADERS ${BASIC_HOST_SOURCE_DIR}/*.hpp )
file( GLOB BASIC_HOST_SOURCE_SOURCES ${BASIC_HOST_SOURCE_DIR}/*.cpp )
set( BASIC_HOST_SOURCES ${BASIC_HOST_SOURCE_HEADERS} ${BASIC_HOST_SOURCE_SOURCES} )
source_group( src FILES ${BASIC_HOST_SOURCES} )

include_directories( ${BASIC_HOST_SOURCE_DIR} )

add_executable( cinder_host 
					${COMMON_INCLUDE_HEADERS}
					${BASIC_HOST_SOURCES}
				)
		
set_target_properties( cinder_host PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( cinder_host )
link_eg( cinder_host )
#link_eventlog( cinder_host )

install( TARGETS cinder_host DESTINATION bin)
