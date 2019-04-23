

cmake_minimum_required(VERSION 3.14.2)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get pybind11
include( ${EG_ROOT_DIR}/cmake/pybind11_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )


include_directories( ${EG_ROOT_DIR}/submodules )


set( PYEG_SOURCE_DIR ${EG_ROOT_DIR}/src/pyeg )

set( PYEG_SOURCES
    ${PYEG_SOURCE_DIR}/main.cpp
    ${PYEG_SOURCE_DIR}/py_eg_reference.cpp
    ${PYEG_SOURCE_DIR}/py_eg_reference.hpp
    ${PYEG_SOURCE_DIR}/runtime.hpp
    ${PYEG_SOURCE_DIR}/runtime.cpp)
    
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
