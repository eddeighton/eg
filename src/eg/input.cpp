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
        loader.load( m_strIdentifier );
        m_pType = loader.loadObjectRef< Opaque >();
    }

    void Dimension::store( Storer& storer ) const
    {
        storer.store( m_strIdentifier );
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
        loader.load( m_strIdentifier );
        loader.load( m_path );
        loader.load( m_bIsEGInclude );
        loader.load( m_bIsSystemInclude );
    }

    void Include::store( Storer& storer ) const
    {
        storer.store( m_strIdentifier );
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
        loader.load( m_strIdentifier );
        m_pType = loader.loadObjectRef< Opaque >();
    }

    void Using::store( Storer& storer ) const
    {
        storer.store( m_strIdentifier );
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
        loader.load( m_strIdentifier );
        m_pReturnType = loader.loadObjectRef< Opaque >();
        m_pParameters = loader.loadObjectRef< Opaque >();
        m_pBody = loader.loadObjectRef< Opaque >();
    }

    void Export::store( Storer& storer ) const
    {
        storer.store( m_strIdentifier );
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
    
    Action::Action( const IndexedObject& object )
        :   Element( object ),
            m_pSize( nullptr ),
            m_pParams( nullptr ),
            m_pBody( nullptr )
    {

    }

    void Action::load( Loader& loader )
    {
        loader.loadObjectVector( m_elements );
        m_pSize = loader.loadObjectRef< Opaque >();
        m_pParams = loader.loadObjectRef< Opaque >();
        m_pBody = loader.loadObjectRef< Opaque >();
        loader.loadOptional( m_definitionFile );
        loader.load( m_strIdentifier );
        loader.load( m_bAbstract );
        loader.load( m_bLink );
        loader.loadObjectVector( m_inheritance );
    }

    void Action::store( Storer& storer ) const
    {
        storer.storeObjectVector( m_elements );
        storer.storeObjectRef( m_pSize );
        storer.storeObjectRef( m_pParams );
        storer.storeObjectRef( m_pBody );
        storer.storeOptional( m_definitionFile );
        storer.store( m_strIdentifier );
        storer.store( m_bAbstract );
        storer.store( m_bLink );
        storer.storeObjectVector( m_inheritance );
    }
    
    Action* Action::findAction( const std::string& strIdentifier ) const
    {
        for( Element* pObject : m_elements )
        {
            if( Action* pAction = dynamic_cast< Action* >( pObject ) )
            {
                if( pAction->m_strIdentifier == strIdentifier )
                    return pAction;
            }
        }
        return nullptr;
    }
    
    void Action::printDeclaration( std::ostream& os, std::string& strIndent, 
        const std::string& strIdentifier, const std::string& strAnnotation ) const
    {
        if( m_bIsTemplate )
        {
            os << strIndent << "template<...>\n";
        }
        
        if( m_bAbstract && m_bLink )
        {
            os << strIndent << "abstract link " << strIdentifier;
        }
        else if( m_bAbstract )
        {
            os << strIndent << "abstract " << strIdentifier;
        }
        else if( m_bLink )
        {
            os << strIndent << "link " << strIdentifier;
        }
        else
        {
            os << strIndent << "action " << strIdentifier;
        }
        
        if( m_pSize )
        {
            os << "[ " << m_pSize->getStr() << " ]";
        }
        
        for( const Opaque* pInherited : m_inheritance )
        {
            if( pInherited == m_inheritance.front() )
                os << " : " << pInherited->getStr();
            else
                os << ", " << pInherited->getStr();
        }
        
        os << " //" << strAnnotation;
    }
    
    void Action::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        printDeclaration( os, strIndent, m_strIdentifier, strAnnotation );
    }
    
    
    const std::string Root::RootTypeName = "root";

    Root::Root( const IndexedObject& object )
        :   Action( object )
    {
        m_strIdentifier = RootTypeName;
		m_rootType = eFile;
    }
    
    void Root::load( Loader& loader )
    {
        Action::load( loader );
        loader.loadOptional( m_includePath );
        loader.load( m_rootType );
    }

    void Root::store( Storer& storer ) const
    {
        Action::store( storer );
        storer.storeOptional( m_includePath );
        storer.store( m_rootType );
    }
    
    void Root::print( std::ostream& os, std::string& strIndent, const std::string& strAnnotation ) const
    {
        Action::print( os, strIndent, strAnnotation );
    }

} //namespace input
} //namespace eg
