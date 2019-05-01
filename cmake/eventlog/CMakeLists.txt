cmake_minimum_required(VERSION 2.8)


#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

set( EVENTLOG_SOURCE_DIR ${EG_ROOT_DIR}/src/eventlog )
#file( GLOB EVENTLOG_SOURCE_HEADERS ${EVENTLOG_SOURCE_DIR}/*.hpp )
#file( GLOB EVENTLOG_SOURCE_SOURCES ${EVENTLOG_SOURCE_DIR}/*.cpp )

set( EVENTLOG_COMMON_SOURCE
    ${EG_ROOT_DIR}/src/common/assert_verify.hpp
    ${EG_ROOT_DIR}/src/common/backtrace.hpp
    ${EG_ROOT_DIR}/src/common/backtrace.cpp
    ${EG_ROOT_DIR}/src/common/file.hpp
    ${EG_ROOT_DIR}/src/common/file.cpp )

set( EVENTLOG_SOURCE_HEADERS
    ${EVENTLOG_SOURCE_DIR}/client.hpp  
    ${EVENTLOG_SOURCE_DIR}/event.hpp       
    ${EVENTLOG_SOURCE_DIR}/file.hpp  
    ${EVENTLOG_SOURCE_DIR}/iterator.hpp  
    ${EVENTLOG_SOURCE_DIR}/log.hpp  
    ${EVENTLOG_SOURCE_DIR}/pid.hpp  
    ${EVENTLOG_SOURCE_DIR}/server.hpp  
    ${EVENTLOG_SOURCE_DIR}/sharedMemory.hpp )

set( EVENTLOG_SOURCE_SOURCES
    ${EVENTLOG_SOURCE_DIR}/client.cpp        
    ${EVENTLOG_SOURCE_DIR}/file.cpp  
    ${EVENTLOG_SOURCE_DIR}/iterator.cpp  
    ${EVENTLOG_SOURCE_DIR}/log.cpp  
    ${EVENTLOG_SOURCE_DIR}/pid.cpp  
    ${EVENTLOG_SOURCE_DIR}/server.cpp  
    ${EVENTLOG_SOURCE_DIR}/sharedMemory.cpp )

set( EVENTLOG_SOURCE_DIR_SOURCES ${EVENTLOG_SOURCE_HEADERS} ${EVENTLOG_SOURCE_SOURCES} )

include_directories( ${EVENTLOG_SOURCE_DIR} )

add_library( eventloglib
					${EVENTLOG_COMMON_SOURCE}
                    ${EVENTLOG_SOURCE_DIR_SOURCES}
				)
		
set_target_properties( eventloglib PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( eventloglib )

install( TARGETS eventloglib DESTINATION bin)