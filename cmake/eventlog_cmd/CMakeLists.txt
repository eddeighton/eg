
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common/common_include.cmake )

#get eventlog
include( ${EG_ROOT_DIR}/cmake/eventlog/eventlog_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

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
		
set_target_properties( eglog PROPERTIES FOLDER log )

set_target_properties( eglog PROPERTIES DEBUG_POSTFIX d )

link_boost( eglog program_options )
link_boost( eglog filesystem )
link_boost( eglog system )
link_boost( eglog date_time )
link_common( eglib )
link_eventlog( eglog )
link_eg( eglog )

install( TARGETS eglog DESTINATION bin)
