
cmake_minimum_required( VERSION 3.1...3.16 )

function( link_eg targetname )
	target_link_libraries( ${targetname} PRIVATE eglib )
endfunction( link_eg )
