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

#include "eg.hpp"
#include "input.hpp"
#include "interface.hpp"
#include "concrete.hpp"
#include "derivation.hpp"
#include "layout.hpp"
#include "identifiers.hpp"
#include "derivation.hpp"
#include "invocation.hpp"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>

namespace eg
{
    
    std::string getInterfaceType( const std::string& strType )
    {
        return EG_INTERFACE_PREFIX + strType;
    }
    std::string getBaseTraitType( std::size_t szIndex )
    {
        std::ostringstream os;
        os << EG_INTERFACE_PREFIX << EG_BASE_PREFIX << szIndex;
        return os.str();
    }
    
    std::string getInterfaceInstantiationType( const std::string& strType, int iDepth )
    {
        std::ostringstream os;
        os << getInterfaceType( strType ) << "< " << EG_INTERFACE_PARAMETER_TYPE << iDepth << " >";
        return os.str(); 
    }
        
    int eds_isspace( int ch )
    {
        return std::isspace( ch );
    }

    int eds_isalnum( int ch )
    {
        return std::isalnum( ch );
    }
    
    std::string style_replace_non_alpha_numeric( const std::string& str, char r )
    {
        std::string strResult;
        std::replace_copy_if( str.begin(), str.end(), std::back_inserter( strResult ), 
                              !boost::bind( &eds_isalnum, _1 ), r );
        return strResult;
    }
    
    void generateIncludeGuard( std::ostream& os, const char* pszName )
    {
        os << "#ifndef EG_INTERFACE_GUARD_" << pszName << "\n";
        os << "#define EG_INTERFACE_GUARD_" << pszName << "\n";
        os << pszLine << pszLine << "\n";
    }
    
    
    
    
    void generateBufferStructures( std::ostream& os, const ImplementationSession& program )
    {
        const Layout& layout = program.getLayout();
        
        generateIncludeGuard( os, "STRUCTURES" );
        
        os << "//data structures\n";
        for( const Buffer* pBuffer : layout.getBuffers() )
        {
            os << "\n//Buffer: " << pBuffer->getTypeName();
            if( const concrete::Action* pAction = pBuffer->getAction() )
            {
                os << " type: " << pAction->getIndex();
            }
            
            os << /*" stride: " << pBuffer->getStride() <<*/ " size: " << pBuffer->getSize() << "\n";
            os << "struct " << pBuffer->getTypeName() << "\n{\n";
            std::size_t szBufferSize = 0U;
            for( const DataMember* pDimension : pBuffer->getDimensions() )
            {
                os << "    ";
                pDimension->print( os );
                os << " " << pDimension->getName() << ";\n";
                szBufferSize += pDimension->getInstanceDimension()->getDataSize();
            }
            
            os << "};\n";
            os << "extern " << pBuffer->getTypeName() << " *" << pBuffer->getVariableName() << ";\n";
            //os << "static_assert( sizeof( " << pBuffer->getTypeName() << " ) == " << szBufferSize << ", \"Incorrect buffer size\" );\n";
            
        }
        
        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }
    
    struct SpecialMemberFunctionVisitor
    {
        std::ostream& os;
        std::vector< const concrete::Action* >& instances;
        std::vector< const concrete::Inheritance_Node* >& iNodes;
        const Layout& layout;
        std::string strIndent;
        
        SpecialMemberFunctionVisitor( std::ostream& os, 
            std::vector< const concrete::Action* >& instances, 
            std::vector< const concrete::Inheritance_Node* >& iNodes,
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
            os << osTypeName.str() << "::" << strActionInterfaceType << "( const TFrom& from )\n";
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
            os << osTypeNameAsType.str() << "& " << osTypeName.str() << "::operator=( const TFrom& from )\n";
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
                    os << "typename " << EG_RESULT_TYPE << "< typename ";
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
                    os << "typename " << EG_RESULT_TYPE << "< " << osTypeName.str() << ", TypePath, Operation >::Type\n";
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
    
    void printActionType( std::ostream& os, const InvocationSolution::Context& returnTypes )
    {
        if( returnTypes.size() > 1 )
        {
            os << EG_VARIANT_TYPE << "< ";
            bool bFirst = true;
            for( const interface::Element* pTarget : returnTypes )
            {
                if( !bFirst )
                    os << ", ";
                else
                    bFirst = false;
                os << pTarget->getStaticType();
            }
            os << " >";
        }
        else if( !returnTypes.empty() )
        {
            os << returnTypes.front()->getStaticType();
        }
        else
        {
            os << "void";
        }
    }
    
    void printReturnType( std::ostream& os, const IndexedObject::Array& objects, const InvocationSolution& invocation )
    {
        const InvocationSolution::Context& returnTypes = invocation.getReturnTypes();
        switch( invocation.getOperation() )
        {
            case id_Imp_NoParams   :
            case id_Imp_Params  :
                if( !invocation.isReturnTypeDimensions() )
                {
                    printActionType( os, returnTypes );
                }
                else if( invocation.getOperation() == id_Imp_NoParams )
                {
                    ASSERT( !returnTypes.empty() );
                    ASSERT( invocation.isDimensionReturnTypeHomogeneous() );
                    os << returnTypes.front()->getStaticType() << "::Read";
                }
                else if( invocation.getOperation() == id_Imp_Params )
                {
                    os << "void";
                }
                break;
            case id_Start      : 
                printActionType( os, returnTypes );
                break;
            case id_Stop       : 
            case id_Pause      : 
            case id_Resume     : 
                os << "void";
                break;
            case id_Wait        :
                if( invocation.isReturnTypeDimensions() )
                {
                    ASSERT( invocation.isDimensionReturnTypeHomogeneous() );
                    os << returnTypes.front()->getStaticType() << "::Read";
                }
                else
                {
                    printActionType( os, returnTypes );
                }
                break;
            case id_Get        :
                if( invocation.isReturnTypeDimensions() )
                {
                    ASSERT( invocation.isDimensionReturnTypeHomogeneous() );
                    os << returnTypes.front()->getStaticType() << "::Get";
                }
                else
                {
                    printActionType( os, returnTypes );
                }
                break;
            case id_Done       :
                os << "bool";
                break;
            case id_Range      : 
                if( invocation.getRoot()->getMaxRanges() == 1 )
                {
                    if( returnTypes.size() == 1 )
                    {
                        os << returnTypes.front()->getStaticType() << "::EGRangeType";
                    }
                    else
                    {
                        std::ostringstream osType;
                        {
                            osType << EG_VARIANT_TYPE << "< ";
                            for( const interface::Element* pElement : returnTypes )
                            {
                                const interface::Action* pReturnType = 
                                    dynamic_cast< const interface::Action* >( pElement );
                                ASSERT( pReturnType );
                                if( pElement != *returnTypes.begin())
                                    osType << ", ";
                                osType << pReturnType->getStaticType();
                            }
                            osType << " >";
                        }
                        std::ostringstream osIterType;
                        {
                            osIterType << EG_REFERENCE_ITERATOR_TYPE << "< " << osType.str() << " >";
                        }
                        os << EG_RANGE_TYPE << "< " << osIterType.str() << " >";
                    }
                }
                else
                {
                    if( returnTypes.size() == 1 )
                    {
                        os << EG_RANGE_TYPE << "< " << EG_MULTI_ITERATOR_TYPE << "< " << 
                            EG_REFERENCE_ITERATOR_TYPE << "< " << returnTypes.front()->getStaticType() << " >, " 
                                << invocation.getRoot()->getMaxRanges() << "U > >";
                    }
                    else
                    {
                        std::ostringstream osType;
                        {
                            osType << EG_VARIANT_TYPE << "< ";
                            for( const interface::Element* pElement : returnTypes )
                            {
                                const interface::Action* pReturnType = 
                                    dynamic_cast< const interface::Action* >( pElement );
                                ASSERT( pReturnType );
                                if( pElement != *returnTypes.begin())
                                    osType << ", ";
                                osType << pReturnType->getStaticType();
                            }
                            osType << " >";
                        }
                        os << EG_RANGE_TYPE << "< " << EG_MULTI_ITERATOR_TYPE << "< " << 
                            EG_REFERENCE_ITERATOR_TYPE << "< " << osType.str() << " >, " << 
                            invocation.getRoot()->getMaxRanges() << "U > >";
                    }
                }
                break;
            case id_Raw      : 
                if( invocation.getRoot()->getMaxRanges() == 1 )
                {
                    if( returnTypes.size() == 1 )
                    {
                        os << EG_RANGE_TYPE << "< " << 
                            EG_REFERENCE_RAW_ITERATOR_TYPE << "< " << 
                                returnTypes.front()->getStaticType() << " > >";
                    }
                    else
                    {
                        std::ostringstream osType;
                        {
                            osType << EG_VARIANT_TYPE << "< ";
                            for( const interface::Element* pElement : returnTypes )
                            {
                                const interface::Action* pReturnType = 
                                    dynamic_cast< const interface::Action* >( pElement );
                                ASSERT( pReturnType );
                                if( pElement != *returnTypes.begin())
                                    osType << ", ";
                                osType << pReturnType->getStaticType();
                            }
                            osType << " >";
                        }
                        std::ostringstream osIterType;
                        {
                            osIterType << EG_REFERENCE_RAW_ITERATOR_TYPE << "< " << osType.str() << " >";
                        }
                        os << EG_RANGE_TYPE << "< " << osIterType.str() << " >";
                    }
                }
                else
                {
                    if( returnTypes.size() == 1 )
                    {
                        os << EG_RANGE_TYPE << "< " << EG_MULTI_ITERATOR_TYPE << "< " << 
                            EG_REFERENCE_RAW_ITERATOR_TYPE << "< " << returnTypes.front()->getStaticType() << " >, " 
                                << invocation.getRoot()->getMaxRanges() << "U > >";
                    }
                    else
                    {
                        std::ostringstream osType;
                        {
                            osType << EG_VARIANT_TYPE << "< ";
                            for( const interface::Element* pElement : returnTypes )
                            {
                                const interface::Action* pReturnType = 
                                    dynamic_cast< const interface::Action* >( pElement );
                                ASSERT( pReturnType );
                                if( pElement != *returnTypes.begin())
                                    osType << ", ";
                                osType << pReturnType->getStaticType();
                            }
                            osType << " >";
                        }
                        os << EG_RANGE_TYPE << "< " << EG_MULTI_ITERATOR_TYPE << "< " << 
                            EG_REFERENCE_RAW_ITERATOR_TYPE << "< " << osType.str() << " >, " << 
                            invocation.getRoot()->getMaxRanges() << "U > >";
                    }
                }
                break;
            default:
                THROW_RTE( "Unknown operation type" );
                break;
        }
    }
    
    void printParameters( std::ostream& os, const IndexedObject::Array& objects, const InvocationSolution& invocation )
    {
        const InvocationSolution::Context& returnTypes = invocation.getReturnTypes();
        ASSERT( !returnTypes.empty() );
        switch( invocation.getOperation() )
        {
            case id_Imp_NoParams   :
            case id_Imp_Params  :
                if( !invocation.isReturnTypeDimensions() )
                {
                    
                }
                else if( invocation.getOperation() == id_Imp_NoParams )
                {
                }
                else if( invocation.getOperation() == id_Imp_Params )
                {
                    os << returnTypes.front()->getStaticType() << "::Write value";
                }
                break;
            case id_Start      : 
                break;
            case id_Stop       : 
            case id_Pause      : 
            case id_Resume     : 
            case id_Wait       : 
            case id_Get        :
            case id_Done       :
            case id_Range      : 
            case id_Raw        : 
                break;
            default:
                THROW_RTE( "Unknown operation type" );
                break;
        }
    }
    
    void printContextType( std::ostream& os, const IndexedObject::Array& objects, const InvocationSolution& invocation )
    {
        printActionType( os, invocation.getContext() );
    }
    
    void printTypePathType( std::ostream& os, const IndexedObject::Array& objects, const InvocationSolution& invocation )
    {
        os << EG_TYPE_PATH << "< ";
            
        bool bFirst = true;
        const std::vector< TypeID >& typePath = invocation.getImplicitTypePath();
        for( TypeID id : typePath )
        {
            if( bFirst)
                bFirst = false;
            else
                os << ", ";
            if( isOperationType( id ) )
            {
                os << getOperationString( static_cast< OperationID >( id ) );
            }
            else if( id < 0 )
            {
                const interface::Element* pElement = dynamic_cast< const interface::Element* >( objects[ -id ] );
                ASSERT( pElement );
                switch( pElement->getType() )
                {
                    case eAbstractDimension : os << dynamic_cast< const interface::Dimension* >(  pElement )->getIdentifier(); break;
                    case eAbstractAction    : os << dynamic_cast< const interface::Action* >(     pElement )->getIdentifier(); break;
                    case eAbstractUsing     : os << dynamic_cast< const interface::Using* >(      pElement )->getIdentifier(); break;
                    case eAbstractRoot      : 
                    case eAbstractOpaque    :
                    case eAbstractInclude   :
                    default:
                        THROW_RTE( "Unsupported type" );
                        break;
                }
            }
            else
            {
                ASSERT( id < static_cast< TypeID >( objects.size() ) );
                const interface::Element* pElement = dynamic_cast< const interface::Element* >( objects[ id ] );
                os << pElement->getStaticType();
            }
        }
        os << " >";
    }
    
    
    Printer CodeGenerator::getDimension( const concrete::Dimension* pDimension, const std::string& strIndex )
    {
        return Printer( m_layout.getDataMember( pDimension ), strIndex.c_str() );
    }
    
    std::ostream& operator<<( std::ostream& os, const Printer& printer )
    {
        printer.m_pDataMember->printVariableAccess( os, printer.pszIndex );
        return os;
    }
    
    void generateInvocation( std::ostream& os, const IndexedObject::Array& objects,
        const DerivationAnalysis& derivationAnalysis, 
        const Layout& layout, const InvocationSolution& invocation )
    {
        os << "template<>\n";
        os << "struct " << EG_INVOKE_IMPL_TYPE << "\n";
        os << "<\n";
        os << "    "; printReturnType( os, objects, invocation ); os << ",\n";
        os << "    "; printContextType( os, objects, invocation ); os << ",\n";
        os << "    "; printTypePathType( os, objects, invocation ); os << ",\n";
        os << "    " << getOperationString( static_cast< OperationID >( invocation.getOperation() ) ) << "\n";
        os << ">\n";
        os << "{\n";
        //os << "    template< typename... Args >\n";
        
        switch( invocation.getOperation() )
        {
            case id_Imp_NoParams   :
            case id_Imp_Params  :
                if( !invocation.isReturnTypeDimensions() )
                {
                    os << "    template< typename... Args >\n";
                }
                else if( invocation.getOperation() == id_Imp_NoParams )
                {
                }
                else if( invocation.getOperation() == id_Imp_Params )
                {
                }
                break;
            case id_Start      : 
                os << "    template< typename... Args >\n";
                break;
            case id_Stop       : 
            case id_Pause      : 
            case id_Resume     : 
            case id_Wait       : 
            case id_Get        :
            case id_Done       :
            case id_Range      :
            case id_Raw        : 
                break;
            case TOTAL_OPERATION_TYPES : 
            default:
                THROW_RTE( "Unknown operation type" );
        }
        
        
        os << "    "; printReturnType( os, objects, invocation ); os << " operator()( "; printContextType( os, objects, invocation ); os << " context"; 
        
        //invocation parameters
        switch( invocation.getOperation() )
        {
            case id_Imp_NoParams   :
            case id_Imp_Params  :
                if( !invocation.isReturnTypeDimensions() )
                {
                    os << ", Args... args )\n";
                }
                else if( invocation.getOperation() == id_Imp_NoParams )
                {
                    os << " )\n";
                }
                else if( invocation.getOperation() == id_Imp_Params )
                {
                    os << ", "; printParameters( os, objects, invocation ); os << " )\n";
                }
                break;
                
            case id_Start      : 
                os << ", Args... args )\n";
                break;
            case id_Stop       : 
            case id_Pause      : 
            case id_Resume     : 
            case id_Wait       : 
            case id_Get        :
            case id_Done       :
            case id_Range      : 
            case id_Raw        :   
                os << " )\n";
                break;
            case TOTAL_OPERATION_TYPES : 
            default:
                THROW_RTE( "Unknown operation type" );
        }
        
        os << "    {\n";
        
        CodeGenerator codeGenerator( layout, 2, "eg::Event()" );
        invocation.getRoot()->generate( codeGenerator, os );
        
        os << "    }\n";
        os << "};\n";
    }
    
    void generateActionInstanceFunctionsForwardDecls( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
    {
        pAction->printType( os ); os << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _gid );\n";
        os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid );\n";
    }
    
    
    void generateImplementationSource( std::ostream& os, const ImplementationSession& program, 
            const eg::TranslationUnit& translationUnit )
    {
        const interface::Root* pRoot = program.getTreeRoot();
        
        const DerivationAnalysis& derivationAnalysis = program.getDerivationAnalysis();
        const Layout& layout = program.getLayout();
        
        const IndexedObject::Array& objects = program.getObjects( eg::IndexedObject::MASTER_FILE );
        
        os << "#include \"structures.hpp\"\n\n";
                
        os << "\n";
        os << "//input::Action function forward declarations\n";
        std::vector< const concrete::Action* > actions = 
            many_cst< concrete::Action >( objects );
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() )
            {
                os << "\n";
                generateActionInstanceFunctionsForwardDecls( os, layout, pAction );
            }
        }
        os << "\n";
        
        os << "\n\n//invocation implementations\n";
        
        os << "template< typename ResultType, typename ContextType, typename TypePathType, typename OperationType >\n";
        os << "struct " << EG_INVOKE_IMPL_TYPE;
        os << "{\n";
        os << "    template< typename... Args >\n";
        os << "    ResultType operator()( ContextType, Args... )\n";
        os << "    {\n";
        os << "        static_assert( 0 && typeid( ResultType ).name() && typeid( ContextType ).name() && typeid( TypePathType ).name(), " << 
            "\"Critical error: Invocation system failed to match implementation\" );\n";
        os << "    }\n";
        os << "};\n";
        
        std::vector< const InvocationSolution* > invocations;
        program.getInvocations( translationUnit.getDatabaseFileID(), invocations );
        for( const InvocationSolution* pInvocation : invocations )
        {
            os << "\n";
            generateInvocation( os, objects, derivationAnalysis, layout, *pInvocation );
        }
        
        //generate the invoke definitions
        
        //generic one for variant
        os << "\n//generic variant invocation adaptor\n";
        os << "template< typename... Ts >\n";
        os << "template< typename TypePath, typename Operation, typename... Args >\n";
        os << "typename " << EG_RESULT_TYPE << "< " << EG_VARIANT_TYPE << "< Ts... >, TypePath, Operation >::Type\n";
        os << EG_VARIANT_TYPE << "< Ts... >::" << EG_INVOKE_MEMBER_FUNCTION_NAME << "( Args... args )\n";
        os << "{\n";
        os << "    using CanonicalTypePathType = typename " << EG_TYPE_PATH_CANNON_TYPE << "< TypePath >::Type;\n";
        os << "    return " << EG_INVOKE_IMPL_TYPE << "< typename " << EG_RESULT_TYPE << "< " << EG_VARIANT_TYPE << 
                "< Ts... >, TypePath, Operation >::Type, " << EG_VARIANT_TYPE << 
                "< Ts... >, CanonicalTypePathType, Operation >()( *this, args... );\n";
        os << "}\n";
        os << "\n";
        
        std::vector< const concrete::Inheritance_Node* > iNodes = 
            many_cst< const concrete::Inheritance_Node >( objects );
            
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
                
        os << "\n";
        os << "\n";
    }
    
}