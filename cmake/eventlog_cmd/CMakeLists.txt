cmake_minimum_required(VERSION 2.8)

#add_custom_command(
#   OUTPUT ${EG_SOURCE_DIR}/buildinfo/buildinfo.hpp
#   COMMAND c:/workspace/pong/bin/buildinfo.exe --dir c:/workspace/pong/src/eg/buildinfo/
#   COMMENT "Generating build info"
#   VERBATIM
#   )
#   
#add_custom_target( buildgen DEPENDS ${EG_SOURCE_DIR}/buildinfo/buildinfo.hpp  )
#add_custom_target( buildgen ALL DEPENDS ${EG_SOURCE_DIR}/buildinfo/buildinfo.hpp  )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#get eventlog
include( ${EG_ROOT_DIR}/cmake/eventlog/eventlog_include.cmake )

#get the sources
set( EVENTLOG_CMD_SOURCE_DIR ${EG_ROOT_DIR}/src/eventlog_cmd )
file( GLOB EVENTLOG_CMD_SOURCE_DIR_HEADERS ${EVENTLOG_CMD_SOURCE_DIR}/*.hpp )
file( GLOB EVENTLOG_CMD_SOURCE_DIR_SOURCES ${EVENTLOG_CMD_SOURCE_DIR}/*.cpp )
set( EVENTLOG_CMD_SOURCES ${EVENTLOG_CMD_SOURCE_DIR_HEADERS} ${EVENTLOG_CMD_SOURCE_DIR_SOURCES} )
source_group( src FILES ${EVENTLOG_CMD_SOURCES} )

add_executable( eglog 
					${COMMON_INCLUDE_HEADERS}
					${EVENTLOG_CMD_SOURCES}
				)
		
set_target_properties( eglog PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( eglog )
link_eg( eglog )
link_eventlog( eglog )

install( TARGETS eglog DESTINATION bin)
