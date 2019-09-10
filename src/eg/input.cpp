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


#include "input.hpp"
#include "eg.hpp"

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
        :  Element( object )
    {
    }
    
    void Opaque::load( Loader& loader )
    {
        loader.load( m_str );
    }

    void Opaque::store( Storer& storer ) const
    {
        storer.store( m_str );
    }
    
    void Opaque::print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const
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
    
    void Dimension::print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const
    {
        VERIFY_RTE( m_pType );
        os << strIndent << "dim " << m_pType->getStr() << " " << m_strIdentifier << ";//" << szIndex << "\n";
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
    
    void Include::print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const
    {
        if( m_strIdentifier.empty() )
            os << strIndent << "include( " << m_path.string() << " );//" << szIndex << "\n";
        else
            os << strIndent << "include " << m_strIdentifier << "( " << m_path.string() << " );//" << szIndex << "\n";
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
    
    
    Action::Action( const IndexedObject& object )
        :   Element( object ),
            m_pSize( nullptr ),
            m_pParams( nullptr )
    {

    }

    void Action::load( Loader& loader )
    {
        loader.loadObjectVector( m_elements );
        m_pSize = loader.loadObjectRef< Opaque >();
        m_pParams = loader.loadObjectRef< Opaque >();
        loader.load( m_bDefined );
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
        storer.store( m_bDefined );
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
    
    void Action::printDeclaration( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const
    {
        if( m_bIsTemplate )
        {
            os << strIndent << "template<...>\n";
        }
        
        if( m_bAbstract && m_bLink )
        {
            os << strIndent << "abstract link " << m_strIdentifier;
        }
        else if( m_bAbstract )
        {
            os << strIndent << "abstract " << m_strIdentifier;
        }
        else if( m_bLink )
        {
            os << strIndent << "link " << m_strIdentifier;
        }
        else
        {
            os << strIndent << "action " << m_strIdentifier;
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
        
        os << " //" << szIndex;
    }
    
    void Action::print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const
    {
        printDeclaration( os, strIndent, getIndex() );
        
        /*os << "\n" << strIndent << "{\n";

        strIndent.push_back( ' ' );
        strIndent.push_back( ' ' );

        for( const Element* pElement : m_elements )
        {
            pElement->print( os, strIndent );
        }

        strIndent.pop_back();
        strIndent.pop_back();

        os << "\n" << strIndent << "}//" << szIndex << "\n";*/
    }
    
    
    const std::string Root::RootTypeName = "root";

    Root::Root( const IndexedObject& object )
        :   Action( object ),
            m_bMainFile( false )
    {
        m_strIdentifier = RootTypeName;
    }
    
    void Root::load( Loader& loader )
    {
        Action::load( loader );
        loader.load( m_path );
        loader.load( m_bMainFile );
    }

    void Root::store( Storer& storer ) const
    {
        Action::store( storer );
        storer.store( m_path );
        storer.store( m_bMainFile );
    }
    
    void Root::print( std::ostream& os, std::string& strIndent, IndexedObject::Index szIndex ) const
    {
        Action::print( os, strIndent, szIndex );
    }

} //namespace input
} //namespace eg
