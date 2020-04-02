
cmake_minimum_required( VERSION 3.1...3.16 )

find_path( INDICATORS_INCLUDEDIR NAMES indicators PATHS ${EG_THIRD_PARTY_DIR}/indicators/install/include/ )

function( link_indicators targetname )
	target_include_directories( ${targetname} PRIVATE ${INDICATORS_INCLUDEDIR} )
endfunction( link_indicators )