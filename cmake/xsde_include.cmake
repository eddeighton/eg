

cmake_minimum_required(VERSION 3.14.2)

find_path( XSDE_BUILD_DEBUG_DIR NAMES xsde.lib PATHS ${EG_THIRD_PARTY_DIR}/build/xsde/debug )
find_path( XSDE_BUILD_RELEASE_DIR NAMES xsde.lib PATHS ${EG_THIRD_PARTY_DIR}/build/xsde/release )

find_file( XSDE_DEBUG_LIB NAME xsde.lib PATHS ${XSDE_BUILD_DEBUG_DIR}/xsde )
find_file( XSDE_RELEASE_LIB NAME xsde.lib PATHS ${XSDE_BUILD_RELEASE_DIR}/xsde )

find_file( XSDE_EXECUTABLE NAME xsde.exe PATHS ${EG_THIRD_PARTY_DIR}/install/xsde )

INCLUDE_DIRECTORIES( optimized ${XSDE_BUILD_RELEASE_DIR} debug ${XSDE_BUILD_DEBUG_DIR} )
LINK_DIRECTORIES( optimized ${XSDE_BUILD_RELEASE_DIR} debug ${XSDE_BUILD_DEBUG_DIR} )

function( link_xsde targetname )
    target_link_libraries( ${targetname} optimized ${XSDE_RELEASE_LIB} debug ${XSDE_DEBUG_LIB} )
endfunction( link_xsde )

function( compile_schema schema nmspace output_directory )

    add_custom_command( COMMAND ${XSDE_EXECUTABLE} 
        ARGS "cxx-hybrid" --generate-parser --generate-serializer --generate-aggregate --no-long-long --namespace-map =${nmspace} --output-dir ${output_directory} ${schema}
        MAIN_DEPENDENCY ${schema}
        OUTPUT 
        ${output_directory}/schema.hxx
        ${output_directory}/schema.cxx
        ${output_directory}/schema-pskel.hxx
        ${output_directory}/schema-pskel.cxx
        ${output_directory}/schema-pimpl.hxx
        ${output_directory}/schema-pimpl.cxx
        ${output_directory}/schema-sskel.hxx
        ${output_directory}/schema-sskel.cxx
        ${output_directory}/schema-simpl.hxx
        ${output_directory}/schema-simpl.cxx
        
        COMMENT "Generating xml schema parser using xsde for ${schema} to ${output_directory}"
    )

endfunction( compile_schema )