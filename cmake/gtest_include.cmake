
cmake_minimum_required(VERSION 3.14.2)

include( ${EG_THIRD_PARTY_DIR}/install/gtest/lib/cmake/GTest/GTestConfig.cmake )

find_path( GTEST_INCLUDE_DIR NAMES gtest PATHS ${EG_THIRD_PARTY_DIR}/install/gtest/include )
find_path( GTEST_LIBRARY_PATH NAMES "gtest.lib" PATHS ${EG_THIRD_PARTY_DIR}/install/gtest/lib )
find_path( GTEST_MAIN_LIBRARY_PATH NAMES "gtest_main.lib" PATHS ${EG_THIRD_PARTY_DIR}/install/gtest/lib )

set( GTEST_LIBRARY ${GTEST_LIBRARY_PATH}/gtest.lib )
set( GTEST_LIBRARY_DEBUG ${GTEST_LIBRARY_PATH}/gtestd.lib )
set( GTEST_MAIN_LIBRARY ${GTEST_MAIN_LIBRARY_PATH}/gtest_main.lib )
set( GTEST_MAIN_LIBRARY_DEBUG ${GTEST_MAIN_LIBRARY_PATH}/gtest_maind.lib )

#enable_testing()
find_package( GTest REQUIRED )

include_directories( ${GTEST_INCLUDE_DIR} )

function( link_gtest targetname )
	target_link_libraries( ${targetname} debug ${GTEST_LIBRARY_DEBUG} optimized ${GTEST_LIBRARY} )
endfunction( link_gtest )

function( link_gtest_main targetname )
	target_link_libraries( ${targetname} debug ${GTEST_MAIN_LIBRARY_DEBUG} optimized ${GTEST_MAIN_LIBRARY} )
endfunction( link_gtest_main )
