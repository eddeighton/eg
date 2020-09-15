
cmake_minimum_required( VERSION 3.1...3.16 )

find_path( LLVM_DIR LLVMConfig.cmake 
    ${EG_THIRD_PARTY_DIR}/llvm/build/lib/cmake/llvm/ )
find_path( CLANG_DIR CLANGConfig.cmake 
    ${EG_THIRD_PARTY_DIR}/llvm/build/lib/cmake/clang/ )
    
find_package( LLVM REQUIRED CONFIG )

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

llvm_map_components_to_libnames(llvm_libs support core bitreader)
    
find_package( CLANG REQUIRED CONFIG )

set(LLVM_LINK_COMPONENTS support)

function( link_clang targetname )

	target_compile_definitions( ${targetname} PRIVATE ${LLVM_DEFINITIONS} )
	target_compile_definitions( ${targetname} PRIVATE ${CLANG_DEFINITIONS} )
	
	target_include_directories( ${targetname} PRIVATE ${LLVM_INCLUDE_DIRS} )
	target_include_directories( ${targetname} PRIVATE ${CLANG_INCLUDE_DIRS} )
	
	target_link_directories( ${targetname} PRIVATE ${LLVM_LIB_DIR} )
	target_link_directories( ${targetname} PRIVATE ${CLANG_LIB_DIR} )

    target_link_libraries( ${targetname} PRIVATE ${llvm_libs} )
	target_link_libraries( ${targetname} PRIVATE clangTooling clangEG )
	
endfunction( link_clang )


