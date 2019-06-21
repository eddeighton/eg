

cmake_minimum_required(VERSION 3.14.2)

include_directories( ${EG_ROOT_DIR}/src/schema )

function( link_schemalib targetname )
	target_link_libraries( ${targetname} schemalib )
endfunction( link_schemalib )
