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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

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
        
    static const char* pszLine = 
    "//////////////////////////////////////////////////////////////////////////////////\n";
    
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

    static const char DELIM = '_';
    std::string getUUIDString()
    {
        boost::uuids::random_generator gen;
        std::string strUUID = boost::to_upper_copy( boost::uuids::to_string( gen() ) );
        return style_replace_non_alpha_numeric( strUUID, DELIM );
    }
    std::string getDateAsNiceString()
    {
        return style_replace_non_alpha_numeric(
            boost::posix_time::to_simple_string(
                boost::posix_time::second_clock::universal_time() ), DELIM );
    }

    void generateIncludeGuard( std::ostream& os )
    {
        const std::string strGUIID = getUUIDString();
        const std::string strDate = getDateAsNiceString();
        
        os << "#ifndef EG_INTERFACE_GUARD_" << strGUIID << "__" << strDate << "\n";
        os << "#define EG_INTERFACE_GUARD_" << strGUIID << "__" << strDate << "\n";
        os << pszLine << pszLine << "\n";
    }
    
    
    //generate include directives
    void generateInterfaceIncludes( std::ostream& os, const interface::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser )
    {
        interface::Element::Collector< input::Include > includes;
        pRoot->visit( includes );
        std::set< boost::filesystem::path > systemIncludes, userIncludes;
        std::vector< boost::filesystem::path > systemIncludesOrdered, userIncludesOrdered;
        
        //always include <cstddef>
        //systemIncludes.insert( "<cstddef>" );
        //systemIncludesOrdered.push_back( "<cstddef>" );
        
        //always include eg.hpp
        userIncludes.insert( "eg.hpp" );
        userIncludesOrdered.push_back( "eg.hpp" );
        
        for( const boost::filesystem::path& hostIncludePath : hostIncludesUser )
        {
            if( userIncludes.find( hostIncludePath ) == userIncludes.end() )
            {
                userIncludes.insert( hostIncludePath );
                userIncludesOrdered.push_back( hostIncludePath );
            }
        }
        for( const boost::filesystem::path& hostIncludePath : hostIncludesSystem )
        {
            if( systemIncludes.find( hostIncludePath ) == systemIncludes.end() )
            {
                systemIncludes.insert( hostIncludePath );
                systemIncludesOrdered.push_back( hostIncludePath );
            }
        }
        
        for( const input::Include* pInclude : includes() )
        {
            const boost::filesystem::path& p = pInclude->getIncludeFilePath();
            if( pInclude->isSystemInclude() )
            {
                if( systemIncludes.find( p ) == systemIncludes.end() )
                {
                    systemIncludes.insert( p );
                    systemIncludesOrdered.push_back( p );
                }
            }
            else
            {
                if( userIncludes.find( p ) == userIncludes.end() )
                {
                    userIncludes.insert( p );
                    userIncludesOrdered.push_back( p );
                }
            }
        }
        
        for( const boost::filesystem::path& p : systemIncludesOrdered )
        {
            os << "#include " << p.string() << "\n";
        }
        for( const boost::filesystem::path& p : userIncludesOrdered )
        {
            os << "#include \"" << p.string() << "\"\n";
        }
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
    
    struct InterfaceVisitor
    {
        std::ostream& os;
        std::string strIndent;
        int depth;
        
        InterfaceVisitor( std::ostream& os ) : os( os ), depth( 0 ) {}
        
        void addActionInterface( std::ostream& os, const std::string& strName, bool bIsIndirectlyAbstract )
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
            os << strIndent << "template< typename TFrom >\n";
            os << strIndent << strActionInterfaceType << "( const TFrom& from );\n";
            
            //assignment operator
            os << strIndent << "template< typename TFrom >\n";
            os << strIndent << strActionInterfaceType << "& operator=( const TFrom& from );\n";
            
            //invocation
            os << strIndent << "template< typename TypePath, typename Operation, typename... Args >\n";
            os << strIndent << "typename " << EG_RESULT_TYPE << "< " << 
                getInterfaceInstantiationType( strName, depth ) <<
                ", TypePath, Operation >::Type " << EG_INVOKE_MEMBER_FUNCTION_NAME << "( Args... args );\n";
            
            //event access
            os << strIndent << EG_EVENT_TYPE << " get_next_event() const;\n";
            
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
            os << strIndent << EG_COROUTINE_TYPE << " operator()();\n";
            
            //iterator type
            os << strIndent << "using Iterator = " << EG_REFERENCE_ITERATOR_TYPE << "< " << strActionInterfaceType << " >;\n";
            
            //range type
            os << strIndent << "using EGRangeType = " << EG_RANGE_TYPE << "< Iterator >;\n";
            
            //member variables
            os << strIndent << EG_REFERENCE_TYPE << " data;\n";
        }
        void addActionTraits( std::ostream& os, const input::Action* pAction )
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
        void push ( const input::Root* pElement, const interface::Element* pNode )
        {    
            ++depth;
            const std::string& strName = pNode->getIdentifier();
            os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth << 
                " >struct [[clang::eg_type(" << pNode->getIndex() << ")]]" << getInterfaceType( strName ) << "\n";
            os << strIndent << "{\n";
            strIndent.push_back( ' ' );
            strIndent.push_back( ' ' );
            addActionInterface( os, strName, dynamic_cast< const interface::Action* >( pNode )->isIndirectlyAbstract() );
            addActionTraits( os, pElement );
        }    
        void push ( const input::Action* pElement, const interface::Element* pNode )
        {
            ++depth;
            os << strIndent << "template< typename " << EG_INTERFACE_PARAMETER_TYPE << depth <<
                " >struct [[clang::eg_type(" << pNode->getIndex() << ")]]" << getInterfaceType( pElement->getIdentifier() ) << "\n";
            os << strIndent << "{\n";
            strIndent.push_back( ' ' );
            strIndent.push_back( ' ' );
            addActionInterface( os, pElement->getIdentifier(), dynamic_cast< const interface::Action* >( pNode )->isIndirectlyAbstract() );
            
            if( pElement->getSize() )
            {
                os << strIndent << "static const " << EG_INSTANCE << " SIZE = " << pElement->getSize()->getStr() << ";\n";
            }
            addActionTraits( os, pElement );
        }
        void pop ( const input::Opaque* pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
            --depth;
            //os << strIndent << "using " << pElement->getIdentifier() << " = " << 
            //    getInterfaceType( pElement->getIdentifier() ) << "< " << EG_INTERFACE_PARAMETER_TYPE << depth << " >;\n";
        }    
        void pop ( const input::Include* pElement, const interface::Element* pNode )
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
        void pop ( const input::Action* pElement, const interface::Element* pNode )
        {
            --depth;
            strIndent.pop_back();
            strIndent.pop_back();
            os << strIndent << "};\n";
            os << strIndent << "using " << pElement->getIdentifier() << " = " << 
                getInterfaceType( pElement->getIdentifier() ) << "< " << EG_INTERFACE_PARAMETER_TYPE << depth << " >;\n";
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
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {    
            push( (input::Action*) pElement, pNode );
        }    
        void push ( const input::Action*    pElement, const interface::Element* pNode )
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
        void pop ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }      
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };

    
    struct ExplicitTraitInstantiationVisitor
    {
        std::ostream& os;
        
        ExplicitTraitInstantiationVisitor( std::ostream& os ) : os( os ) {}
                
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
            os << "  static const " << EG_INSTANCE << " Size = " << EG_DIMENSION_TRAITS << "< " << pElement->getType()->getStr() << " >::Size;\n";
            os << "};\n";
        }    
        void push ( const input::Include*   pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Root*      pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Action*    pElement, const interface::Element* pNode )
        {
            std::vector< const interface::Element* > path = getPath( pNode );
            
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
                os << "  using Dependency = " << EG_OBJECT_TRAITS << "< " << pOpaque->getStr() << " >::Dependency;\n";
                os << "  static const " << EG_INSTANCE << " Size = " << EG_OBJECT_TRAITS << "< " << pOpaque->getStr() << " >::Size;\n";
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
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };
    
    void generateIncludeHeader( std::ostream& os, const interface::Root* pRoot, 
        const std::vector< boost::filesystem::path >& hostIncludesSystem, 
        const std::vector< boost::filesystem::path >& hostIncludesUser )
    {
        generateIncludeGuard( os );
        
        generateInterfaceIncludes( os, pRoot, hostIncludesSystem, hostIncludesUser );
        
        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }
    
    void generateInterface( std::ostream& os, const interface::Root* pRoot, const Identifiers* pIdentifiers )
    {
        generateIncludeGuard( os );
        
        generateForwardDeclarations( os, pIdentifiers );
        
        {
            os << "\n//EG Interface\n";
            InterfaceVisitor interfaceVisitor( os );
            pRoot->pushpop( interfaceVisitor );
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
    
    struct OperationsSourceVisitor
    {
        std::ostream& os;
        std::string strIndent;
        
        OperationsSourceVisitor( std::ostream& os ) : os( os ) {}
        
        
        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }      
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Include*   pElement, const interface::Element* pNode )
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
            {
                int iCounter = 1;
                for( const interface::Element* pNodeIter : path )
                {
                    os << strIndent << "template<>\n";
                    ++iCounter;
                }
            }
            
            //just generate an explicit template specialisation
            os << strIndent << EG_COROUTINE_TYPE << " ";
            {
                for( const interface::Element* pNodeIter : path )
                {
                    os << getInterfaceType( pNodeIter->getIdentifier() ) << "< void >::";
                }
                os << "operator()()\n";
            }
            
            //generate the function body
            os << strIndent << "{\n";
            strIndent.push_back( ' ' );
            strIndent.push_back( ' ' );
            
            for( const interface::Element* pChild : pNode->getChildren() )
            {
                if( const input::Opaque* pOpaque = 
                        dynamic_cast< const input::Opaque* >( pChild->getInputElement() ) )
                {
                    os << strIndent << pOpaque->getStr() << "\n";
                }
            }
            
            os << "\n" << strIndent << "co_return;\n";
            strIndent.pop_back();
            strIndent.pop_back();
            os << strIndent << "}\n";
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
        void pop ( const input::Root*      pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Action*    pElement, const interface::Element* pNode )
        {
        }
    };
    
    void generateOperationSource( std::ostream& os, const interface::Root* pRoot )
    {
        generateIncludeGuard( os );
        
        //generate operations
        {
            OperationsSourceVisitor visitor( os );
            pRoot->pushpop( visitor );
        }
        
        os << "\n" << pszLine << pszLine;
        os << "#endif\n";
    }
    
    
    void generateBufferStructures( std::ostream& os, const ImplementationSession& program )
    {
        const Layout& layout = program.getLayout();
        
        generateIncludeGuard( os );
        
        os << "//data structures\n";
        for( const Buffer* pBuffer : layout.getBuffers() )
        {
            os << "\n//Buffer: " << pBuffer->getTypeName() << /*" stride: " << pBuffer->getStride() <<*/ " size: " << pBuffer->getSize() << "\n";
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
            //os << "static " << pBuffer->getTypeName() << " *" << pBuffer->getVariableName() << ";\n";
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
            
            //get_next_event
            os << osTemplateArgLists.str();
            os << EG_EVENT_TYPE << " " << osTypeName.str() << "::get_next_event() const\n";
            os << "{\n";
            os << "    " << EG_EVENT_TYPE << " ev;\n";
            os << "    " << EG_EVENT_LOG_EVENT_TYPE << " _event;\n";
            os << "    " << EG_EVENT_ITERATOR << "* pIterator = nullptr;\n";
            
            if( dynamicCompatibleTypes.size() > 1 )
            {
            os << "  switch( data.type )\n";
            os << "  {\n";
            for( const concrete::Action* pCompatible : dynamicCompatibleTypes )
            {
                const DataMember* pEventIterator = layout.getDataMember( pCompatible->getEventIterator() );
            os << "      case " << pCompatible->getIndex() << ": //" << pCompatible->getFriendlyName() << "\n";
            os << "         pIterator = &" << Printer( pEventIterator, "data.instance" ) << ";\n";
            os << "         break;\n";
            }
            os << "     default:\n";
            os << "         break;\n";
            os << "  }\n";
            }
            else if( dynamicCompatibleTypes.size() == 1 )
            {
                const concrete::Action* pCompatible = *dynamicCompatibleTypes.begin();
                const DataMember* pEventIterator = layout.getDataMember( pCompatible->getEventIterator() );
            os << "    pIterator = &" << Printer( pEventIterator, "data.instance" ) << ";\n";
            }
            if( !dynamicCompatibleTypes.empty() )
            {
            os << "    if( pIterator )\n";
            os << "    {\n";
            os << "         while( g_eg_event_log->GetEvent( *pIterator, _event ) )\n";
            os << "         {\n";   
            os << "              if( 0 == strcmp( _event.type, \"stop\" ) )\n";
            os << "              {\n";
            os << "                  ev.data = *reinterpret_cast< const " << EG_REFERENCE_TYPE << "* >( _event.value );\n";
            os << "                  break;\n";
            os << "              }\n"; 
            os << "         }\n";
            os << "    }\n";
            }
            os << "    return ev;\n";
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
                const DataMember* pRunning = layout.getDataMember( pCompatible->getRunningTimestamp() );
            os << "      case " << pCompatible->getIndex() << ": //" << pCompatible->getFriendlyName() << "\n";
            os << "         return " << Printer( pRunning, "instance" ) << ";\n";
            }
            os << "      default: return " << EG_INVALID_TIMESTAMP << ";\n";
            os << "    }\n";
            }
            else //if( dynamicCompatibleTypes.size() == 1 )
            {
                const concrete::Action* pCompatible = *dynamicCompatibleTypes.begin();
                const DataMember* pRunning = layout.getDataMember( pCompatible->getRunningTimestamp() );
            os << "    return " << Printer( pRunning, "instance" ) << ";\n";
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
        std::string strIndent;
        
        InvokeVisitor( std::ostream& os ) : os( os ) {}
        
        
        void push ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }      
        void push ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void push ( const input::Include*   pElement, const interface::Element* pNode )
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
            os << osTypeName.str() << "::" << EG_INVOKE_MEMBER_FUNCTION_NAME << "( Args... args )\n";
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
        void pop ( const input::Opaque*    pElement, const interface::Element* pNode )
        {    
        }     
        void pop ( const input::Dimension* pElement, const interface::Element* pNode )
        {    
        }    
        void pop ( const input::Include*   pElement, const interface::Element* pNode )
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
                if( invocation.isImplicitStarter() )
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
            case id_Stop       : 
            case id_Pause      : 
            case id_Resume     : 
                os << "void";
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
                            returnTypes.front()->getStaticType() << ", " << invocation.getRoot()->getMaxRanges() << "U > >";
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
                            osType.str() << ", " << invocation.getRoot()->getMaxRanges() << "U > >";
                    }
                }
                break;
            default:
                THROW_RTE( "Unknown operation type" );
                break;
        }
    }
    
    void printParameterTypes( std::ostream& os, const IndexedObject::Array& objects, const InvocationSolution& invocation )
    {
        const InvocationSolution::Context& returnTypes = invocation.getReturnTypes();
        switch( invocation.getOperation() )
        {
            case id_Imp_NoParams   :
            case id_Imp_Params  :
                if( invocation.isImplicitStarter() )
                {
                }
                else if( invocation.getOperation() == id_Imp_NoParams )
                {
                }
                else if( invocation.getOperation() == id_Imp_Params )
                {
                    os << returnTypes.front()->getStaticType() << "::Write";
                }
                break;
            case id_Get        :
            case id_Stop       : 
            case id_Pause      : 
            case id_Resume     : 
            case id_Range      : 
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
                if( invocation.isImplicitStarter() )
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
            case id_Get        :
            case id_Stop       : 
            case id_Pause      : 
            case id_Resume     : 
            case id_Range      : 
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
                ASSERT( id < objects.size() );
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
    
    inline std::ostream& operator<<( std::ostream& os, const Printer& printer )
    {
        printer.m_pDataMember->printVariableAccess( os, printer.pszIndex );;
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
        os << "    "; printReturnType( os, objects, invocation ); os << " operator()( "; printContextType( os, objects, invocation ); os << " context"; 
        
        //invocation parameters
        switch( invocation.getOperation() )
        {
            case id_Imp_NoParams   :
            case id_Imp_Params  :
                if( invocation.isImplicitStarter() )
                {
                    os << " )\n";
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
            case id_Get                  :  os << " )\n";   break;
            case id_Stop                 :  os << " )\n";   break;
            case id_Pause                :  os << " )\n";   break;
            case id_Resume               :  os << " )\n";   break;
            case id_Range                :  os << " )\n";   break;
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
        //os << "bool " << pAction->getName() << "_executor();\n";
    }
    
    void generateActionInstanceFunctions( std::ostream& os, const Layout& layout, const concrete::Action* pAction )
    {
        if( pAction->getRunningTimestamp() && pAction->getPauseTimestamp() && pAction->getCoroutine() )
        {
            if( pAction->getParent() && pAction->getParent()->getParent() )
            {
        
        /////starter
        pAction->printType( os ); os << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        os << "    //claim next free index\n";
        
                const concrete::Action* pParentAction = dynamic_cast< const concrete::Action* >( pAction->getParent() );
                VERIFY_RTE( pParentAction );
                
                const DataMember* pIteratorData = layout.getDataMember( pParentAction->getIterator( pAction ) );
                const DataMember* pAllocatorData = layout.getDataMember( pAction->getAllocatorData() );
                const DataMember* pRunningData = layout.getDataMember( pAction->getRunningTimestamp() );
                const DataMember* pPausedData = layout.getDataMember( pAction->getPauseTimestamp() );
                const DataMember* pCoroutineData = layout.getDataMember( pAction->getCoroutine() );
                const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
                const DataMember* pEventIterator = layout.getDataMember( pAction->getEventIterator() );
                const DataMember* pStoppedTimestamp = layout.getDataMember( pAction->getStopTimestamp() );
                const DataMember* pObject = pAction->getMappedObject() ? 
                    layout.getDataMember( pAction->getMappedObject() ) : nullptr;
                
                
        os << "    " << EG_ITERATOR_TYPE << " iter, expected;\n";
        os << "    while( true )\n";
        os << "    {\n";
        os << "         iter = " << EG_ITERATOR_TYPE << "( " << Printer( pIteratorData, "_gid" ) << ".load() );\n";
        os << "         if( iter.protection )\n";
        os << "             continue;\n";
        os << "         else if( iter.full )\n";
        os << "             break;\n";
        os << "         expected = iter;\n";
        os << "         " << EG_INSTANCE << " nextCellIndex = static_cast< " << EG_INSTANCE << " >( iter.head );\n";
        os << "         //claim the next free index\n";
        os << "         if( nextCellIndex == " << pAction->getLocalDomainSize() - 1U << " )\n";
        os << "         {\n";
        os << "             iter.head = 0U;\n";
        os << "         }\n";
        os << "         else\n";
        os << "         {\n";
        os << "             ++iter.head;\n";
        os << "         }\n";
        os << "         if( static_cast< " << EG_INSTANCE << " >( iter.head ) == static_cast< " << EG_INSTANCE << " >( iter.tail ) )\n";
        os << "         {\n";
        os << "             iter.full = 1U;\n";
        os << "         }\n";
        
        //need to calculate the index based on local domain size
        std::ostringstream osNextIndex;
        osNextIndex << "_gid * " << pAction->getLocalDomainSize() << " + nextCellIndex";
        os << "         " << EG_INSTANCE << " nextInstance = " << Printer( pAllocatorData, osNextIndex.str().c_str() ) << ";\n";
        os << "         if( " << Printer( pStoppedTimestamp, "nextInstance" ) << " <= clock::subcycle() )\n";
        os << "         {\n";
        os << "             //attempt to set the atomic\n";
        os << "             if( " << Printer( pIteratorData, "_gid" ) << ".compare_exchange_weak( expected.data, iter.data ) )\n";
        os << "             {\n";
        os << "                 //successfully claimed index so get the actual instance from the ring buffer\n";
        os << "                 const " << EG_INSTANCE << " startSubCycle = clock::subcycle() + 1;\n";
        os << "                 "; pAction->printType( os ); os << "& reference = " << 
                                Printer( pReferenceData, "nextInstance" ) << ";\n";
        os << "                 reference.data.timestamp = startSubCycle;\n";
        os << "                 " << Printer( pRunningData, "nextInstance" ) << " = startSubCycle;\n";
        os << "                 " << Printer( pPausedData, "nextInstance" ) << " = startSubCycle;\n";
        os << "                 " << EG_EVENT_LOG_EVENT_TYPE << " ev = { \"start\", startSubCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) };\n";
        os << "                 g_eg_event_log->PutEvent( ev );\n";
        os << "                 " << Printer( pEventIterator, "nextInstance" ) << " = g_eg_event_log->GetEventIterator();\n";
                //if there is an object mapping then start it
                if( pObject )
                {
                    pObject->printStart( os, "nextInstance" );
                }
        os << "                 " << Printer( pCoroutineData, "nextInstance" ) << " = reference();\n";
        os << "                 return reference;\n";
        os << "             }\n";
        os << "         }\n";
        os << "         else\n";
        os << "         {\n";
        os << "             break;\n";
        os << "         }\n";
        os << "    }\n";   
        os << "    //failure return null handle\n";
        os << "    "; pAction->printType( os ); os << " nullInstance;\n";
        os << "    return nullInstance;\n";
        os << "}\n";
        os << "\n";
        
            }
            else
            {
                //simple starter for root
                const DataMember* pRunningData = layout.getDataMember( pAction->getRunningTimestamp() );
                const DataMember* pPausedData = layout.getDataMember( pAction->getPauseTimestamp()   );
                const DataMember* pCoroutineData = layout.getDataMember( pAction->getCoroutine() );
                const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
                const DataMember* pEventIterator = layout.getDataMember( pAction->getEventIterator() );
                        
        pAction->printType( os ); os << " " << pAction->getName() << "_starter( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        os << "    const " << EG_INSTANCE << " startSubCycle = clock::subcycle();\n";
        os << "    "; pAction->printType( os ); os << "& reference = " << Printer( pReferenceData, "0" ) << ";\n";
        os << "    reference.data.timestamp = startSubCycle;\n";
        os << "    " << Printer( pRunningData, "0" ) << " = startSubCycle;\n";
        os << "    " << Printer( pPausedData, "0" ) << " = startSubCycle;\n";
        os << "    " << EG_EVENT_LOG_EVENT_TYPE << " ev = { \"start\", startSubCycle, &reference.data, sizeof( " << EG_REFERENCE_TYPE << " ) };\n";
        os << "    g_eg_event_log->PutEvent( ev );\n";
        os << "    " << Printer( pEventIterator, "0" ) << " = g_eg_event_log->GetEventIterator();\n";
        os << "    " << Printer( pCoroutineData, "0" ) << " = reference();\n";
        os << "    return reference;\n";
        os << "}\n";
        os << "\n";
                
            }
        
        ////stopper
        os << "void " << pAction->getName() << "_stopper( " << EG_INSTANCE << " _gid )\n";
        os << "{\n";
        os << "     " << EG_INSTANCE << " _parent_id = _gid / " << pAction->getLocalDomainSize() << ";\n";
        
            if( pAction->getParent() && pAction->getParent()->getParent() )
            {
                
                const concrete::Action* pParentAction = dynamic_cast< const concrete::Action* >( pAction->getParent() );
                VERIFY_RTE( pParentAction );
                
                const DataMember* pIteratorData = layout.getDataMember( pParentAction->getIterator( pAction ) );
                const DataMember* pAllocatorData = layout.getDataMember( pAction->getAllocatorData() );
                const DataMember* pRunningData = layout.getDataMember( pAction->getRunningTimestamp() );
                const DataMember* pPausedData = layout.getDataMember( pAction->getPauseTimestamp() );
                const DataMember* pCoroutineData = layout.getDataMember( pAction->getCoroutine() );
                const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
                const DataMember* pStoppedTimestamp = layout.getDataMember( pAction->getStopTimestamp() );
                const DataMember* pObject = pAction->getMappedObject() ? 
                    layout.getDataMember( pAction->getMappedObject() ) : nullptr;
                
        os << "     if( " << pRunningData->getBuffer()->getVariableName() << "[ _gid ]." << 
            pRunningData->getName() << " != " << EG_INVALID_TIMESTAMP << " )\n";
        os << "     {\n";
                
        os << "         " << EG_ITERATOR_TYPE << " iter, expected;\n";
        os << "         while( true )\n";
        os << "         {\n";
        os << "              iter = " << EG_ITERATOR_TYPE << "( " << Printer( pIteratorData, "_parent_id" ) << ".load() );\n";
        os << "              if( iter.protection )\n";
        os << "                  break;\n";
        os << "              expected = iter;\n";
        os << "              " << EG_INSTANCE << " freeCellIndex = static_cast< " << EG_INSTANCE << " >( iter.tail );\n";
        os << "              //if buffer is full then set the protection bit while freeing\n";
        os << "              if( iter.full )\n";
        os << "              {\n";
        os << "                  iter.protection = 1U;\n";
        os << "                  iter.full = 0U;\n";
        os << "              }\n";
        os << "              //claim the index to store free instance\n";
        os << "              if( static_cast< " << EG_INSTANCE << " >( iter.tail ) == " << pAction->getLocalDomainSize() << " - 1U )\n";
        os << "              {\n";
        os << "                  iter.tail = 0U;\n";
        os << "              }\n";
        os << "              else\n";
        os << "              {\n";
        os << "                  ++iter.tail;\n";
        os << "              }\n";
        os << "              //attempt to set the atomic\n";
        os << "              if( " << Printer( pIteratorData, "_parent_id" ) << ".compare_exchange_weak( expected.data, iter.data ) )\n";
        os << "              {\n";
        os << "                  //successfully freed index\n";
        os << "                  " << Printer( pAllocatorData, "freeCellIndex" ) << " = _gid;\n";
        os << "                  if( iter.protection )\n";
        os << "                  {\n";
        os << "                      //turn off the protection bit\n";
        os << "                      expected = iter;\n";
        os << "                      iter.protection = 0;\n";
        os << "                      while( " << Printer( pIteratorData, "_parent_id" ) << ".compare_exchange_weak( expected.data, iter.data ) );\n";
        os << "                  }\n";
        os << "                  break;\n";
        os << "              }\n";
        os << "         }\n";   
                        
                        
                        //stop the subtree
                for( const concrete::Element* pChild : pAction->getChildren() )
                {
                    if( const concrete::Action* pChildAction = dynamic_cast< const concrete::Action* >( pChild ) )
                    {
        os << "         for( " << EG_INSTANCE << " childIndex = _gid * " << pChildAction->getLocalDomainSize() << 
                                "; childIndex != ( _gid + 1 ) * " << pChildAction->getLocalDomainSize() << "; ++childIndex )\n";
        os << "         {\n";
        os << "             " << pChildAction->getName() << "_stopper( childIndex );\n";
        os << "         }\n";
                    }
                }
        
        os << "         " << Printer( pRunningData, "_gid" ) << " = " << EG_INVALID_TIMESTAMP << ";\n";
        os << "         " << Printer( pPausedData, "_gid" ) << " = " << EG_INVALID_TIMESTAMP << ";\n";
        os << "         " << Printer( pStoppedTimestamp, "_gid" ) << " = clock::subcycle() + 2U;\n";
        os << "         " << Printer( pCoroutineData, "_gid" ) << ".destroy();\n";
        os << "         " << EG_EVENT_LOG_EVENT_TYPE << " ev = { \"stop\", clock::subcycle(), &" << Printer( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) };\n";
        os << "         g_eg_event_log->PutEvent( ev );\n";
                //if there is an object mapping then start it
                if( pObject )
                {
                    pObject->printStop( os, "_gid" );
                    
                }
        os << "     }\n";
            }
            else
            {
                const DataMember* pRunningData = layout.getDataMember( pAction->getRunningTimestamp() );
                const DataMember* pPausedData = layout.getDataMember( pAction->getPauseTimestamp()   );
                const DataMember* pCoroutineData = layout.getDataMember( pAction->getCoroutine() );
                const DataMember* pReferenceData = layout.getDataMember( pAction->getReference() );
                
        os << "     if( " << Printer( pRunningData, "_gid" ) << " != " << EG_INVALID_TIMESTAMP << " )\n";
        os << "     {\n";
                        //stop the subtree
                for( const concrete::Element* pChild : pAction->getChildren() )
                {
                    if( const concrete::Action* pChildAction = dynamic_cast< const concrete::Action* >( pChild ) )
                    {
        os << "         for( " << EG_INSTANCE << " childIndex = _gid * " << pChildAction->getLocalDomainSize() << 
                                "; childIndex != ( _gid + 1 ) * " << pChildAction->getLocalDomainSize() << "; ++childIndex )\n";
        os << "         {\n";
        os << "             " << pChildAction->getName() << "_stopper( childIndex );\n";
        os << "         }\n";
                    }
                }
        os << "         " << Printer( pRunningData, "_gid" ) << " = " << EG_INVALID_TIMESTAMP << ";\n";
        os << "         " << Printer( pPausedData, "_gid" ) << " = " << EG_INVALID_TIMESTAMP << ";\n";
        os << "         " << Printer( pCoroutineData, "_gid" ) << ".destroy();\n";
        os << "         " << EG_EVENT_LOG_EVENT_TYPE << " ev = { \"stop\", clock::subcycle(), &" << Printer( pReferenceData, "_gid" ) << ", sizeof( " << EG_REFERENCE_TYPE << " ) };\n";
        os << "         g_eg_event_log->PutEvent( ev );\n";
        os << "     }\n";
            }
        
        os << "}\n";
        os << "\n";
        }
        
    }
    
    void generateImplementationSource( std::ostream& os, const ImplementationSession& program, 
            std::size_t szTranslationUnitID, const std::vector< std::string >& dependencies )
    {
        const interface::Root* pRoot = program.getTreeRoot();
        const concrete::Action* pInstanceRoot = program.getInstanceRoot();
        
        const DerivationAnalysis& derivationAnalysis = program.getDerivationAnalysis();
        const Layout& layout = program.getLayout();
        
        const IndexedObject::Array& objects = program.getObjects( eg::IndexedObject::MASTER_FILE );
        
        os << "#include \"structures.hpp\"\n\n";
        
        os << "//eg implementation source code\n";
        os << "\n//buffers\n";
        for( const Buffer* pBuffer : layout.getBuffers() )
        {
            os << "static " << pBuffer->getTypeName() << " *" << pBuffer->getVariableName() << ";\n";
            //os << "static_assert( sizeof( " << pBuffer->getTypeName() << " ) == " << szBufferSize << ", \"Incorrect buffer size\" );\n";
        }
        
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
        
     
   const char* pszDefaultInterfaces = R"(
//global dependencies
static eg::_clock* g_eg_clock;

//clock impl\n";
eg::TimeStamp clock::cycle()    { return g_eg_clock->cycle(); }
eg::TimeStamp clock::subcycle() { return g_eg_clock->subcycle(); }
float clock::ct()               { return g_eg_clock->ct(); }
float clock::dt()               { return g_eg_clock->dt(); }

static eg::_event_log* g_eg_event_log;

void events::put( const char* type, eg::TimeStamp timestamp, const void* value, std::size_t size )
{
    eg::_event ev = { type, timestamp, value, size };
    g_eg_event_log->PutEvent( ev );
}
)";
        os << pszDefaultInterfaces;
        
        //initialiser
        os << "void initialise( " << EG_DEPENDENCY_PROVIDER_TYPE << "* pDependencyProvider )\n";
        os << "{    \n";
        os << "    //buffers\n";
        for( const Buffer* pBuffer : layout.getBuffers() )
        {
        os << "    " << pBuffer->getVariableName() << " = reinterpret_cast< " << pBuffer->getTypeName() << 
            "* >( pDependencyProvider->getBuffer( \"" << pBuffer->getVariableName() << "\" ) );\n";
        }
        os << "    //global dependencies\n";
        os << "    g_eg_clock = reinterpret_cast< eg::_clock* >( pDependencyProvider->getInterface( \"_clock\" ) );\n";
        os << "    g_eg_event_log = reinterpret_cast< eg::_event_log* >( pDependencyProvider->getInterface( \"_event_log\" ) );\n";
        for( const std::string& strDependency : dependencies )
        {
            std::string strVariable = strDependency;
            strVariable[ 0 ] = 'g';
        os << "    " << strVariable << " = reinterpret_cast< " << strDependency << "* >( pDependencyProvider->getInterface( \"" << strDependency << "\" ) );\n";
        }            
        os << "}\n";
        os << "\n";
        
        os << "//input::Action Function Implementations\n";
        for( const concrete::Action* pAction : actions )
        {
            if( pAction->getParent() )
            {
                os << "\n";
                generateActionInstanceFunctions( os, layout, pAction );
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
        program.getInvocations( szTranslationUnitID, invocations );
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
            const DataMember* pRunning = layout.getDataMember( pConcreteAction->getRunningTimestamp() );
        os << "        case " << pConcreteAction->getIndex() << ": //" << pConcreteAction->getFriendlyName() << "\n";
        os << "            return " << Printer( pRunning, "instance" ) << ";\n";
        }
        os << "        default: return " << EG_INVALID_TIMESTAMP << ";\n";
        
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
            InvokeVisitor visitor( os );
            pRoot->pushpop( visitor );
        }
                
        os << "\n";
        os << "\n";
    }
    
}