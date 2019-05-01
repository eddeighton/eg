cmake_minimum_required(VERSION 2.8)


#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get eventlog lib
include( ${EG_ROOT_DIR}/cmake/eventlog/eventlog_include.cmake )

set( EVENTLOG_PLUGIN_API ${EG_API_DIR}/eventlog/eventlog_api.hpp )

set( EVENTLOG_SOURCE_DIR ${EG_ROOT_DIR}/src/eventlog )

set( EVENTLOG_SOURCE_SOURCES
    ${EVENTLOG_SOURCE_DIR}/event_log_module.cpp )

set( EVENTLOG_MODULE_SOURCES ${EVENTLOG_PLUGIN_API} ${EVENTLOG_SOURCE_SOURCES} )

include_directories( ${EVENTLOG_SOURCE_DIR} )

add_definitions(-DEG_EVENTLOG_SHARED_MODULE)

add_library( eventlog SHARED
					${COMMON_INCLUDE_HEADERS}
                    ${EVENTLOG_MODULE_SOURCES}
				)
		
set_target_properties( eventlog PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( eventlog )
link_eventlog( eventlog )

install( TARGETS eventlog DESTINATION bin)