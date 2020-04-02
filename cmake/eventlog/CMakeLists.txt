
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common/common_include.cmake )

set( EVENTLOG_SOURCE_DIR ${EG_ROOT_DIR}/src/eventlog )

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

add_library( eventloglib ${EVENTLOG_SOURCE_DIR_SOURCES} )

set_target_properties( eventloglib PROPERTIES FOLDER log )
		
set_target_properties( eventloglib PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( eventloglib )
link_common( eventloglib )

install( FILES ${EVENTLOG_SOURCE_HEADERS} DESTINATION include/eventlog )
install( TARGETS eventloglib DESTINATION lib )