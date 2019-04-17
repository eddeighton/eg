
cmake_minimum_required(VERSION 2.8)



set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set( CMAKE_PARENT_PATH ${CMAKE_CURRENT_LIST_DIR}/.. )
find_path( EG_ROOT_DIR NAMES api PATHS ${CMAKE_PARENT_PATH} )

######################################
set(CMAKE_CONFIGURATION_TYPES Debug Release CACHE TYPE INTERNAL FORCE)
set(CMAKE_INSTALL_PREFIX ${EG_ROOT_DIR} CACHE TYPE INTERNAL FORCE)

set( EG_API_DIR ${EG_ROOT_DIR}/api )
set( EG_SRC_DIR ${EG_ROOT_DIR}/src )
set( EG_THIRD_PARTY_DIR ${EG_ROOT_DIR}/third_party )

######################################
#win32 specific settings
IF( ${WIN32} )

# /DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP
#set( CMAKE_CXX_FLAGS "/DWIN32 /D_WINDOWS /W3 /GR /EHsc /MP")

add_definitions(-DNOMINMAX -D_NOMINMAX)
#add_definitions(-DUNICODE -D_UNICODE)
##specify the character set to use for all projects
add_definitions(-DSBCS -D_SBCS)

##ensure c++ dependencies are installed
include (InstallRequiredSystemLibraries)

ENDIF( ${WIN32} )



project( eg )


add_subdirectory(database)