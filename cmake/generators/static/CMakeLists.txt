
cmake_minimum_required( VERSION 3.1...3.16 )

#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common_include.cmake )

#get eg
include( ${EG_ROOT_DIR}/cmake/eg/eg_include.cmake )

include_directories( ${EG_ROOT_DIR}/src/generators )

#get the sources
set( STATIC_GENERATOR_SOURCE_DIR ${EG_ROOT_DIR}/src/generators/static )

set( STATIC_GENERATOR_SOURCES  
                ${EG_ROOT_DIR}/src/generators/generators.hpp
                ${EG_ROOT_DIR}/src/generators/generators.cpp
                ${STATIC_GENERATOR_SOURCE_DIR}/main.cpp )

source_group( src FILES ${STATIC_GENERATOR_SOURCES} )

add_executable( static_generator 
					${COMMON_INCLUDE_HEADERS}
					${STATIC_GENERATOR_SOURCES}
				)
		
set_target_properties( static_generator PROPERTIES FOLDER generators )
		
set_target_properties( static_generator PROPERTIES DEBUG_POSTFIX d )

link_boost_usual( static_generator )
link_common( static_generator )
link_eg( static_generator )

install( TARGETS static_generator DESTINATION bin)
