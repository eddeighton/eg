
cmake_minimum_required(VERSION 3.14.2)

#get xsde
include( ${EG_ROOT_DIR}/cmake/xsde_include.cmake )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#handle the schema code gen
set( EG_SCHEMA_SOURCE_DIR ${EG_ROOT_DIR}/src/driver )
find_file( EG_SCHEMA_FILE NAME schema.xsd PATHS ${EG_SCHEMA_SOURCE_DIR} )
compile_schema( ${EG_SCHEMA_FILE} ${EG_SCHEMA_SOURCE_DIR} )
set( SCHEMA_GENERATED_CODE
    ${EG_SCHEMA_SOURCE_DIR}/schema.hxx
    ${EG_SCHEMA_SOURCE_DIR}/schema.cxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-pskel.hxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-pskel.cxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-pimpl.hxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-pimpl.cxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-sskel.hxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-sskel.cxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-simpl.hxx
    ${EG_SCHEMA_SOURCE_DIR}/schema-simpl.cxx )

#glob the rest of the source code
set( DRIVER_SOURCE_DIR ${EG_ROOT_DIR}/src/driver )
file( GLOB DRIVER_SOURCE_HEADERS ${DRIVER_SOURCE_DIR}/*.hpp )
file( GLOB DRIVER_SOURCE_SOURCES ${DRIVER_SOURCE_DIR}/*.cpp )
set( DRIVER_SOURCES ${DRIVER_SOURCE_HEADERS} ${DRIVER_SOURCE_SOURCES} )

source_group( src FILES ${DRIVER_SOURCES} )
source_group( xsde FILES ${SCHEMA_GENERATED_CODE} )

add_executable( eg
                    ${DATABASE_API}
                    ${EG_SOURCES}
                    ${SCHEMA_GENERATED_CODE}
                    ${DRIVER_SOURCES}
				)

set_target_properties( eg PROPERTIES DEBUG_POSTFIX d )

link_xsde( eg )
link_boost( eg filesystem )
link_boost( eg iostreams )
link_boost( eg serialization )
link_clang( eg )
link_eg( eg )

install( TARGETS eg DESTINATION bin)