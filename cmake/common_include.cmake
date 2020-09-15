cmake_minimum_required(VERSION 2.8)

find_path( COMMON_INSTALL_PATH share PATHS ${CMAKE_CURRENT_LIST_DIR}../../../common/install )

set( CMAKE_PREFIX_PATH "${COMMON_INSTALL_PATH}/share;${CMAKE_PREFIX_PATH}" )
find_package( common )

function( link_common targetname )
	target_link_libraries( ${targetname} PRIVATE Common::commonlib )
endfunction( link_common )
