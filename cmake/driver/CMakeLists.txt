
cmake_minimum_required(VERSION 3.14.2)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get clang
include( ${EG_ROOT_DIR}/cmake/clang_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

set( DRIVER_SOURCE_DIR ${EG_ROOT_DIR}/src/driver )
file( GLOB DRIVER_SOURCE_HEADERS ${DRIVER_SOURCE_DIR}/*.hpp )
file( GLOB DRIVER_SOURCE_SOURCES ${DRIVER_SOURCE_DIR}/*.cpp )
set( DRIVER_SOURCES ${DRIVER_SOURCE_HEADERS} ${DRIVER_SOURCE_SOURCES} )
source_group( src FILES ${DRIVER_SOURCES} )

add_executable( eg
                    ${DATABASE_API}
                    ${EG_SOURCES}
                    ${DRIVER_SOURCES}
				)

set_target_properties( eg PROPERTIES DEBUG_POSTFIX d )

link_boost( eg filesystem )
link_boost( eg iostreams )
link_boost( eg serialization )
link_clang( eg )
link_eg( eg )

install( TARGETS eg DESTINATION bin)