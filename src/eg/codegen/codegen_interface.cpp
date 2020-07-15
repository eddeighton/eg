

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

#include <boost/algorithm/string.hpp>

#include <sstream>
#include <cctype>


namespace eg
{
    
    
    std::string getStaticType( const interface::Element* pElement )
    {
        std::ostringstream os;
        std::vector< const ::eg::interface::Element* > path = 
            ::eg::interface::getPath( pElement );
        for( const interface::Element* pNodeIter : path )
        {
            if( pNodeIter != *path.begin())
                os << "::";
            os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
        }
        return os.str();
    }
    

    void generateForwardDeclarations( std::ostream& os, const Identifiers* pIdentifiers )
    {
        const Identifiers::IdentifierMap ids = pIdentifiers->getIdentifiersMap();
        for( Identifiers::IdentifierMap::const_iterator i = ids.begin(),
            iEnd = ids.end(); i!=iEnd; ++i )
        {
            const std::string& strName = i->first;
            if( !strName.empty() && ( strName != input::Root::RootTypeName ) )
            {
                const interface::Element* pElement = i->second;
                os << "struct [[clang::eg_type( -" << pElement->getIndex() << " )]]" << strName << ";\n";
            }
        }
    }
    
    void generateActionDefinition( std::ostream& os, const std::string& strIndent, int depth, bool bIsStateFree, 
        const interface::Context* pAction, const std::string& strName, 
        const input::Opaque* pParams, bool bIsIndirectlyAbstract, bool bHasOperation )
    {
        const std::string strActionInterfaceType = getInterfaceType( strName );

        //default constructor
        os << strIndent << strActionInterfaceType << "()\n";
        os << strIndent << "{\n";
        os << strIndent << "  data.instance = 0;\n";
        os << strIndent << "  data.type = 0;\n";
        os << strIndent << "  data.timestamp = " << EG_INVALID_TIMESTAMP << ";\n";
        os << strIndent << "}\n";

        //impl constructor
        os << strIndent << strActionInterfaceType << "( const " << EG_REFERENCE_TYPE << "& reference )\n";
        os << strIndent << "{\n";
        os << strIndent << "  data = reference;\n";
        os << strIndent << "}\n";

        //conversion constructor
        if( !bIsStateFree )
        {
        os << strIndent << "template< typename TFrom >\n";
        os << strIndent << strActionInterfaceType << "( const TFrom& from );\n";

        //assignment operator
        os << strIndent << "template< typename TFrom >\n";
        os << strIndent << strActionInterfaceType << "& operator=( const TFrom& from );\n";
        }
        
        //null
        os << strIndent << "static " << strActionInterfaceType << " null() { return " << strActionInterfaceType << "(); }\n";
        
        //invocation
        {
            os << strIndent << "template< typename TypePath, typename Operation, typename... Args >\n";
            os << strIndent << "typename " << EG_RESULT_TYPE << "< " <<
                getInterfaceInstantiationType( strName, depth ) <<
                ", TypePath, Operation >::Type " << EG_INVOKE_MEMBER_FUNCTION_NAME << "( Args... args ) const;\n";
        }

        //generate all exported member functions
        std::vector< interface::Export* > exports;
        pAction->getExports( exports );
        for( interface::Export* pExport : exports )
        {
            //os << strIndent << "template< typename... Args >\n";
            os << strIndent << "inline " << pExport->getReturnType() << " " << pExport->getIdentifier() << "( " << 
                pExport->getParameters() << " ) const;\n";
        }

        //void* operator
        os << strIndent << "operator const void*() const\n";
        os << strIndent << "{\n";
        os << strIndent << "      if( data.timestamp != " << EG_INVALID_TIMESTAMP << " )\n";
        os << strIndent << "      {\n";
        os << strIndent << "          return reinterpret_cast< const void* >( &data );\n";
        os << strIndent << "      }\n";
        os << strIndent << "      else\n";
        os << strIndent << "      {\n";
        os << strIndent << "          return nullptr;\n";
        os << strIndent << "      }\n";
        os << strIndent << "}\n";

        //active
        if( !bIsStateFree )
        {
        os << strIndent << "operator ::eg::ReferenceState() const;\n";
        }
        
        //ReferenceState comparisons
        if( !bIsStateFree )
        {
        os << strIndent << "bool operator==( ::eg::ReferenceState cmp ) const;\n";
        os << strIndent << "bool operator!=( ::eg::ReferenceState cmp ) const;\n";
        }
        
        //equality operator
        os << strIndent << "template< typename TComp >\n";
        os << strIndent << "bool operator==( const TComp& cmp ) const\n";
        os << strIndent << "{\n";
        os << strIndent << "    return data == cmp.data;\n";
        os << strIndent << "}\n";

        //in-equality operator
        os << strIndent << "template< typename TComp >\n";
        os << strIndent << "bool operator!=( const TComp& cmp ) const\n";
        os << strIndent << "{\n";
        os << strIndent << "    return !(data == cmp.data);\n";
        os << strIndent << "}\n";

        //comparison operator
        os << strIndent << "template< typename TComp >\n";
        os << strIndent << "bool operator<( const TComp& cmp ) const\n";
        os << strIndent << "{\n";
        os << strIndent << "    return data < cmp.data;\n";
        os << strIndent << "}\n";

        //operation
        if( const interface::Abstract* pContext = dynamic_cast< const interface::Abstract* >( pAction ) )
        {
        }
        else if( const interface::Event* pContext = dynamic_cast< const interface::Event* >( pAction ) )
        {
        }
        else if( const interface::Function* pContext = dynamic_cast< const interface::Function* >( pAction ) )
        {
            if( pParams )
                os << strIndent << pContext->getReturnType() << " operator()(" << pParams->getStr() << ") const;\n";
            else
                os << strIndent << pContext->getReturnType() << " operator()() const;\n";
        }
        else if( const interface::Action* pContext = dynamic_cast< const interface::Action* >( pAction ) )
        {
            if( bHasOperation )
            {
                if( pParams )
                {
                    os << strIndent << EG_RETURN_REASON_TYPE << " operator()(" << pParams->getStr() << ") const;\n";
                }
                else
                {
                    os << strIndent << EG_RETURN_REASON_TYPE << " operator()() const;\n";
                }
            }
        }
        else if( const interface::Object* pContext = dynamic_cast< const interface::Object* >( pAction ) )
        {
            if( bHasOperation )
            {
                if( pParams )
                {
                    os << strIndent << EG_RETURN_REASON_TYPE << " operator()(" << pParams->getStr() << ") const;\n";
                }
                else
                {
                    os << strIndent << EG_RETURN_REASON_TYPE << " operator()() const;\n";
                }
            }
        }
        else if( const interface::Link* pContext = dynamic_cast< const interface::Link* >( pAction ) )
        {
        }
        else
        {
            THROW_RTE( "Unknown abstract type" );
        }
        

        if( !bIsStateFree )
        {
        //iterator type
        os << strIndent << "using Iterator = " << EG_REFERENCE_ITERATOR_TYPE << "< " << strActionInterfaceType << " >;\n";

        //range type
        os << strIndent << "using EGRangeType = " << EG_RANGE_TYPE << "< Iterator >;\n";
        }
        
        //member variables
        os << strIndent << EG_REFERENCE_TYPE << " data;\n";
    }
    void generateActionTraits( std::ostream& os, const std::string& strIndent, int depth, const input::Context* pAction )
    {
        std::size_t szCounter = 0;
        for( const input::Opaque* pOpaque : pAction->getInheritance() )
        {
            os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth <<
                " >struct [[clang::eg_type(" << pOpaque->getIndex() << ")]]" <<
                getBaseTraitType( szCounter ) << ";\n";
            ++szCounter;
        }
    }

    struct InterfaceVisitor
    {
        std::ostream& os;
        std::string strIndent;
        int depth;

        InterfaceVisitor( std::ostream& os ) : os( os ), depth( 0 ) {}


        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {
            ++depth;
            os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth <<
                " >struct [[clang::eg_type(" << pNode->getIndex() << ")]]" << getInterfaceType( pElement->getIdentifier() ) << ";\n";
        }
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Using*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root* pElement, const interface::Element* pNode )
        {
            const interface::Context* pAction = dynamic_cast< const interface::Context* >( pNode );
            ++depth;
            const std::string& strName = pNode->getIdentifier();
            os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth <<
                " >struct [[clang::eg_type(" << pNode->getIndex() << ")]]" << getInterfaceType( strName ) << "\n";
            os << strIndent << "{\n";
            strIndent.push_back( ' ' );
            strIndent.push_back( ' ' );
            generateActionDefinition( os, strIndent, depth, false, pAction, 
                strName, pElement->getParams(), pAction->isIndirectlyAbstract(), pAction->hasDefinition() );
            generateActionTraits( os, strIndent, depth, pElement );
        }
        void push ( const input::Context* pElement, const interface::Element* pNode )
        {
            const interface::Context* pAction = dynamic_cast< const interface::Context* >( pNode );
            ++depth;
            
            if( const interface::Function* pContext = dynamic_cast< const interface::Function* >( pAction ) )
            {
                //forward declare
                os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth <<
                    " >struct [[clang::eg_type(" << pNode->getIndex() << ")]]" << getInterfaceType( pElement->getIdentifier() ) << ";\n";
            }
            else
            {
                os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth <<
                    " >struct [[clang::eg_type(" << pNode->getIndex() << ")]]" << getInterfaceType( pElement->getIdentifier() ) << "\n";
                os << strIndent << "{\n";
                
                strIndent.push_back( ' ' );
                strIndent.push_back( ' ' );
                generateActionDefinition( os, strIndent, depth, false, pAction, 
                    pElement->getIdentifier(), pElement->getParams(), pAction->isIndirectlyAbstract(), pAction->hasDefinition() );

                if( pElement->getSize() )
                {
                    os << strIndent << "static const " << EG_INSTANCE << " " << ::eg::EG_TRAITS_SIZE << " = " << pElement->getSize()->getStr() << ";\n";
                }
                generateActionTraits( os, strIndent, depth, pElement );
            }
        }
        void pop ( const input::Opaque* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {
            --depth;
        }
        void pop ( const input::Include* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Using* pElement, const interface::Element* pNode )
        {
            os << strIndent << "using " << pElement->getIdentifier() << " = " << pElement->getType()->getStr() << ";\n";
        }
        void pop ( const input::Export* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root* pElement, const interface::Element* pNode )
        {
            --depth;
            const std::string& strName = pNode->getIdentifier();
            strIndent.pop_back();
            strIndent.pop_back();
            os << strIndent << "};\n";
            if( strName != input::Root::RootTypeName )
                os << strIndent << "using " << strName << " = " <<
                    getInterfaceType( strName ) << "< " << EG_INTERFACE_PARAMETER_TYPE << depth << " >;\n";
            else
                os << strIndent << "using " << strName << " = " <<
                    getInterfaceType( strName ) << "< void >;\n";
        }
        void pop ( const input::Context* pElement, const interface::Element* pNode )
        {
            --depth;
            if( const interface::Function* pContext = dynamic_cast< const interface::Function* >( pNode ) )
            {
                //do nothing
            }
            else
            {
                strIndent.pop_back();
                strIndent.pop_back();
                os << strIndent << "};\n";
                os << strIndent << "using " << pElement->getIdentifier() << " = " <<
                    getInterfaceType( pElement->getIdentifier() ) << "< " << EG_INTERFACE_PARAMETER_TYPE << depth << " >;\n";
            }
        }
    };

    struct DelayedInterfaceVisitor
    {
        std::ostream& os;
        std::string strIndent = "  ";
        int depth;

        DelayedInterfaceVisitor( std::ostream& os ) : os( os ), depth( 0 ) {}

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
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root* pElement, const interface::Element* pNode )
        {
            ++depth;
        }
        void push ( const input::Context* pElement, const interface::Element* pNode )
        {
            const interface::Context* pAction = dynamic_cast< const interface::Context* >( pNode );
            ++depth;
            
            if( const interface::Function* pFunction = dynamic_cast< const interface::Function* >( pAction ) )
            {
                std::vector< const interface::Element* > path = getPath( pNode );
                
                {
                    int depth = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        os << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth++ << " >\n";
                    }
                }
                
                os << "struct [[clang::eg_type(" << pNode->getIndex() << ")]]";
                {
                    int depth = 1;
                    for( const interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            os << "::";
                        if( depth == path.size() )
                            os << getInterfaceType( pNodeIter->getIdentifier() );
                        else
                            os << getInterfaceType( pNodeIter->getIdentifier() ) << "< " << EG_INTERFACE_PARAMETER_TYPE << depth++ << " >";
                    }
                }
                
                os << "\n{\n";
                
                generateActionDefinition( os, strIndent, depth, true, pAction, 
                    pElement->getIdentifier(), pElement->getParams(), 
                    pAction->isIndirectlyAbstract(), pAction->hasDefinition() );

                generateActionTraits( os, strIndent, depth, pElement );
                
                os << "\n};\n";
            }
        }
        void pop ( const input::Opaque* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Include* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Using* pElement, const interface::Element* pNode )
        {
            os << strIndent << "using " << pElement->getIdentifier() << " = " << pElement->getType()->getStr() << ";\n";
        }
        void pop ( const input::Export* pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root* pElement, const interface::Element* pNode )
        {
            --depth;
        }
        void pop ( const input::Context* pElement, const interface::Element* pNode )
        {
            --depth;
        }
    };


    struct ExplicitInstantiationVisitor
    {
        std::ostream& os;

        ExplicitInstantiationVisitor( std::ostream& os ) : os( os ) {}

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
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {
            push( (input::Context*) pElement, pNode );
        }
        void push ( const input::Context*    pElement, const interface::Element* pNode )
        {
            if( const interface::Function* pContext = dynamic_cast< const interface::Function* >( pNode ) )
            {
            }
            else
            {
                //calculate the path to the root type
                std::vector< const interface::Element* > path = getPath( pNode );
                //generate type explicit template specialisation
                {
                    os << "template struct ";
                    for( const interface::Element* pNodeIter : path )
                    {
                        if( pNodeIter != *path.begin())
                            os << "::";
                        os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
                    }
                    os << ";\n";
                }
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
        void pop ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Context*    pElement, const interface::Element* pNode )
        {
        }
    };


    struct ExplicitTraitInstantiationVisitor
    {
        std::ostream& os;
        int depth;
        ExplicitTraitInstantiationVisitor( std::ostream& os ) : os( os ), depth( 0 ) {}

        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {
        }

        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {
            std::vector< const interface::Element* > path = getPath( pNode );
            //generate dimension trait explicit template specialisation

            for( const interface::Element* pNodeIter : path )
                os << "template<>\n";

            os << "struct ";
            for( const interface::Element* pNodeIter : path )
            {
                if( pNodeIter != *path.begin())
                    os << "::";
                os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
            }
            os << "\n{\n";
            os << "  using Type  = " << pElement->getType()->getStr() << ";\n";
            os << "  using Read  = " << EG_DIMENSION_TRAITS << "< " << pElement->getType()->getStr() << " >::Read;\n";
            os << "  using Write = " << EG_DIMENSION_TRAITS << "< " << pElement->getType()->getStr() << " >::Write;\n";
            os << "  using Get   = " << EG_DIMENSION_TRAITS << "< " << pElement->getType()->getStr() << " >::Get;\n";
            os << "  static const " << EG_INSTANCE << " " << EG_TRAITS_SIZE << " = " << EG_DIMENSION_TRAITS << "< " << pElement->getType()->getStr() << " >::" << EG_TRAITS_SIZE << ";\n";
            os << "  static const " << EG_INSTANCE << " " << EG_TRAITS_SIMPLE << " = " << EG_DIMENSION_TRAITS << "< " << pElement->getType()->getStr() << " >::" << EG_TRAITS_SIMPLE << ";\n";
            os << "};\n";
        }
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Using*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {
            ++depth;
        }
        void push ( const input::Context*    pElement, const interface::Element* pNode )
        {
            ++depth;
            std::vector< const interface::Element* > path = getPath( pNode );
            
            const interface::Context* pAction = dynamic_cast< const interface::Context* >( pNode );
            
            std::size_t szCounter = 0;
            for( const input::Opaque* pOpaque : pElement->getInheritance() )
            {
                for( const interface::Element* pNodeIter : path )
                    os << "template<>\n";
                os << "template<>\n";

                os << "struct ";
                for( const interface::Element* pNodeIter : path )
                {
                    if( pNodeIter != *path.begin())
                        os << "::";
                    os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >";
                }

                os << "::" << getBaseTraitType( szCounter ) << "< void >";
                ++szCounter;

                os << "\n{\n";
                os << "  using Type  = " << pOpaque->getStr() << ";\n";
                os << "};\n";
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
        void pop ( const input::Export*   pElement, const interface::Element* pNode )
        {
        }
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {
            --depth;
        }
        void pop ( const input::Context*    pElement, const interface::Element* pNode )
        {
            --depth;
        }
    };

    void generateInterface( std::ostream& os, const interface::Root* pRoot, const Identifiers* pIdentifiers )
    {
        generateIncludeGuard( os, "INTERFACE" );

        generateForwardDeclarations( os, pIdentifiers );

        {
            os << "\n//EG Interface\n";
            InterfaceVisitor interfaceVisitor( os );
            pRoot->pushpop( interfaceVisitor );
            os << "\n";
        }
        
        {
            os << "\n//Delayed EG Interface\n";
            DelayedInterfaceVisitor delayedInterfaceVisitor( os );
            pRoot->pushpop( delayedInterfaceVisitor );
            os << "\n";
        }

        {
            os << "\n//Explicit Template Instantiations\n";
            ExplicitInstantiationVisitor visitor( os );
            pRoot->pushpop( visitor );
            os << "\n";
        }

        {
            os << "\n//Explicit Trait Template Instantiations\n";
            ExplicitTraitInstantiationVisitor visitor( os );
            pRoot->pushpop( visitor );
            os << "\n";
        }

        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }

}