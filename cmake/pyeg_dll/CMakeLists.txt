

cmake_minimum_required(VERSION 3.14.2)

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get pybind11
#include( ${EG_ROOT_DIR}/cmake/pybind11_include.cmake )

#get eg
#include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#get python
find_path( Python3_ROOT_DIR NAMES python.exe PATHS C:/Anaconda3/ )

find_package( Python3 COMPONENTS Interpreter Development NumPy )
message("Include dirs of Python: " ${Python3_INCLUDE_DIR} )
message("Libs of Python: " ${Python3_LIBRARY_RELEASE} )

#find_path( NUMPY_INCLUDE NAMES numpy/arrayobject.h PATHS C:/Anaconda3/Lib/site-packages/numpy/core/include )

include_directories( ${Python3_INCLUDE_DIR} )
#include_directories( ${NUMPY_INCLUDE} )

set( PYEG_SOURCE_DIR ${EG_ROOT_DIR}/src/pyeg )

set( PYEG_SOURCES
    ${PYEG_SOURCE_DIR}/python_include.hpp
    ${PYEG_SOURCE_DIR}/pyeg.cpp )

add_library( pyeg_dll SHARED MODULE
                    ${PYEG_SOURCES}
				)

set_target_properties( pyeg_dll PROPERTIES DEBUG_POSTFIX d SUFFIX ".pyd" OUTPUT_NAME "pyeg" )
set_target_properties( pyeg_dll PROPERTIES FOLDER pyeg )

target_link_libraries( pyeg_dll ${Python3_LIBRARY_RELEASE} )

#link_boost( pyeg_dll filesystem )
#link_boost( pyeg_dll iostreams )
#link_boost( pyeg_dll serialization )
#link_pybind11( pyeg_dll )
#link_eg( pyeg_dll )

install( TARGETS pyeg_dll DESTINATION bin)
