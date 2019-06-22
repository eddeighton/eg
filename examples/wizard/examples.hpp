
#ifndef WIZARD_EXAMPLES
#define WIZARD_EXAMPLES

#include "schema/wizardxml/wizard_schema.hxx"
#include "schema/wizardxml/wizard_schema-pimpl.hxx"
#include "schema/wizardxml/wizard_schema-simpl.hxx"

#include "schema/egxml/eg_schema.hxx"
#include "schema/egxml/eg_schema-pimpl.hxx"
#include "schema/egxml/eg_schema-simpl.hxx"

#include "schema/project.hpp"

#include "eg/default_traits.hpp"

#include <memory>

struct Example
{
    std::shared_ptr< egxml::EG > pXML;
    boost::filesystem::path dir;
    Environment environment;
    Project project;
    
    Example( std::shared_ptr< egxml::EG > pXML, const boost::filesystem::path& projectDir )
        :   pXML( pXML ),
            dir( projectDir ),
            environment( projectDir ),
            project( projectDir, environment, pXML->Project(), "release" )
    {
    
    }
    
    
};

namespace eg
{
    template<>
    struct DimensionTraits< Environment >
    {
        using Read  = Environment&; //change Read access to return a reference
        using Write = Environment;
        using Get   = Environment&;
        static const Instance Size = sizeof( Environment );
        
        static void initialise( Environment& env )
        {
            new( &env ) Environment;
        }
    };
}

#endif
