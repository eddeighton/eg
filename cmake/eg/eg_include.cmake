

cmake_minimum_required(VERSION 3.14.2)


function( link_eg targetname )
	target_link_libraries( ${targetname} eglib )
endfunction( link_eg )
