
cmake_minimum_required(VERSION 2.8)

#command to build boost...
#b2 --prefix=workspace\eg\third_party\install\boost --build-type=complete --build-dir=workspace\eg\third_party\build\boost threading=multi runtime-link=shared variant=release install


######################################
#Sort out the boost dependencies
find_path( BOOST_INCLUDEDIR NAMES boost PATHS ${EG_THIRD_PARTY_DIR}/install/boost/include )
find_path( BOOST_LIBRARYDIR NAMES boost_system-mt-gd.lib PATHS ${EG_THIRD_PARTY_DIR}/install/boost/lib )

set(BOOST_REQUIRED_VERSION 1.7.0 )
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost ${BOOST_REQUIRED_VERSION} QUIET COMPONENTS
    program_options
    date_time
    chrono
    random
    graph
    filesystem
    thread
    system
    python
    REQUIRED )

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
