

cmake_minimum_required(VERSION 3.14.2)

find_path( XSDE_BUILD_DEBUG_DIR NAMES xsde/xsde.lib PATHS ${EG_THIRD_PARTY_DIR}/xsde/install/debug )
find_path( XSDE_BUILD_RELEASE_DIR NAMES xsde/xsde.lib PATHS ${EG_THIRD_PARTY_DIR}/xsde/install/release )

find_file( XSDE_DEBUG_LIB NAME xsde.lib PATHS ${XSDE_BUILD_DEBUG_DIR}/xsde )
find_file( XSDE_RELEASE_LIB NAME xsde.lib PATHS ${XSDE_BUILD_RELEASE_DIR}/xsde )

find_file( XSDE_EXECUTABLE NAME xsde.exe PATHS ${EG_THIRD_PARTY_DIR}/xsde/src/xsde-3.2.0-i686-windows/bin )

INCLUDE_DIRECTORIES( optimized ${XSDE_BUILD_RELEASE_DIR} debug ${XSDE_BUILD_DEBUG_DIR} )
LINK_DIRECTORIES( optimized ${XSDE_BUILD_RELEASE_DIR} debug ${XSDE_BUILD_DEBUG_DIR} )

function( link_xsde targetname )
    target_link_libraries( ${targetname} PRIVATE optimized ${XSDE_RELEASE_LIB} debug ${XSDE_DEBUG_LIB} )
endfunction( link_xsde )

macro( compile_schema target_name xml_schema nmspace output_directory )

    #message( "Compile schema called with " ${xml_schema} " " ${nmspace} " " ${output_directory} )

    add_custom_command( COMMAND ${XSDE_EXECUTABLE} 
        ARGS "cxx-hybrid" --generate-parser --generate-serializer --generate-aggregate --no-long-long --namespace-map =${nmspace} --output-dir ${output_directory} ${xml_schema}
        MAIN_DEPENDENCY ${xml_schema}
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
        
        BYPRODUCTS 
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
        
        COMMENT "Generating xml schema parser using xsde for ${xml_schema} to ${output_directory}"
        
    )
    
    ##problem with this is it will run every time
    #add_custom_target( 
    #    ${target_name}
    #    ${XSDE_EXECUTABLE} "cxx-hybrid" --generate-parser --generate-serializer --generate-aggregate --no-long-long --namespace-map =${nmspace} --output-dir ${output_directory} ${xml_schema}
    #    
    #    BYPRODUCTS 
    #    ${output_directory}/schema.hxx
    #    ${output_directory}/schema.cxx
    #    ${output_directory}/schema-pskel.hxx
    #    ${output_directory}/schema-pskel.cxx
    #    ${output_directory}/schema-pimpl.hxx
    #    ${output_directory}/schema-pimpl.cxx
    #    ${output_directory}/schema-sskel.hxx
    #    ${output_directory}/schema-sskel.cxx
    #    ${output_directory}/schema-simpl.hxx
    #    ${output_directory}/schema-simpl.cxx
    #    
    #    DEPENDS ${xml_schema}
    #    
    #    COMMENT "Generating xml schema parser using xsde for ${xml_schema} to ${output_directory}" )
    
    #enable this to cause the schema to recompile - or find a way for cmake to detect the schema file timestamp properly
    #add_custom_target(
    #    ${target_name}
    #    SOURCES ${xml_schema} 
    #    
    #    DEPENDS ${xml_schema} 
    #)
    
    add_custom_target(
        ${target_name}
        DEPENDS ${xml_schema} 
    )

endmacro( compile_schema )