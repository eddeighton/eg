
cmake_minimum_required(VERSION 3.14.2)

#command to build boost...
#b2 --prefix=workspace\eg\third_party\install\boost --build-type=complete --build-dir=workspace\eg\third_party\build\boost threading=multi runtime-link=shared variant=release install

include( ${EG_THIRD_PARTY_DIR}/install/boost/lib/cmake/Boost-1.70.0/BoostConfig.cmake )

######################################
#Sort out the boost dependencies
find_path( BOOST_INCLUDEDIR NAMES boost PATHS ${EG_THIRD_PARTY_DIR}/install/boost/include/boost-1_70 )
find_path( BOOST_LIBRARYDIR NAMES "cmake/Boost-1.70.0/BoostConfig.cmake" PATHS ${EG_THIRD_PARTY_DIR}/install/boost/lib )

#set( Boost_DIR ${EG_THIRD_PARTY_DIR}/install/boost/ )

set(BOOST_REQUIRED_VERSION 1.70.0 )
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.70.0 REQUIRED QUIET COMPONENTS 
    program_options
    date_time
    fiber
    random
    filesystem
    system
    iostreams
    serialization )

INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIRS} )
LINK_DIRECTORIES( ${Boost_LIBRARY_DIRS} )

#custom boost assert handle so can dump callstack
add_definitions(-DBOOST_ENABLE_ASSERT_HANDLER -D_BOOST_ENABLE_ASSERT_HANDLER)

add_definitions(-DBOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE -D_BOOST_CONFIG_SUPPRESS_OUTDATED_MESSAGE)

function( link_boost targetname lib )
    target_link_libraries( ${targetname} ${Boost_${lib}_LIBRARY} )
endfunction( link_boost )

function( link_boost_usual targetname )
    target_link_libraries( ${targetname} ${Boost_PROGRAM_OPTIONS_LIBRARY} )
    target_link_libraries( ${targetname} ${Boost_FILESYSTEM_LIBRARY} )
    target_link_libraries( ${targetname} ${Boost_SYSTEM_LIBRARY} )
endfunction( link_boost_usual )
