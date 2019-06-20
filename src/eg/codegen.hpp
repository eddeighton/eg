//  Copyright (c) Deighton Systems Limited. 2019. All Rights Reserved.
//  Author: Edward Deighton
//  License: Please see license.txt in the project root folder.

//  Use and copying of this software and preparation of derivative works
//  based upon this software are permitted. Any copy of this software or
//  of any derivative work must include the above copyright notice, this
//  paragraph and the one after it.  Any distribution of this software or
//  derivative works must comply with all applicable laws.

//  This software is made available AS IS, and COPYRIGHT OWNERS DISCLAIMS
//  ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE, AND NOTWITHSTANDING ANY OTHER PROVISION CONTAINED HEREIN, ANY
//  LIABILITY FOR DAMAGES RESULTING FROM THE SOFTWARE OR ITS USE IS
//  EXPRESSLY DISCLAIMED, WHETHER ARISING IN CONTRACT, TORT (INCLUDING
//  NEGLIGENCE) OR STRICT LIABILITY, EVEN IF COPYRIGHT OWNERS ARE ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGES.


#ifndef CODE_GEN_18_04_2019
#define CODE_GEN_18_04_2019


#include "interface.hpp"
#include "implementation_session.hpp"
#include "instruction.hpp"

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
    static const char* EG_ACTION_STATE = "eg::ActionState";
    static const char* EG_INVALID_TIMESTAMP = "eg::INVALID_TIMESTAMP";
    static const char* EG_INVALID_STATE = "eg::TOTAL_ACTION_STATES";
    static const char* EG_OBJECT_TRAITS = "eg::ObjectTraits";
    static const char* EG_DIMENSION_TRAITS = "eg::DimensionTraits";
    static const char* EG_TYPE_PATH = "__eg_type_path";
    static const char* EG_VARIANT_TYPE = "__eg_variant";
    static const char* EG_INVOCATION_TYPE = "__eg_invocation";
    static const char* EG_INVOKE_MEMBER_FUNCTION_NAME = "invoke";
    static const char* EG_INVOKE_IMPL_TYPE = "__invoke_impl";
    static const char* EG_REFERENCE_ITERATOR_TYPE = "__eg_ReferenceIterator";
    static const char* EG_MULTI_ITERATOR_TYPE = "__eg_MultiIterator";
    static const char* EG_RESULT_TRAIT_TYPE = "__eg_result_type";
    static const char* EG_RANGE_TYPE = "__eg_Range";
    static const char* EG_RESULT_TYPE = "eg::result_type";
    static const char* EG_FIBER_TYPE = "boost::fibers::fiber";
    static const char* EG_TYPE_PATH_CANNON_TYPE = "eg::CanonicaliseTypePath";
    static const char* EG_ITERATOR_TYPE = "eg::Iterator";
    static const char* EG_REFERENCE_TYPE = "eg::reference";
    static const char* EG_IS_CONVERTIBLE_TYPE = "eg::is_convertible";
    static const char* EG_EVENT_TYPE = "eg::Event";
    static const char* EG_EVENT_LOG_EVENT_TYPE = "eg::_event";
    static const char* EG_EVENT_ITERATOR = "eg::event_iterator";
    static const char* EG_DEPENDENCY_PROVIDER_TYPE = "eg::DependencyProvider";
    //static const char* EG_DEFAULT_FIBER_STACK_TYPE = "boost::fibers::pooled_fixedsize_stack( EG_FIBER_STACK_SIZE )";
    static const char* EG_DEFAULT_FIBER_STACK_TYPE = "boost::fibers::fixedsize_stack( EG_FIBER_STACK_SIZE )";
    
    class Identifiers;
    
    std::string getInterfaceType( const std::string& strType );
    std::string getBaseTraitType( std::size_t szIndex );
    std::string getInterfaceInstantiationType( const std::string& strType, int iDepth );
    
    void generateIncludeHeader( std::ostream& os, const interface::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser );
    
    void generateInterface( std::ostream& os, const interface::Root* pRoot, const Identifiers* pIdentifiers, std::size_t szFiberStackSize );

    void generateOperationSource( std::ostream& os, const interface::Root* pRoot );
    
    void generateBufferStructures( std::ostream& os, const ImplementationSession& program );

    void generateImplementationSource( std::ostream& os, const ImplementationSession& program, std::size_t szTranslationUnitID );
        
    void generate_dynamic_interface( std::ostream& os, ImplementationSession& session );
    void generateActionInstanceFunctions( std::ostream& os, const ImplementationSession& program );
            
    class DataMember;

    struct Printer
    {
        const DataMember* m_pDataMember;
        const char* pszIndex;
        Printer( const DataMember* pDataMember, const char* pszIndex ) : m_pDataMember( pDataMember ), pszIndex( pszIndex ) {}
    };
    
    std::ostream& operator<<( std::ostream& os, const Printer& printer );

    class Layout;
    class CodeGenerator
    {
        static const int INDENT = 4;
    public:
        CodeGenerator( const Layout& layout, int iIndent, const std::string& strFailureType ) 
            :   m_layout( layout ), 
                m_strFailureType( strFailureType )
        {
            for( int i = 0; i != iIndent; ++i )
                pushIndent();
        }
        
        using VariableExprMap = std::map< const Variable*, std::string >;
        
        inline void setVarExpr( const Variable* pVariable, const std::string& str )
        {
            VariableExprMap::const_iterator iFind = m_variables.find( pVariable );
            VERIFY_RTE( iFind == m_variables.end() );
            m_variables.insert( std::make_pair( pVariable, str ) );
        }
        inline const std::string& getVarExpr( const Variable* pVariable ) const
        {
            VariableExprMap::const_iterator iFind = m_variables.find( pVariable );
            VERIFY_RTE( iFind != m_variables.end() );
            return iFind->second;
        }
        
        const std::string& getIndent() const { return m_strIndent; }
        void pushIndent() 
        { 
            for( int i = 0; i != INDENT; ++i )
                m_strIndent.push_back( ' ' );
        }
        void popIndent()
        {
            for( int i = 0; i != INDENT; ++i )
                m_strIndent.pop_back();
        }
        
        Printer getDimension( const concrete::Dimension* pDimension, const std::string& strIndex );
        
        std::string getFailureReturnType() const { return m_strFailureType; }
        
        const Layout& getLayout() const { return m_layout; }
    private:
        VariableExprMap m_variables;
        std::string m_strIndent;
        const Layout& m_layout;
        std::string m_strFailureType;
    };
}

#endif //CODE_GEN_18_04_2019