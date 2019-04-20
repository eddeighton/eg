cmake_minimum_required(VERSION 2.8)


#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

set( EVENTLOG_SOURCE_DIR ${EG_ROOT_DIR}/src/eventlog )
file( GLOB EVENTLOG_SOURCE_HEADERS ${EVENTLOG_SOURCE_DIR}/*.hpp )
file( GLOB EVENTLOG_SOURCE_SOURCES ${EVENTLOG_SOURCE_DIR}/*.cpp )

set( EVENTLOG_SOURCE_DIR_SOURCES ${EVENTLOG_SOURCE_HEADERS} ${EVENTLOG_SOURCE_SOURCES} )

include_directories( ${EVENTLOG_SOURCE_DIR} )

add_library( eventloglib
					${COMMON_INCLUDE_HEADERS}
                    ${EVENTLOG_SOURCE_DIR_SOURCES}
				)
		
set_target_properties( eventloglib PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( eventloglib )

install( TARGETS eventloglib DESTINATION bin)