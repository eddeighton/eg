
cmake_minimum_required(VERSION 3.14.2)

#get xsde
include( ${EG_ROOT_DIR}/cmake/xsde_include.cmake )

#get eg schema
include( ${EG_ROOT_DIR}/cmake/schema/schema_include.cmake )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#glob the rest of the source code
set( INNOSETUP_SOURCE_DIR ${EG_ROOT_DIR}/src/innosetup )

set( INNOSETUP_SOURCES ${INNOSETUP_SOURCE_DIR}/main.cpp )

source_group( src FILES ${INNOSETUP_SOURCES} )

add_executable( innosetup_cmd

                    ${EG_SRC_DIR}/common/assert_verify.hpp
                    ${EG_SRC_DIR}/common/backtrace.hpp
                    ${EG_SRC_DIR}/common/backtrace.cpp
                    ${EG_SRC_DIR}/common/escape.hpp
                    ${EG_SRC_DIR}/common/escape.cpp
                    ${EG_SRC_DIR}/common/file.hpp
                    ${EG_SRC_DIR}/common/file.cpp
                    ${EG_SRC_DIR}/common/require_semicolon.hpp
                    
                    ${INNOSETUP_SOURCES}
				)

set_target_properties( innosetup_cmd PROPERTIES DEBUG_POSTFIX d )

link_xsde( innosetup_cmd )
link_schemalib( innosetup_cmd )
link_boost( innosetup_cmd filesystem )


install( TARGETS innosetup_cmd DESTINATION bin)