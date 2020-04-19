cmake_minimum_required(VERSION 2.8)

find_path( COMMON_INSTALL_PATH share PATHS ${CMAKE_CURRENT_LIST_DIR}../../../common/install )

include( ${COMMON_INSTALL_PATH}/share/common-config.cmake )

find_package( common )

function( link_common targetname )
	target_link_libraries( ${targetname} PUBLIC Common::commonlib )
endfunction( link_common )
