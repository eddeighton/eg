
cmake_minimum_required( VERSION 3.1...3.16 )

find_path( SPDLOG_INSTALL_PATH spdlog PATHS ${EG_THIRD_PARTY_DIR}/spdlog/install/lib/cmake/ )

set( CMAKE_PREFIX_PATH "${SPDLOG_INSTALL_PATH}/spdlog;${CMAKE_PREFIX_PATH}" )
find_package( spdlog )

function( link_spdlog targetname )
    target_compile_definitions( ${targetname} PUBLIC SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE )
    target_link_libraries( ${targetname} PUBLIC spdlog::spdlog )
endfunction( link_spdlog )

