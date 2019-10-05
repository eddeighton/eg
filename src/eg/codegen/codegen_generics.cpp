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


#include "codegen.hpp"

#include "../eg.hpp"
#include "../input.hpp"
#include "../interface.hpp"
#include "../concrete.hpp"
#include "../derivation.hpp"
#include "../layout.hpp"
#include "../identifiers.hpp"
#include "../derivation.hpp"
#include "../invocation.hpp"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>

namespace eg
{
    
    struct SpecialMemberFunctionVisitor
    {
        std::ostream& os;
        const std::vector< const concrete::Action* >& instances;
        const std::vector< const concrete::Inheritance_Node* >& iNodes;
        const Layout& layout;
        std::string strIndent;
        
        SpecialMemberFunctionVisitor( std::ostream& os, 
            const std::vector< const concrete::Action* >& instances, 
            const std::vector< const concrete::Inheritance_Node* >& iNodes,
            const Layout& layout ) 
        :   os( os ), 
            instances( instances ), 
            iNodes( iNodes ), 
            layout( layout )
        {
            
        }
        
        
        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }      
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Using*   pElement, const interface::Element* pNode )
        {    
        }  
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {    
            push( (input::Action*) pElement, pNode );
        }    
        void push ( const input::Action*    pElement, const interface::Element* pNode )
        {
            //calculate the path to the root type
            std::vector< const interface::Element* > path = getPath( pNode );
            
            //generate type comment
            {
                os << "\n//";
                for( const interface::Element* pNodeIter : path )
                {
                    if( pNodeIter != *path.begin())
                        os << "::";
                    os << pNodeIter->getIdentifier();
                }
                os << "\n";
            }
            
            //generate the template argument lists
            bool bMultipleElements = false;
            std::ostringstream osTemplateArgLists;
            std::ostringstream osTemplateArgListsSpecialised;
            {
                int iCounter = 1;
                for( const interface::Element* pNodeIter : path )
                {
                    osTemplateArgLists << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << iCounter << " >\n";
                    osTemplateArgListsSpecialised << strIndent << "template<>\n";
                    ++iCounter;
                }
                if( iCounter > 2 )
                    bMultipleElements = true;
            }
            
            std::ostringstream osTypeName;
            {
                int iCounter = 1;
                for( const interface::Element* pNodeIter : path )
                {
                    if( pNodeIter != *path.begin())
                        osTypeName << "::";
                    osTypeName << getInterfaceType( pNodeIter->getIdentifier() ) << 
                        "< " << EG_INTERFACE_PARAMETER_TYPE << iCounter << " >";
                    ++iCounter;
                }
            }
            
            std::ostringstream osTypeNameAsType;
            {
                int iCounter = 1;
                for( const interface::Element* pNodeIter : path )
                {
                    if( pNodeIter != *path.begin())
                        osTypeNameAsType << "::template ";
                    else if( bMultipleElements )
                        osTypeNameAsType << "typename ";
                    osTypeNameAsType << getInterfaceType( pNodeIter->getIdentifier() ) << 
                        "< " << EG_INTERFACE_PARAMETER_TYPE << iCounter << " >";
                    ++iCounter;
                }
            }
            std::ostringstream osTypeVoid;
            {
                for( const interface::Element* pNodeIter : path )
                {
                    if( pNodeIter != *path.begin())
                        osTypeVoid << "::";
                    osTypeVoid << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
                }
            }
            
            std::string strActionInterfaceType = getInterfaceType( pNode->getIdentifier() );
            
            const interface::Action* pNodeAction = dynamic_cast< const interface::Action* >( pNode );
            
            std::set< const interface::Action*, CompareIndexedObjects > staticCompatibleTypes;
            std::set< const concrete::Action*, CompareIndexedObjects > dynamicCompatibleTypes;
            {
                for( const concrete::Inheritance_Node* pINode : iNodes )
                {
                    if( pINode->getAbstractAction() == pNodeAction )
                    {
                        for( const concrete::Inheritance_Node* pINodeIter = pINode; pINodeIter; pINodeIter = pINodeIter->getParent() )
                        {
                            staticCompatibleTypes.insert( pINodeIter->getAbstractAction() );
                            dynamicCompatibleTypes.insert( pINodeIter->getRootConcreteAction() );
                        }
                        pINode->getStaticDerived( staticCompatibleTypes );
                        pINode->getDynamicDerived( dynamicCompatibleTypes );
                    }
                }
            }
            
            //conversion traits
            for( const interface::Action* pCompatible : staticCompatibleTypes )
            {
                std::ostringstream osCompatibleTypeName;
                {
                    std::vector< const interface::Element* > compatiblePath = getPath( pCompatible );
                    for( const interface::Element* pNodeIter : compatiblePath )
                    {
                        if( pNodeIter != *path.begin())
                            osCompatibleTypeName << "::";
                        osCompatibleTypeName << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
                    }
                }
            os << "template<>\n";
            os << "struct " << EG_IS_CONVERTIBLE_TYPE << "< " << osCompatibleTypeName.str() << ", " << osTypeVoid.str() << " >\n";
            os << "{\n";
            os << "    static constexpr const bool value = true;\n";
            os << "};\n";
            }
            
            //conversion constructor
            os << osTemplateArgLists.str();
            os << "template< typename TFrom >\n";
            os << "inline " << osTypeName.str() << "::" << strActionInterfaceType << "( const TFrom& from )\n";
            os << "{\n";
            os << "  static_assert( " << EG_IS_CONVERTIBLE_TYPE << "< TFrom, " << osTypeVoid.str() << " >::value, \"Incompatible eg type conversion\" );\n";
            if( !dynamicCompatibleTypes.empty() )
            {
            os << "  switch( from.data.type )\n";
            os << "  {\n";
            for( const concrete::Action* pCompatible : dynamicCompatibleTypes )
            {
            os << "     case " << pCompatible->getIndex() << ": //" << pCompatible->getFriendlyName() << "\n";
            }
            os << "         data = from.data;\n";
            os << "         break;\n";
            os << "     default:\n";
            os << "         data.timestamp = " << EG_INVALID_TIMESTAMP << ";\n";
            os << "         break;\n";
            os << "  }\n";
            }
            else
            {
            os << "    data.timestamp = " << EG_INVALID_TIMESTAMP << ";\n";
            }
            os << "}\n";
            
            //assignment operator
            os << osTemplateArgLists.str();
            os << "template< typename TFrom >\n";
            os << "inline " << osTypeNameAsType.str() << "& " << osTypeName.str() << "::operator=( const TFrom& from )\n";
            os << "{\n";
            os << "  static_assert( " << EG_IS_CONVERTIBLE_TYPE << "< TFrom, " << osTypeVoid.str() << " >::value, \"Incompatible eg type conversion\" );\n";
            if( !dynamicCompatibleTypes.empty() )
            {
            os << "  switch( from.data.type )\n";
            os << "  {\n";
            for( const concrete::Action* pCompatible : dynamicCompatibleTypes )
            {
            os << "      case " << pCompatible->getIndex() << ": //" << pCompatible->getFriendlyName() << "\n";
            }
            os << "         data = from.data;\n";
            os << "         break;\n";
            os << "     default:\n";
            os << "         data.timestamp = " << EG_INVALID_TIMESTAMP << ";\n";
            os << "         break;\n";
            os << "  }\n";
            }
            else
            {
            os << "    data.timestamp = " << EG_INVALID_TIMESTAMP << ";\n";
            }
            os << "  return *this;\n";
            os << "}\n";
            
            //getTimestamp
            if( !dynamicCompatibleTypes.empty() )
            {
            os << "template<>\n";
            os << "inline " << EG_TIME_STAMP << " getTimestamp< " << osTypeVoid.str() << " >( " << EG_TYPE_ID << " type, " << EG_INSTANCE << " instance )\n";
            os << "{\n";
            if( dynamicCompatibleTypes.size() > 1 )
            {
            os << "    switch( type )\n";
            os << "    {\n";
            for( const concrete::Action* pCompatible : dynamicCompatibleTypes )
            {
                const DataMember* pReference = layout.getDataMember( pCompatible->getReference() );
            os << "      case " << pCompatible->getIndex() << ": //" << pCompatible->getFriendlyName() << "\n";
            os << "         return " << Printer( pReference, "instance" ) << ".data.timestamp;\n";
            }
            os << "      default: return " << EG_INVALID_TIMESTAMP << ";\n";
            os << "    }\n";
            }
            else //if( dynamicCompatibleTypes.size() == 1 )
            {
                const concrete::Action* pCompatible = *dynamicCompatibleTypes.begin();
                const DataMember* pReference = layout.getDataMember( pCompatible->getReference() );
            os << "    return " << Printer( pReference, "instance" ) << ".data.timestamp;\n";
            }
            os << "}\n";
            os << "\n";
            }
            
            //getStopCycle
            if( !dynamicCompatibleTypes.empty() )
            {
            os << "template<>\n";
            os << "inline " << EG_TIME_STAMP << " getStopCycle< " << osTypeVoid.str() << " >( " << EG_TYPE_ID << " type, " << EG_INSTANCE << " instance )\n";
            os << "{\n";
            if( dynamicCompatibleTypes.size() > 1 )
            {
            os << "    switch( type )\n";
            os << "    {\n";
            for( const concrete::Action* pCompatible : dynamicCompatibleTypes )
            {
                const DataMember* pReference = layout.getDataMember( pCompatible->getStopCycle() );
            os << "      case " << pCompatible->getIndex() << ": //" << pCompatible->getFriendlyName() << "\n";
            os << "         return " << Printer( pReference, "instance" ) << ";\n";
            }
            os << "      default: return " << EG_INVALID_TIMESTAMP << ";\n";
            os << "    }\n";
            }
            else //if( dynamicCompatibleTypes.size() == 1 )
            {
                const concrete::Action* pCompatible = *dynamicCompatibleTypes.begin();
                const DataMember* pReference = layout.getDataMember( pCompatible->getStopCycle() );
            os << "    return " << Printer( pReference, "instance" ) << ";\n";
            }
            os << "}\n";
            os << "\n";
            }
            
            //getState
            if( !dynamicCompatibleTypes.empty() )
            {
            os << "template<>\n";
            os << "inline " << EG_ACTION_STATE << " getState< " << osTypeVoid.str() << " >( " << EG_TYPE_ID << " type, " << EG_INSTANCE << " instance )\n";
            os << "{\n";
            if( dynamicCompatibleTypes.size() > 1 )
            {
            os << "    switch( type )\n";
            os << "    {\n";
            for( const concrete::Action* pCompatible : dynamicCompatibleTypes )
            {
                const DataMember* pState = layout.getDataMember( pCompatible->getState() );
            os << "      case " << pCompatible->getIndex() << ": //" << pCompatible->getFriendlyName() << "\n";
            os << "         return " << Printer( pState, "instance" ) << ";\n";
            }
            os << "      default: return " << EG_INVALID_STATE << ";\n";
            os << "    }\n";
            }
            else //if( dynamicCompatibleTypes.size() == 1 )
            {
                const concrete::Action* pCompatible = *dynamicCompatibleTypes.begin();
                const DataMember* pState = layout.getDataMember( pCompatible->getState() );
            os << "    return " << Printer( pState, "instance" ) << ";\n";
            }
            os << "}\n";
            os << "\n";
            }
            
        }
        void pop ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }     
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Using*   pElement, const interface::Element* pNode )
        {    
        }  
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };
    
    
    struct InvokeVisitor
    {
        std::ostream& os;
        const eg::TranslationUnit& m_translationUnit;
        std::string strIndent;
        
        InvokeVisitor( std::ostream& os, const eg::TranslationUnit& translationUnit ) : os( os ), m_translationUnit( translationUnit ) {}
        
        
        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }      
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Using*   pElement, const interface::Element* pNode )
        {    
        }  
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {    
            push( (input::Action*) pElement, pNode );
        }    
        void push ( const input::Action*    pElement, const interface::Element* pNode )
        {
            const interface::Action* pAction = dynamic_cast< const interface::Action* >( pNode );
            VERIFY_RTE( pAction );
            
            //TODO ELIMINATION: only generate ::invoke member function definitions for the set of contexts the translation unit 
            //actually uses - 
            {
                //calculate the path to the root type
                std::vector< const interface::Element* > path = getPath( pNode );
                
                //generate type comment
                {
                    os << "\n//";
                    for( const interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            os << "::";
                        os << pNodeIter->getIdentifier();
                    }
                    os << "\n";
                }
                
                //generate the template argument lists
                {
                    int iCounter = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << iCounter << " >\n";
                        ++iCounter;
                    }
                }
                
                //generate the member function template arguments
                os << "template< typename TypePath, typename Operation, typename... Args >\n";
                
                std::ostringstream osTypeName;
                {
                    int iCounter = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            osTypeName << "::";
                        osTypeName << getInterfaceType( pNodeIter->getIdentifier() ) << 
                            "< " << EG_INTERFACE_PARAMETER_TYPE << iCounter << " >";
                        ++iCounter;
                    }
                }
                
                //generate the return type
                if( path.size() > 1U )
                {
                    //if multiple elements then need typename and use of template keyword
                    os << "inline typename " << EG_RESULT_TYPE << "< typename ";
                    int iCounter = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            os << "::template ";
                        os << getInterfaceType( pNodeIter->getIdentifier() ) << 
                            "< " << EG_INTERFACE_PARAMETER_TYPE << iCounter << " >";
                        ++iCounter;
                    }
                    os << ", TypePath, Operation >::Type\n";
                }
                else
                {
                    os << "inline typename " << EG_RESULT_TYPE << "< " << osTypeName.str() << ", TypePath, Operation >::Type\n";
                }
                
                //generate the invoke member function name
                os << osTypeName.str() << "::" << EG_INVOKE_MEMBER_FUNCTION_NAME << "( Args... args ) const\n";
                os << strIndent << "{\n";
                strIndent.push_back( ' ' );
                strIndent.push_back( ' ' );
                
                //generate the implementation
                os << "    using CanonicalTypePathType = typename " << EG_TYPE_PATH_CANNON_TYPE << "< TypePath >::Type;\n";
                os << "    return " << EG_INVOKE_IMPL_TYPE << "< typename " << EG_RESULT_TYPE << "< " << 
                    osTypeName.str() << ", TypePath, Operation >::Type, " << 
                    osTypeName.str() << ", CanonicalTypePathType, Operation >()( *this, args... );\n";
                
                strIndent.pop_back();
                strIndent.pop_back();
                os << strIndent << "}\n";
            }
        }
        void pop ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }     
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Using*   pElement, const interface::Element* pNode )
        {    
        }  
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };
    
    
    void generateGenerics(std::ostream& os, 
        const ImplementationSession& program, 
        const std::vector< const concrete::Action* >& actions, 
        const std::vector< const concrete::Inheritance_Node* >& iNodes,
        const eg::TranslationUnit& translationUnit )
    {
        const Layout& layout = program.getLayout();
        const interface::Root* pRoot = program.getTreeRoot();
        
        
        //generate the invoke definitions
        
        //generic one for variant
        os << "\n//generic variant invocation adaptor\n";
        os << "template< typename... Ts >\n";
        os << "template< typename TypePath, typename Operation, typename... Args >\n";
        os << "inline typename " << EG_RESULT_TYPE << "< " << EG_VARIANT_TYPE << "< Ts... >, TypePath, Operation >::Type\n";
        os << EG_VARIANT_TYPE << "< Ts... >::" << EG_INVOKE_MEMBER_FUNCTION_NAME << "( Args... args )\n";
        os << "{\n";
        os << "    using CanonicalTypePathType = typename " << EG_TYPE_PATH_CANNON_TYPE << "< TypePath >::Type;\n";
        os << "    return " << EG_INVOKE_IMPL_TYPE << "< typename " << EG_RESULT_TYPE << "< " << EG_VARIANT_TYPE << 
                "< Ts... >, TypePath, Operation >::Type, " << EG_VARIANT_TYPE << 
                "< Ts... >, CanonicalTypePathType, Operation >()( *this, args... );\n";
        os << "}\n";
        os << "\n";
            
        std::vector< const InvocationSolution* > invocations;
        program.getInvocations( translationUnit.getDatabaseFileID(), invocations );
        
        //generate variant getTimestamp functions
        std::set< InvocationSolution::Context > variantTypes;
        for( const InvocationSolution* pInvocation : invocations )
        {
            const InvocationSolution::Context& returnTypes = pInvocation->getReturnTypes();
            if( returnTypes.size() > 1U )
            {
                if( variantTypes.count( returnTypes ) == 0U )
                {
                    variantTypes.insert( returnTypes );
                    
                    //std::set< const interface::Action*, CompareIndexedObjects > staticCompatibleTypes;
                    std::set< const concrete::Action*, CompareIndexedObjects > dynamicCompatibleTypes;
                    {
                        for( const concrete::Inheritance_Node* pINode : iNodes )
                        {
                            if( std::find( returnTypes.begin(), returnTypes.end(), pINode->getAbstractAction() ) != returnTypes.end() )
                            //if( pINode->getAbstractAction() == pNodeAction )
                            {
                                for( const concrete::Inheritance_Node* pINodeIter = pINode; pINodeIter; pINodeIter = pINodeIter->getParent() )
                                {
                                    //staticCompatibleTypes.insert( pINodeIter->getAbstractAction() );
                                    dynamicCompatibleTypes.insert( pINodeIter->getRootConcreteAction() );
                                }
                                //pINode->getStaticDerived( staticCompatibleTypes );
                                pINode->getDynamicDerived( dynamicCompatibleTypes );
                            }
                        }
                    }
                    ASSERT( !dynamicCompatibleTypes.empty() );
                    if( !dynamicCompatibleTypes.empty() )
                    {
        os << "template<>\n";
        os << "inline " << EG_TIME_STAMP << " getTimestamp< ";
        printActionType( os, returnTypes );
        os << " >( " << EG_TYPE_ID << " type, " << EG_INSTANCE << " instance )\n";
        os << "{\n";
        os << "    switch( type )\n";
        os << "    {\n";
        
        for( const concrete::Action* pConcreteAction : dynamicCompatibleTypes )
        {
            const DataMember* pReference = layout.getDataMember( pConcreteAction->getReference() );
        os << "        case " << pConcreteAction->getIndex() << ": //" << pConcreteAction->getFriendlyName() << "\n";
        os << "            return " << Printer( pReference, "instance" ) << ".data.timestamp;\n";
        }
        os << "        default: return " << EG_INVALID_TIMESTAMP << ";\n";
        
        os << "    }\n";
        os << "}\n";
        
        os << "template<>\n";
        os << "inline " << EG_TIME_STAMP << " getStopCycle< ";
        printActionType( os, returnTypes );
        os << " >( " << EG_TYPE_ID << " type, " << EG_INSTANCE << " instance )\n";
        os << "{\n";
        os << "    switch( type )\n";
        os << "    {\n";
        
        for( const concrete::Action* pConcreteAction : dynamicCompatibleTypes )
        {
            const DataMember* pReference = layout.getDataMember( pConcreteAction->getStopCycle() );
        os << "        case " << pConcreteAction->getIndex() << ": //" << pConcreteAction->getFriendlyName() << "\n";
        os << "            return " << Printer( pReference, "instance" ) << ";\n";
        }
        os << "        default: return " << EG_INVALID_TIMESTAMP << ";\n";
        
        os << "    }\n";
        os << "}\n";
                    
        os << "template<>\n";
        os << "inline " << EG_ACTION_STATE << " getState< ";
        printActionType( os, returnTypes );
        os << " >( " << EG_TYPE_ID << " type, " << EG_INSTANCE << " instance )\n";
        os << "{\n";
        os << "    switch( type )\n";
        os << "    {\n";
        
        for( const concrete::Action* pConcreteAction : dynamicCompatibleTypes )
        {
            const DataMember* pState = layout.getDataMember( pConcreteAction->getState() );
        os << "        case " << pConcreteAction->getIndex() << ": //" << pConcreteAction->getFriendlyName() << "\n";
        os << "            return " << Printer( pState, "instance" ) << ";\n";
        }
        os << "        default: return " << EG_INVALID_STATE << ";\n";
        
        os << "    }\n";
        os << "}\n";
                    }
                }
            }
        }
        
        {
            SpecialMemberFunctionVisitor visitor( os, actions, iNodes, layout );
            pRoot->pushpop( visitor );
        }
        {
            InvokeVisitor visitor( os, translationUnit );
            pRoot->pushpop( visitor );
        }
        
    }
    
    
}