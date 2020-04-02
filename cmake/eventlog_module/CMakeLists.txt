
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common/common_include.cmake )

#get eventlog lib
include( ${EG_ROOT_DIR}/cmake/eventlog/eventlog_include.cmake )

set( EVENTLOG_PLUGIN_API ${EG_API_DIR}/eventlog/eventlog_api.hpp )

set( EVENTLOG_SOURCE_DIR ${EG_ROOT_DIR}/src/eventlog )

set( EVENTLOG_SOURCE_SOURCES
    ${EVENTLOG_SOURCE_DIR}/event_log_module.cpp )

set( EVENTLOG_MODULE_SOURCES ${EVENTLOG_PLUGIN_API} ${EVENTLOG_SOURCE_SOURCES} )

include_directories( ${EVENTLOG_SOURCE_DIR} )

add_definitions(-DEG_EVENTLOG_SHARED_MODULE)

add_library( eventlog SHARED ${EVENTLOG_MODULE_SOURCES} )
		
set_target_properties( eventlog PROPERTIES FOLDER log )

set_target_properties( eventlog PROPERTIES DEBUG_POSTFIX d )

link_boost( eventlog program_options )
link_boost( eventlog filesystem )
link_boost( eventlog system )
link_boost( eventlog date_time )
link_boost( eventlog iostreams )
link_common( eventlog )
link_eventlog( eventlog )

install( FILES ${EVENTLOG_PLUGIN_API} DESTINATION include/eventlog_api )
install( TARGETS eventlog DESTINATION bin)