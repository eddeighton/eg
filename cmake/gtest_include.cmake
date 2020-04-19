
cmake_minimum_required( VERSION 3.1...3.16 )

include( ${EG_THIRD_PARTY_DIR}/gtest/install/lib/cmake/GTest/GTestConfig.cmake )

find_path( GTEST_INCLUDE_DIR NAMES gtest PATHS ${EG_THIRD_PARTY_DIR}/gtest/install/include )
find_path( GTEST_LIBRARY_PATH NAMES "gtest.lib" PATHS ${EG_THIRD_PARTY_DIR}/gtest/install/lib )
find_path( GTEST_MAIN_LIBRARY_PATH NAMES "gtest_main.lib" PATHS ${EG_THIRD_PARTY_DIR}/gtest/install/lib )

set( GTEST_LIBRARY ${GTEST_LIBRARY_PATH}/gtest.lib )
set( GTEST_LIBRARY_DEBUG ${GTEST_LIBRARY_PATH}/gtestd.lib )
set( GTEST_MAIN_LIBRARY ${GTEST_MAIN_LIBRARY_PATH}/gtest_main.lib )
set( GTEST_MAIN_LIBRARY_DEBUG ${GTEST_MAIN_LIBRARY_PATH}/gtest_maind.lib )

find_package( GTest REQUIRED )

function( link_gtest targetname )
	target_include_directories( ${targetname} PRIVATE ${GTEST_INCLUDE_DIR} )
	target_link_libraries( ${targetname} PUBLIC debug ${GTEST_LIBRARY_DEBUG} optimized ${GTEST_LIBRARY} )
endfunction( link_gtest )

function( link_gtest_main targetname )
	target_include_directories( ${targetname} PRIVATE ${GTEST_INCLUDE_DIR} )
	target_link_libraries( ${targetname} PUBLIC debug ${GTEST_MAIN_LIBRARY_DEBUG} optimized ${GTEST_MAIN_LIBRARY} )
endfunction( link_gtest_main )
