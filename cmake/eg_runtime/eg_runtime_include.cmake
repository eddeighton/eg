

cmake_minimum_required(VERSION 3.14.2)


function( link_eg_runtime targetname )
	target_link_libraries( ${targetname} PUBLIC eg_runtime )
endfunction( link_eg_runtime )
