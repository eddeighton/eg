
#ifndef CODE_GEN_18_04_2019
#define CODE_GEN_18_04_2019


#include "abstract.hpp"
#include "implementation_session.hpp"

#include <string>
#include <cstddef>
#include <vector>
#include <ostream>

namespace eg
{
    static const char* EG_BASE_PREFIX = "Base_";
    static const char* EG_INTERFACE_PREFIX = "__eg_";
    static const char* EG_INTERFACE_PARAMETER_TYPE = "__eg";
    
    static const char* EG_INSTANCE = "eg::Instance";
    static const char* EG_TYPE_ID = "eg::TypeID";
    static const char* EG_TIME_STAMP = "eg::TimeStamp";
    static const char* EG_INVALID_TIMESTAMP = "eg::INVALID_TIMESTAMP";
    static const char* EG_OBJECT_TRAITS = "eg::ObjectTraits";
    static const char* EG_DIMENSION_TRAITS = "eg::DimensionTraits";
    static const char* EG_TYPE_PATH = "__eg_type_path";
    static const char* EG_VARIANT_TYPE = "__eg_variant";
    static const char* EG_RESULT_TYPE = "eg::result_type";
    static const char* EG_COROUTINE_TYPE = "eg::Coroutine";
    static const char* EG_RANGE_TYPE = "eg::Range";
    static const char* EG_TYPE_PATH_CANNON_TYPE = "eg::CanonicaliseTypePath";
    static const char* EG_ITERATOR_TYPE = "eg::Iterator";
    static const char* EG_REFERENCE_TYPE = "eg::reference";
    static const char* EG_REFERENCE_ITERATOR_TYPE = "eg::ReferenceIterator";
    static const char* EG_IS_CONVERTIBLE_TYPE = "eg::is_convertible";
    static const char* EG_EVENT_TYPE = "eg::Event";
    static const char* EG_EVENT_LOG_EVENT_TYPE = "eg::_event";
    static const char* EG_EVENT_ITERATOR = "eg::event_iterator";
    static const char* EG_DEPENDENCY_PROVIDER_TYPE = "eg::DependencyProvider";
    
    class Identifiers;
    
    std::string getInterfaceType( const std::string& strType );
    std::string getBaseTraitType( std::size_t szIndex );
    std::string getInterfaceInstantiationType( const std::string& strType, int iDepth );
    
    void generateIncludeHeader( std::ostream& os, const abstract::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser );
    
    void generateInterface( std::ostream& os, const abstract::Root* pRoot, const Identifiers* pIdentifiers );

    void generateOperationSource( std::ostream& os, const abstract::Root* pRoot );
    
    void generateBufferStructures( std::ostream& os, const ImplementationSession& program );

    void generateImplementationSource( std::ostream& os, const ImplementationSession& program, 
        std::size_t szTranslationUnitID, const std::vector< std::string >& dependencies );
}

#endif //CODE_GEN_18_04_2019