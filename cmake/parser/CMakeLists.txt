
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

set( CLANG_PLUGIN_API ${EG_API_DIR}/parser/parser.hpp )

set( CLANG_PLUGIN_SOURCES
    ${EG_SRC_DIR}/parser/parser.cpp
    )

add_definitions(-DEG_DATABASE_SHARED_MODULE)

add_library( eg_parser SHARED
                    ${CLANG_PLUGIN_API}
                    ${CLANG_PLUGIN_SOURCES}
				)

set_target_properties( eg_parser PROPERTIES FOLDER eg )

set_target_properties( eg_parser PROPERTIES DEBUG_POSTFIX d )

link_boost( eg_parser filesystem )
link_boost( eg_parser iostreams )
link_boost( eg_parser serialization )
link_clang( eg_parser )
link_common( eg_parser )
link_eg( eg_parser )

install( FILES ${CLANG_PLUGIN_API} DESTINATION include/eg_parser )
install( TARGETS eg_parser DESTINATION bin)