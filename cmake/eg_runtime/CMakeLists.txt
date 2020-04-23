
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get pybind11
include( ${EG_ROOT_DIR}/cmake/pybind11_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

#get python
#find_path( Python3_ROOT_DIR NAMES python.exe PATHS C:/Anaconda3/ )

#find_package( Python3 COMPONENTS Interpreter Development NumPy )
#message("Include dirs of Python: " ${Python3_INCLUDE_DIR} )
#message("Libs of Python: " ${Python3_LIBRARY_RELEASE} )

#find_path( NUMPY_INCLUDE NAMES numpy/arrayobject.h PATHS C:/Anaconda3/Lib/site-packages/numpy/core/include )

#include_directories( ${Python3_INCLUDE_DIR} )
#include_directories( ${NUMPY_INCLUDE} )


set( EG_RUNTIME_API ${EG_API_DIR}/eg_runtime/eg_runtime.hpp )

set( EG_RUNTIME_SRC_DIR ${EG_ROOT_DIR}/src/eg_runtime )

set( EG_RUNTIME_SOURCES ${EG_RUNTIME_SRC_DIR}/eg_runtime.cpp )

add_definitions(-DEG_RUNTIME_SHARED_MODULE)

add_library( eg_runtime SHARED
                    ${EG_RUNTIME_API}
                    ${EG_RUNTIME_SOURCES}
				)

set_target_properties( eg_runtime PROPERTIES FOLDER eg )

#set_target_properties( eg_runtime PROPERTIES DEBUG_POSTFIX d SUFFIX ".pyd" OUTPUT_NAME "pyeg" )
set_target_properties( eg_runtime PROPERTIES DEBUG_POSTFIX d )

#target_link_libraries( eg_runtime ${Python3_LIBRARY_RELEASE} )

link_boost( eg_runtime filesystem )
link_boost( eg_runtime iostreams )
link_boost( eg_runtime serialization )
link_pybind11( eg_runtime )
link_common( eg_runtime )
link_eg( eg_runtime )

install( FILES ${EG_RUNTIME_API} DESTINATION include/eg_runtime )
install( TARGETS eg_runtime EXPORT eg DESTINATION bin)
