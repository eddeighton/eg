
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common/common_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

set( CLANG_PLUGIN_API ${EG_API_DIR}/clang_plugin/clang_plugin.hpp )

set( CLANG_PLUGIN_SOURCES
    ${EG_SRC_DIR}/clang_plugin/clang_utils.hpp
    ${EG_SRC_DIR}/clang_plugin/clang_utils.cpp
    ${EG_SRC_DIR}/clang_plugin/clang_plugin.cpp
    )

add_definitions(-DEG_DATABASE_SHARED_MODULE)

add_library( eg_clang_plugin SHARED
                    ${CLANG_PLUGIN_API}
                    ${CLANG_PLUGIN_SOURCES}
				)

set_target_properties( eg_clang_plugin PROPERTIES FOLDER eg )

set_target_properties( eg_clang_plugin PROPERTIES DEBUG_POSTFIX d )

link_boost( eg_clang_plugin filesystem )
link_boost( eg_clang_plugin iostreams )
link_boost( eg_clang_plugin serialization )
link_clang( eg_clang_plugin )
link_common( eg_clang_plugin )
link_eg( eg_clang_plugin )

install( FILES ${CLANG_PLUGIN_API} DESTINATION include/eg_clang_plugin )
install( TARGETS eg_clang_plugin DESTINATION bin)