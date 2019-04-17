cmake_minimum_required(VERSION 2.8)


#
#set( CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP /await /bigobj")

#get boost
#include( ${WORKSPACE_DIR}/cmake/boost.cmake )

#get common
#include( ${MODULE_DIR}/common/cmake/common_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

set( DATABASE_API
    ${EG_API_DIR}/database/database.hpp
    )
    
set( DATABASE_SOURCES
    ${EG_SRC_DIR}/database/database.cpp
    )

include_directories( ${EG_API_DIR} )

add_definitions(-DEG_DATABASE_SHARED_MODULE)

add_library( database SHARED
                    ${DATABASE_API}
                    ${DATABASE_SOURCES}
				)

set_target_properties( database PROPERTIES DEBUG_POSTFIX d )

#link_boost_usual(database)
#link_common( database )
link_clang( database )

install( TARGETS database DESTINATION bin)