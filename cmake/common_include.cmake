cmake_minimum_required(VERSION 2.8)

#we assume workspace directory is set

#add_definitions(-D_USE_MATH_DEFINES -D_USE_MATH_DEFINES)
#
#set( COMMON_DIR ${MODULE_DIR}/common )
#
#set( COMMON_INCLUDE_DIR ${COMMON_DIR}/api/common )
#file( GLOB COMMON_INCLUDE_HEADERS ${COMMON_INCLUDE_DIR}/*.hpp ${COMMON_INCLUDE_DIR}/*.h )
#source_group( common FILES ${COMMON_INCLUDE_HEADERS} )
#
#include_directories( ${COMMON_DIR}/api )


find_path( COMMON_INSTALL_PATH share PATHS ${CMAKE_CURRENT_LIST_DIR}../../../common/install )

include( ${COMMON_INSTALL_PATH}/share/common-config.cmake )
#include( ${COMMON_INSTALL_PATH}/share/common-debug.cmake )
#include( ${COMMON_INSTALL_PATH}/share/common-release.cmake )

find_package( common )

function( link_common targetname )
	target_link_libraries( ${targetname} Common::commonlib )
endfunction( link_common )
