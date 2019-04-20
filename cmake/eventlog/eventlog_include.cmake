

include_directories( ${PROJECT_DIR}/src )

link_directories( ${PROJECT_DIR}/bin )

set( EVENTLOG_SOURCE_DIR ${PROJECT_DIR}/src/eventlog )
include_directories( ${EVENTLOG_SOURCE_DIR} )

function( link_eventlog targetname )
	target_link_libraries( ${targetname} eventloglib )
endfunction( link_eventlog )