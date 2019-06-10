

cmake_minimum_required(VERSION 3.14.2)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get xsde
include( ${EG_ROOT_DIR}/cmake/xsde_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

include_directories( ${EG_ROOT_DIR}/submodules )

set( SCHEMA_SOURCE_DIR ${EG_ROOT_DIR}/src/schema )

find_file( SCHEMA_FILE NAME schema.xsd PATHS ${SCHEMA_SOURCE_DIR} )

#execute the schema code gen
compile_schema( ${SCHEMA_FILE} "test" ${SCHEMA_SOURCE_DIR} )

set( SCHEMA_SOURCES
    ${EG_SRC_DIR}/common/assert_verify.hpp
    ${EG_SRC_DIR}/common/backtrace.hpp
    ${EG_SRC_DIR}/common/backtrace.cpp
    ${EG_SRC_DIR}/common/require_semicolon.hpp
    ${EG_SRC_DIR}/common/file.hpp
    ${EG_SRC_DIR}/common/file.cpp
    
    ${SCHEMA_SOURCE_DIR}/schema.hxx
    ${SCHEMA_SOURCE_DIR}/schema.cxx
    ${SCHEMA_SOURCE_DIR}/schema-pskel.hxx
    ${SCHEMA_SOURCE_DIR}/schema-pskel.cxx
    ${SCHEMA_SOURCE_DIR}/schema-pimpl.hxx
    ${SCHEMA_SOURCE_DIR}/schema-pimpl.cxx
    ${SCHEMA_SOURCE_DIR}/schema-sskel.hxx
    ${SCHEMA_SOURCE_DIR}/schema-sskel.cxx
    ${SCHEMA_SOURCE_DIR}/schema-simpl.hxx
    ${SCHEMA_SOURCE_DIR}/schema-simpl.cxx
    
    ${SCHEMA_SOURCE_DIR}/main.cpp)
    
source_group( src FILES ${SCHEMA_SOURCES} )
    
add_executable( schema_test
                    ${EG_SOURCES}
                    ${SCHEMA_SOURCES}
				)

set_target_properties( schema_test PROPERTIES DEBUG_POSTFIX d )

link_boost( schema_test filesystem )
link_xsde( schema_test )
link_eg( schema_test )

install( TARGETS schema_test DESTINATION bin)

