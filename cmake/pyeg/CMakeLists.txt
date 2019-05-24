

cmake_minimum_required(VERSION 3.14.2)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get pybind11
include( ${EG_ROOT_DIR}/cmake/pybind11_include.cmake )

#get eg
#include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#get eg runtime
#include( ${EG_ROOT_DIR}/cmake/eg_runtime/eg_runtime_include.cmake )

include_directories( ${EG_ROOT_DIR}/submodules )

set( PYEG_SOURCE_DIR ${EG_ROOT_DIR}/src/pyeg )

set( PYEG_SOURCES
    ${EG_SRC_DIR}/common/assert_verify.hpp
    ${EG_SRC_DIR}/common/backtrace.hpp
    ${EG_SRC_DIR}/common/backtrace.cpp
    ${EG_SRC_DIR}/common/require_semicolon.hpp
    ${EG_SRC_DIR}/common/file.hpp
    ${EG_SRC_DIR}/common/file.cpp
    
    ${PYEG_SOURCE_DIR}/main.cpp)
    
source_group( src FILES ${PYEG_SOURCES} )
    
set( CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /MP") # /await

set( CMAKE_EXE_LINKER_FLAGS "/SAFESEH:NO" )
# 

add_executable( pyeg
                    ${DATABASE_API}
                    ${EG_SOURCES}
                    ${PYEG_SOURCES}
				)

set_target_properties( pyeg PROPERTIES DEBUG_POSTFIX d )

link_boost( pyeg fiber )
link_boost( pyeg filesystem )
#link_boost( pyeg iostreams )
#link_boost( pyeg serialization )
#link_eg( pyeg )
#link_eg_runtime( pyeg )

link_pybind11( pyeg )

install( TARGETS pyeg DESTINATION bin)
