


cmake_minimum_required(VERSION 3.14.2)

#get xsde
include( ${EG_ROOT_DIR}/cmake/xsde_include.cmake )
    
#handle the schema code gen
set( SCHEMA_SOURCE_DIR ${EG_ROOT_DIR}/src/schema )

find_file( EG_SCHEMA_FILE NAME eg_schema.xsd PATHS ${SCHEMA_SOURCE_DIR} )
set( EG_SCHEMA_GENERATED_SOURCE_DIR ${SCHEMA_SOURCE_DIR}/egxml )
compile_schema( ${EG_SCHEMA_FILE} "egxml" ${EG_SCHEMA_GENERATED_SOURCE_DIR} )
set( EG_SCHEMA_GENERATED_SOURCE
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pskel.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pskel.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pimpl.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-pimpl.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-sskel.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-sskel.cxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-simpl.hxx
        ${EG_SCHEMA_GENERATED_SOURCE_DIR}/eg_schema-simpl.cxx )

find_file( WIZARD_SCHEMA_FILE NAME wizard_schema.xsd PATHS ${SCHEMA_SOURCE_DIR} )
set( WIZARD_SCHEMA_GENERATED_SOURCE_DIR ${SCHEMA_SOURCE_DIR}/wizardxml )
compile_schema( ${WIZARD_SCHEMA_FILE} "wizardxml" ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR} )
set( WIZARD_SCHEMA_GENERATED_SOURCE
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pskel.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pskel.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pimpl.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-pimpl.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-sskel.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-sskel.cxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-simpl.hxx
        ${WIZARD_SCHEMA_GENERATED_SOURCE_DIR}/wizard_schema-simpl.cxx )

find_file( INSTALLATION_SCHEMA_FILE NAME installation_schema.xsd PATHS ${SCHEMA_SOURCE_DIR} )
set( INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR ${SCHEMA_SOURCE_DIR}/installationxml )
compile_schema( ${INSTALLATION_SCHEMA_FILE} "installationxml" ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR} )
set( INSTALLATION_SCHEMA_GENERATED_SOURCE
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pskel.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pskel.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pimpl.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-pimpl.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-sskel.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-sskel.cxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-simpl.hxx
        ${INSTALLATION_SCHEMA_GENERATED_SOURCE_DIR}/installation_schema-simpl.cxx )
        
add_library( schemalib 
                    ${EG_SCHEMA_GENERATED_SOURCE}
                    ${WIZARD_SCHEMA_GENERATED_SOURCE}
                    ${INSTALLATION_SCHEMA_GENERATED_SOURCE}
				)

set_target_properties( schemalib PROPERTIES DEBUG_POSTFIX d )

link_xsde( schemalib )

install( TARGETS schemalib DESTINATION bin)
