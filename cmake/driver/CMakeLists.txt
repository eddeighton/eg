
cmake_minimum_required( VERSION 3.1...3.16 )

#get xsde
include( ${EG_ROOT_DIR}/cmake/xsde_include.cmake )

#get eg schema
include( ${EG_ROOT_DIR}/cmake/schema/schema_include.cmake )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get indicators
include( ${EG_ROOT_DIR}/cmake/indicators_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common/common_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#glob the rest of the source code
set( DRIVER_SOURCE_DIR ${EG_ROOT_DIR}/src/driver )
file( GLOB DRIVER_SOURCE_HEADERS ${DRIVER_SOURCE_DIR}/*.hpp )
file( GLOB DRIVER_SOURCE_SOURCES ${DRIVER_SOURCE_DIR}/*.cpp )
set( DRIVER_SOURCES ${DRIVER_SOURCE_HEADERS} ${DRIVER_SOURCE_SOURCES} )

source_group( src FILES ${DRIVER_SOURCES} )

add_executable( eg
                    ${DATABASE_API}
                    ${DRIVER_SOURCES}
				)

set_target_properties( eg PROPERTIES FOLDER eg )

set_target_properties( eg PROPERTIES DEBUG_POSTFIX d )

link_xsde( eg )
link_schemalib( eg )
link_boost( eg program_options )
link_boost( eg timer )
link_boost( eg filesystem )
link_boost( eg iostreams )
link_boost( eg serialization )
link_clang( eg )
link_indicators( eg )
link_common( eg )
link_eg( eg )

install( TARGETS eg DESTINATION bin)