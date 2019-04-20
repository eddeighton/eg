
cmake_minimum_required(VERSION 3.14.2)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( EG_ROOT_DIR NAMES api PATHS ${CMAKE_PARENT_PATH} )

######################################
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)
set(CMAKE_INSTALL_PREFIX ${EG_ROOT_DIR} CACHE TYPE INTERNAL FORCE)

set( EG_API_DIR ${EG_ROOT_DIR}/api )
set( EG_SRC_DIR ${EG_ROOT_DIR}/src )
set( EG_THIRD_PARTY_DIR ${EG_ROOT_DIR}/third_party )

add_compile_options(/std:c++17)

include_directories( ${EG_API_DIR} )
include_directories( ${EG_SRC_DIR} )
link_directories( ${PROJECT_DIR}/bin )

######################################
#win32 specific settings
IF( ${WIN32} )

# /DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP
#set( CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP")

add_compile_options(/DWIN32)
add_compile_options(/D_WINDOWS)
add_compile_options(/MP)
add_compile_options(/W3)

add_definitions(-D_WIN32_WINNT=0x0601)
add_definitions(-DNOMINMAX -D_NOMINMAX)
#add_definitions(-DUNICODE -D_UNICODE)
##specify the character set to use for all projects
add_definitions(-DSBCS -D_SBCS)

##ensure c++ dependencies are installed
include (InstallRequiredSystemLibraries)

ENDIF( ${WIN32} )


project( eg )

add_subdirectory(eventlog)
add_subdirectory(eventlog_cmd)
add_subdirectory(eg)
add_subdirectory(clang_plugin)
add_subdirectory(driver)
add_subdirectory(hosts)
add_subdirectory(tests)