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


#include "eg_compiler/codegen/codegen.hpp"

#include "eg_compiler/eg.hpp"
#include "eg_compiler/input.hpp"
#include "eg_compiler/interface.hpp"
#include "eg_compiler/concrete.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/layout.hpp"
#include "eg_compiler/identifiers.hpp"
#include "eg_compiler/derivation.hpp"
#include "eg_compiler/translation_unit.hpp"
#include "eg_compiler/invocation.hpp"

#include <boost/bind.hpp>
#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>

namespace eg
{
    
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
                    case eAbstractExport    :
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
        
        
        os << "    inline "; printReturnType( os, objects, invocation ); os << " operator()( "; printContextType( os, objects, invocation ); os << " context"; 
        
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
    
    void generateImplementationSource( std::ostream& os, 
        const ImplementationSession& program, 
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
        //should this be inline?
        os << "    inline ResultType operator()( ContextType, Args... )\n";
        //os << "    ResultType operator()( ContextType, Args... )\n";
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
        
        
        std::vector< const concrete::Inheritance_Node* > iNodes = 
            many_cst< const concrete::Inheritance_Node >( objects );
        
        generateGenerics( os, program, actions, iNodes, translationUnit );
                
        os << "\n";
        os << "\n";
    }
}