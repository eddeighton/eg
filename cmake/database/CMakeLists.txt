
cmake_minimum_required(VERSION 3.14.2)
#
#set( CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP /await /bigobj")
#set( CMAKE_CXX_FLAGS "/MP /GR /W3 /EHsc /std:c++17" )


#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/database/eg_sources_include.cmake )

add_definitions(-DEG_DATABASE_SHARED_MODULE)

add_library( database SHARED
                    ${DATABASE_API}
                    ${EG_SOURCES}
				)

set_target_properties( database PROPERTIES DEBUG_POSTFIX d )

link_boost( database filesystem )
link_boost( database iostreams )
link_boost( database serialization )
link_clang( database )

install( TARGETS database DESTINATION bin)