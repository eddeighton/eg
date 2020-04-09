
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

include_directories( ${EG_ROOT_DIR}/src/generators )

#get the sources
set( PYTHON_HOST_SOURCE_DIR ${EG_ROOT_DIR}/src/generators/python )

set( PYTHON_HOST_SOURCES 
                ${EG_ROOT_DIR}/src/generators/generators.hpp
                ${EG_ROOT_DIR}/src/generators/generators.cpp
                ${PYTHON_HOST_SOURCE_DIR}/main.cpp
                ${PYTHON_HOST_SOURCE_DIR}/python_codegen.cpp )

source_group( src FILES ${PYTHON_HOST_SOURCES} )

add_executable( python_host 
					${COMMON_INCLUDE_HEADERS}
					${PYTHON_HOST_SOURCES}
				)
		
set_target_properties( python_host PROPERTIES FOLDER generators )
		
set_target_properties( python_host PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( python_host )
link_common( python_host )
link_eg( python_host )

install( TARGETS python_host DESTINATION bin)
