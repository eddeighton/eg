
cmake_minimum_required( VERSION 3.1...3.16 )

#get xsde
include( ${EG_ROOT_DIR}/cmake/xsde_include.cmake )
    
#get boost
include( ${EG_ROOT_DIR}/cmake/boost_include.cmake )

#get common
include( ${EG_ROOT_DIR}/cmake/common_include.cmake )

#handle the schema code gen
set( SCHEMA_SOURCE_DIR ${EG_ROOT_DIR}/src/schema )

find_file( EG_SCHEMA_FILE NAME eg_schema.xsd PATHS ${SCHEMA_SOURCE_DIR} )
set( EG_SCHEMA_GENERATED_SOURCE_DIR ${SCHEMA_SOURCE_DIR}/egxml )
compile_schema( eg_schema_target ${EG_SCHEMA_FILE} "egxml" ${EG_SCHEMA_GENERATED_SOURCE_DIR} )

set( EG_SCHEMA_GENERATED_HEADERS
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pskel.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pimpl.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-sskel.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-simpl.hxx )
set( EG_SCHEMA_GENERATED_SOURCE
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pskel.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pimpl.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-sskel.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-simpl.cxx )

find_file( WIZARD_SCHEMA_FILE NAME wizard_schema.xsd PATHS ${SCHEMA_SOURCE_DIR} )
set( WIZARD_SCHEMA_GENERATED_SOURCE_DIR ${SCHEMA_SOURCE_DIR}/wizardxml )
compile_schema( wizard_schema_target ${WIZARD_SCHEMA_FILE} "wizardxml" ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR} )
set( WIZARD_SCHEMA_GENERATED_HEADERS
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pskel.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pimpl.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-sskel.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-simpl.hxx )
set( WIZARD_SCHEMA_GENERATED_SOURCE
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pskel.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pimpl.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-sskel.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-simpl.cxx )

find_file( INSTALLATION_SCHEMA_FILE NAME installation_schema.xsd PATHS ${SCHEMA_SOURCE_DIR} )
set( INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR ${SCHEMA_SOURCE_DIR}/installationxml )
compile_schema( installation_schema_target ${INSTALLATION_SCHEMA_FILE} "installationxml" ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR} )
set( INSTALLATION_SCHEMA_GENERATED_HEADERS
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pskel.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pimpl.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-sskel.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-simpl.hxx )
set( INSTALLATION_SCHEMA_GENERATED_SOURCE
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pskel.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pimpl.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-sskel.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-simpl.cxx )
        
set( OTHER_HEADERS
        ${SCHEMA_SOURCE_DIR}/environment_string.hpp
        ${SCHEMA_SOURCE_DIR}/project.hpp)
set( OTHER_SOURCES
        ${SCHEMA_SOURCE_DIR}/environment_string.cpp
        ${SCHEMA_SOURCE_DIR}/project.cpp )
        
add_library( schemalib 
                    ${EG_SCHEMA_GENERATED_HEADERS} ${EG_SCHEMA_GENERATED_SOURCE}
                    ${WIZARD_SCHEMA_GENERATED_HEADERS} ${WIZARD_SCHEMA_GENERATED_SOURCE}
                    ${INSTALLATION_SCHEMA_GENERATED_HEADERS} ${INSTALLATION_SCHEMA_GENERATED_SOURCE}
                    ${OTHER_HEADERS} ${OTHER_SOURCES}
				)

add_dependencies( schemalib eg_schema_target )
add_dependencies( schemalib wizard_schema_target )
add_dependencies( schemalib installation_schema_target )

set_target_properties( schemalib PROPERTIES FOLDER schema )
set_target_properties( eg_schema_target PROPERTIES FOLDER schema )
set_target_properties( wizard_schema_target PROPERTIES FOLDER schema )
set_target_properties( installation_schema_target PROPERTIES FOLDER schema )

set_target_properties( schemalib PROPERTIES DEBUG_POSTFIX d )

link_xsde( schemalib )
link_boost( schemalib filesystem )
link_common( schemalib )

install( FILES  ${OTHER_HEADERS} DESTINATION include/schema )
install( FILES  ${EG_SCHEMA_GENERATED_HEADERS} DESTINATION include/schema/egxml )
install( FILES  ${WIZARD_SCHEMA_GENERATED_HEADERS} DESTINATION include/schema/wizardxml )
install( FILES  ${INSTALLATION_SCHEMA_GENERATED_HEADERS} DESTINATION include/schema/installationxml )
install( TARGETS schemalib DESTINATION lib )
install( FILES  ${EG_ROOT_DIR}/src/schema/.eg DESTINATION include/schema )
                
                
