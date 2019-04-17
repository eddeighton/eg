

cmake_minimum_required(VERSION 2.8)


find_path( LLVM_DIR LLVMConfig.cmake 
    ${EG_THIRD_PARTY_DIR}/build/llvm/lib/cmake/llvm/ )
find_path( CLANG_DIR CLANGConfig.cmake 
    ${EG_THIRD_PARTY_DIR}/build/llvm/lib/cmake/clang/ )
    
find_package( LLVM REQUIRED CONFIG )
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
link_directories( ${LLVM_LIB_DIR} )

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

llvm_map_components_to_libnames(llvm_libs support core bitreader)
    
find_package( CLANG REQUIRED CONFIG )
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${CLANG_DEFINITIONS})
link_directories( ${CLANG_LIB_DIR} )

set(LLVM_LINK_COMPONENTS support)

function( link_clang targetname )
    target_link_libraries( ${targetname} ${llvm_libs} )
    target_link_libraries( ${targetname} clangTooling )
endfunction( link_clang )


