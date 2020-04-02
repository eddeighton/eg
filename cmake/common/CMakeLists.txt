
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

set( COMMON_SOURCE_DIR ${EG_ROOT_DIR}/src/common )

set( COMMON_HEADERS
        ${COMMON_SOURCE_DIR}/assert_verify.hpp
        ${COMMON_SOURCE_DIR}/backtrace.hpp
        ${COMMON_SOURCE_DIR}/clang_warnings.hpp
        ${COMMON_SOURCE_DIR}/escape.hpp
        ${COMMON_SOURCE_DIR}/file.hpp
        ${COMMON_SOURCE_DIR}/require_semicolon.hpp
        ${COMMON_SOURCE_DIR}/tick.hpp )
        
set( COMMON_SOURCE
        ${COMMON_SOURCE_DIR}/backtrace.cpp
        ${COMMON_SOURCE_DIR}/escape.cpp
        ${COMMON_SOURCE_DIR}/file.cpp )

add_library( commonlib 
                    ${COMMON_HEADERS} ${COMMON_SOURCE}
				)

set_target_properties( commonlib PROPERTIES DEBUG_POSTFIX d )

link_boost( commonlib filesystem )

install( FILES  ${COMMON_HEADERS} DESTINATION include/common )
install( TARGETS commonlib DESTINATION lib )
install( FILES  ${COMMON_SOURCE_DIR}/.eg DESTINATION include/common )