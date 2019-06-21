
#ifndef WIZARD_EXAMPLES
#define WIZARD_EXAMPLES

#include "schema/wizardxml/wizard_schema.hxx"
#include "schema/wizardxml/wizard_schema-pimpl.hxx"
#include "schema/wizardxml/wizard_schema-simpl.hxx"

#include "schema/egxml/eg_schema.hxx"
#include "schema/egxml/eg_schema-pimpl.hxx"
#include "schema/egxml/eg_schema-simpl.hxx"

#include "schema/project.hpp"

#include <memory>

struct Example
{
    Environment environment;
    Project project;
    std::shared_ptr< egxml::EG > pXML;
    
    Example( const boost::filesystem::path& projectDir, std::shared_ptr< egxml::EG > pXML )
        :   pXML( pXML ),
            project( projectDir, environment, pXML->Project(), "default" )
    {
    
    }
};

#endif
