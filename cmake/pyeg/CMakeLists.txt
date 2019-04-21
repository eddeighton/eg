

cmake_minimum_required(VERSION 3.14.2)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get pybind11
include( ${EG_ROOT_DIR}/cmake/pybind11_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

set( PYEG_SOURCE_DIR ${EG_ROOT_DIR}/src/pyeg )
file( GLOB PYEG_SOURCE_HEADERS ${PYEG_SOURCE_DIR}/*.hpp )
file( GLOB PYEG_SOURCE_SOURCES ${PYEG_SOURCE_DIR}/*.cpp )
set( PYEG_SOURCES ${PYEG_SOURCE_HEADERS} ${PYEG_SOURCE_SOURCES} )
source_group( src FILES ${PYEG_SOURCES} )

add_executable( pyeg
                    ${DATABASE_API}
                    ${EG_SOURCES}
                    ${PYEG_SOURCES}
				)

set_target_properties( pyeg PROPERTIES DEBUG_POSTFIX d )

link_boost( pyeg filesystem )
link_boost( pyeg iostreams )
link_boost( pyeg serialization )
link_pybind11( pyeg )
link_eg( pyeg )

install( TARGETS pyeg DESTINATION bin)
