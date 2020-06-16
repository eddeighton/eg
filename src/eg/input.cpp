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


#include "eg_compiler/input.hpp"
#include "eg_compiler/eg.hpp"

#include "common/file.hpp"

namespace eg
{
namespace input
{
    
    
    
    void HasIdentifier::load( Loader& loader )
    {
        loader.load( m_strIdentifier );
    }
    
    void HasIdentifier::store( Storer& storer ) const
    {
        storer.store( m_strIdentifier );
    }
    
    void HasChildren::load( Loader& loader )
    {
        loader.loadObjectVector( m_elements );
    }
    
    void HasChildren::store( Storer& storer ) const
    {
        storer.storeObjectVector( m_elements );
    }
    
    
    void HasDomain::load( Loader& loader )
    {
        m_pSize = loader.loadObjectRef< Opaque >();
    }
    
    void HasDomain::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_pSize );
    }
    
    void HasParameters::load( Loader& loader )
    {
        m_pReturnType = loader.loadObjectRef< Opaque >();
        m_pParams = loader.loadObjectRef< Opaque >();
    }
    
    void HasParameters::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_pReturnType );
        storer.storeObjectRef( m_pParams );
    }
    
    void HasBody::load( Loader& loader )
    {
        m_pBody = loader.loadObjectRef< Opaque >();
        loader.loadOptional( m_definitionFile );
    }
    
    void HasBody::store( Storer& storer ) const
    {
        storer.storeObjectRef( m_pBody );
        storer.storeOptional( m_definitionFile );
    }
    
    void HasInheritance::load( Loader& loader )
    {
        loader.loadObjectVector( m_inheritance );
    }
    
    void HasInheritance::store( Storer& storer ) const
    {
        storer.storeObjectVector( m_inheritance );
    }
    
    
    
    void printDeclaration( std::ostream& os, 
        std::string& strIndent, 
        const std::string& strInputType, 
        const std::string& strIdentifier, 
        const Opaque* pReturnType,
        const Opaque* pParams,
        const Opaque* pSize,
        const std::vector< Opaque* >& inheritance,
        const std::string& strAnnotation  )
    {
        os << strIndent << strInputType << " " << strIdentifier;
        
        if( pParams )
        {
            os << "( " << pParams->getStr() << " )";
        }
        
        if( pSize )
        {
            os << "[ " << pSize->getStr() << " ]";
        }
        
        if( pReturnType )
        {
            os << " : " << pReturnType->getStr();
        }
        
        for( const Opaque* pInherited : inheritance )
        {
            if( pInherited == inheritance.front() )
                os << " : " << pInherited->getStr();
            else
                os << ", " << pInherited->getStr();
        }
        
        os << " //" << strAnnotation;
    }
    
    
    
    

    Element::Element( const IndexedObject& object )
        :   IndexedObject( object )
    {

    }

    
    Opaque::Opaque( const IndexedObject& object )
        :   Element( object ),
            m_bSemantic( true )
    {
    }
    
    void Opaque::load( Loader& loader )
    {
        loader.load( m_bSemantic );
        loader.load( m_str );
    }

    void Opaque::store( Storer& storer ) const
    {
        storer.store( m_bSemantic );
        storer.store( m_str );
    }
    
    void Opaque::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        os << strIndent << m_str << "\n";// << "//" << szIndex;
    }
    
    Dimension::Dimension( const IndexedObject& object )
        :   Element( object ),
            m_pType( nullptr )
    {

    }

    void Dimension::load( Loader& loader )
    {
        HasIdentifier::load( loader );
        m_pType = loader.loadObjectRef< Opaque >();
    }

    void Dimension::store( Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.storeObjectRef( m_pType );
    }
    
    void Dimension::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        VERIFY_RTE( m_pType );
        os << strIndent << "dim " << m_pType->getStr() << " " << m_strIdentifier << ";//" << strAnnotation << "\n";
    }
    
    Include::Include( const IndexedObject& object )
        :   Element( object ),
            m_bIsEGInclude( false ),
            m_bIsSystemInclude( false )
    {

    }

    void Include::load( Loader& loader )
    {
        HasIdentifier::load( loader );
        loader.load( m_path );
        loader.load( m_bIsEGInclude );
        loader.load( m_bIsSystemInclude );
    }

    void Include::store( Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.store( m_path );
        storer.store( m_bIsEGInclude );
        storer.store( m_bIsSystemInclude );
    }
    
    void Include::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        if( m_strIdentifier.empty() )
            os << strIndent << "include( " << m_path.string() << " );//" << strAnnotation << "\n";
        else
            os << strIndent << "include " << m_strIdentifier << "( " << m_path.string() << " );//" << strAnnotation << "\n";
    }
    
    void Include::setIncludeFilePath( const std::string& strIncludeFile )
    {
        VERIFY_RTE( !strIncludeFile.empty() );
        if( strIncludeFile[ 0 ] == '<' )
        {
            m_bIsSystemInclude = true;
            m_path = strIncludeFile;
        }
        else
        {
            m_path = strIncludeFile;
            if( boost::filesystem::extension( m_path ) == FILE_EXTENSION )
            {
                m_path = boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( m_path ) );
                m_bIsEGInclude = true;
            }
            else
            {
                m_bIsEGInclude = false;
            }
        }
    }
    
    Using::Using( const IndexedObject& object )
        :   Element( object ),
            m_pType( nullptr )
    {

    }

    void Using::load( Loader& loader )
    {
        HasIdentifier::load( loader );
        m_pType = loader.loadObjectRef< Opaque >();
    }

    void Using::store( Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.storeObjectRef( m_pType );
    }
    
    void Using::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        VERIFY_RTE( m_pType );
        os << strIndent << "using " << m_strIdentifier << " = " << m_pType->getStr() << ";//" << strAnnotation << "\n";
    }
    
    Export::Export( const IndexedObject& object )
        :   Element( object ),
            m_pReturnType( nullptr ),
            m_pParameters( nullptr ),
            m_pBody( nullptr )
    {

    }

    void Export::load( Loader& loader )
    {
        HasIdentifier::load( loader );
        m_pReturnType = loader.loadObjectRef< Opaque >();
        m_pParameters = loader.loadObjectRef< Opaque >();
        m_pBody = loader.loadObjectRef< Opaque >();
    }

    void Export::store( Storer& storer ) const
    {
        HasIdentifier::store( storer );
        storer.storeObjectRef( m_pReturnType );
        storer.storeObjectRef( m_pParameters );
        storer.storeObjectRef( m_pBody );
    }
    
    void Export::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        VERIFY_RTE( m_pReturnType );
        VERIFY_RTE( m_pParameters );
        VERIFY_RTE( m_pBody );
        
        os << strIndent << "export " << m_pReturnType->getStr() << " " << 
            m_strIdentifier << "( " << m_pParameters->getStr() << " )//" << strAnnotation << "\n";
        os << "{ " << m_pBody->getStr() << " }\n";
    }
    
    Context::Context( const IndexedObject& object )
        :   Element( object )
    {

    }

    void Context::load( Loader& loader )
    {
        HasIdentifier::load( loader );
        HasChildren::load( loader );
        HasDomain::load( loader );
        HasBody::load( loader );
        HasParameters::load( loader );
        HasInheritance::load( loader );
        
        loader.load( m_contextType );
        
    }

    void Context::store( Storer& storer ) const
    {
        HasIdentifier::store( storer );
        HasChildren::store( storer );
        HasDomain::store( storer );
        HasBody::store( storer );
        HasParameters::store( storer );
        HasInheritance::store( storer );
        
        storer.store( m_contextType );
    }
    
    Context* Context::findContext( const std::string& strIdentifier ) const
    {
        for( Element* pObject : m_elements )
        {
            if( Context* pContext = dynamic_cast< Context* >( pObject ) )
            {
                if( pContext->m_strIdentifier == strIdentifier )
                    return pContext;
            }
        }
        return nullptr;
    }
    
    void Context::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        printDeclaration( os, strIndent, getContextType(), m_strIdentifier, m_pReturnType, m_pParams, m_pSize, m_inheritance, strAnnotation );
    }
    
    const char* Context::getContextType() const
    {
        switch( m_contextType )
        {
            case eAbstract :  return "abstract" ;
            case eEvent    :  return "event"    ;
            case eFunction :  return "function" ;
            case eAction   :  return "action"   ;
            case eLink     :  return "link"     ;
            case eObject   :  return "object"   ;
            default:
                THROW_RTE( "Unknown context type" );
        }
    }
    
    const std::string Root::RootTypeName = "root";

    Root::Root( const IndexedObject& object )
        :   Context( object )
    {
        m_contextType = eObject;
        m_strIdentifier = RootTypeName;
		m_rootType = eFile;
    }
    
    void Root::load( Loader& loader )
    {
        Context::load( loader );
        loader.loadOptional( m_includePath );
        loader.load( m_rootType );
    }

    void Root::store( Storer& storer ) const
    {
        Context::store( storer );
        storer.storeOptional( m_includePath );
        storer.store( m_rootType );
    }
    
    void Root::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        printDeclaration( os, strIndent, "root", m_strIdentifier, nullptr, nullptr, m_pSize, m_inheritance, strAnnotation );
    }

} //namespace input
} //namespace eg
