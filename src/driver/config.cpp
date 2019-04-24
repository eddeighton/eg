
#include "config.hpp"

#include "eg/eg.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"
#include "common/escape.hpp"

#pragma warning( push )
#pragma warning( disable : 4996) //iterator thing
#pragma warning( disable : 4244) //conversion to DWORD from system_clock::rep
#include "boost/process.hpp"
#pragma warning( pop )

#include "boost/filesystem.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/xml_parser.hpp"
//#include <boost/property_tree/json_parser.hpp>
//#include <boost/property_tree/ini_parser.hpp>


template< class T >
void appendIfUnique( std::vector< T >& vec, const T& value )
{
    bool bFound = false;
    for( const T& existing : vec )
    {
        if( existing == value )
        {
            bFound = true;
            break;
        }
    }
    if( !bFound )
        vec.push_back( value );
}

std::string printPath( const boost::filesystem::path& thePath )
{
    return Common::escapeString( thePath.generic_string() );
}

Configuration::Configuration()
{
    if( EG_LIBRARY.empty() )
    {
        if( const char* pszEnvPath = getenv( "EG" ) )
        {
            EG_LIBRARY = std::string( pszEnvPath );
        }
    }
    
    DEFAULT = EG_LIBRARY / std::string( "default_config.xml" );
            
    CLANG   = boost::process::search_path( "clang.exe" );
    
    projectFolder = boost::filesystem::current_path();
}

void Configuration::loadHostConfig( const std::string& strHost )
{
    const boost::filesystem::path hostPath( strHost );
    const std::string strHostName = hostPath.filename().stem().generic_string();
    const boost::filesystem::path hostConfigFile =
        boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( EG_LIBRARY / ( strHostName + ".xml" ) ) );
    if( !boost::filesystem::exists( hostConfigFile ) )
    {
        THROW_RTE( "Cannot locate host config file: " << hostConfigFile.generic_string() );
    }
    
    load( hostConfigFile, strHost );
}

void Configuration::load( const boost::filesystem::path& filePath, const std::string& strPrefix )
{
    boost::property_tree::ptree config_tree;
    boost::property_tree::read_xml( filePath.string(), config_tree );
    
    if( projectFolder.empty() )
        projectFolder    = config_tree.get( strPrefix + ".folder",  projectFolder.generic_string() );
    
    if( strCompilerFlags.empty() )
        strCompilerFlags    = config_tree.get( strPrefix + ".flags",        strCompilerFlags );
    if( strHostFlags.empty() )
        strHostFlags        = config_tree.get( strPrefix + ".hostflags",    strHostFlags );
    if( strInputPCH.empty() )
        strInputPCH         = config_tree.get( strPrefix + ".pch",          strInputPCH );
    if( hostCmd.empty() )
        hostCmd             = config_tree.get( strPrefix + ".hostcmd",      hostCmd.generic_string() );
    
    if( hostCmd.empty() )
    {
        hostCmd = config_tree.get( strPrefix + ".hostcmd",   hostCmd.generic_string() );
        hostCmd = boost::process::search_path( hostCmd );
    }
    
    if( boost::optional< boost::property_tree::ptree& > sources =
            config_tree.get_child_optional( strPrefix + ".dependencies" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : sources.get() ) 
        {
            if( v.first == "interface" )
            {
                // The data function is used to access the data stored in a node.
                dependencies.push_back( v.second.data() );
            }
        }
    }
    
    if( boost::optional< boost::property_tree::ptree& > sources =
            config_tree.get_child_optional( strPrefix + ".source.eg" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : sources.get() ) 
        {
            if( v.first == "file" )
            {
                // The data function is used to access the data stored in a node.
                inputFiles.push_back( v.second.data() );
            }
        }
    }
    if( boost::optional< boost::property_tree::ptree& > sources =
            config_tree.get_child_optional( strPrefix + ".source.generated" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : sources.get() ) 
        {
            if( v.first == "file" )
            {
                // The data function is used to access the data stored in a node.
                boost::filesystem::path actualPath = getIntermediateFolder() / v.second.data();
                inputFiles.push_back( actualPath.generic_string() );
            }
        }
    }
    if( boost::optional< boost::property_tree::ptree& > sources =
            config_tree.get_child_optional( strPrefix + ".source.cpp" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : sources.get() ) 
        {
            if( v.first == "file" )
            {
                // The data function is used to access the data stored in a node.
                inputFiles.push_back( v.second.data() );
            }
        }
    }
    if( boost::optional< boost::property_tree::ptree& > sources =
            config_tree.get_child_optional( strPrefix + ".libs" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : sources.get() ) 
        {
            if( v.first == "file" )
            {
                // The data function is used to access the data stored in a node.
                cppLibs.push_back( v.second.data() );
            }
        }
    }
    if( boost::optional< boost::property_tree::ptree& > sources =
            config_tree.get_child_optional( strPrefix + ".libs.includedirectories" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : sources.get() ) 
        {
            if( v.first == "folder" )
            {
                // The data function is used to access the data stored in a node.
                cppIncludeDirectories.push_back( v.second.data() );
            }
        }
    }
    
    if( boost::optional< boost::property_tree::ptree& > includes =
            config_tree.get_child_optional( strPrefix + ".include.user" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : includes.get() ) 
        {
            if( v.first == "file" )
            {
                const std::string& strSourceFile = v.second.data();
                appendIfUnique( hostIncludeUser, boost::filesystem::path( strSourceFile ) );
            }
        }
    }
    
    if( boost::optional< boost::property_tree::ptree& > includes =
            config_tree.get_child_optional( strPrefix + ".include.system" ) )
    {
        for( const boost::property_tree::ptree::value_type& v : includes.get() ) 
        {
            if( v.first == "file" )
            {
                const std::string& strSourceFile = v.second.data();
                appendIfUnique( hostIncludeSystem, boost::filesystem::path( strSourceFile ) );
            }
        }
    }
    
    filterFiles();
}

void Configuration::save( const boost::filesystem::path& filePath )
{
    boost::property_tree::ptree config_tree;
    
    if( !projectFolder.empty() )
        config_tree.put( "project.folder", projectFolder.generic_string() );
    
    if( !strCompilerFlags.empty() )
        config_tree.put( "project.flags", strCompilerFlags );
    
    if( !strHostFlags.empty() )
        config_tree.put( "project.hostflags", strHostFlags );
    
    if( !strInputPCH.empty() )
        config_tree.put( "project.pch",   strInputPCH );
    
    if( !hostCmd.empty() )
        config_tree.put( "project.hostcmd",  hostCmd );
    
    for( const std::string& strDependency : dependencies )
    {
        config_tree.add( "project.dependencies.interface", strDependency );
    }
    
    for( const boost::filesystem::path& egFilePath : egSourceCode )
    {
        config_tree.add( "project.source.eg.file", 
            boost::filesystem::relative( egFilePath, filePath.parent_path() ).generic_string() );
    }
    for( const boost::filesystem::path& cppFilePath : cppSourceCode )
    {
        config_tree.add( "project.source.cpp.file", 
            boost::filesystem::relative( cppFilePath, filePath.parent_path() ).generic_string() );
    }
    for( const boost::filesystem::path& cppLibFilePath : cppLibs )
    {
        config_tree.add( "project.libs.file", cppLibFilePath.generic_string() );
    }
    for( const boost::filesystem::path& cppIncludeFolder : cppIncludeDirectories )
    {
        config_tree.add( "project.libs.includedirectories.folder", cppIncludeFolder.generic_string() );
    }
    for( const boost::filesystem::path& cppIncludeFilePath : hostIncludeUser )
    {
        config_tree.add( "project.include.user.file", cppIncludeFilePath.generic_string() );
    }
    for( const boost::filesystem::path& cppIncludeFilePath : hostIncludeSystem )
    {
        config_tree.add( "project.include.system.file", cppIncludeFilePath.generic_string() );
    }
    
    boost::property_tree::xml_writer_settings< std::string > settings( ' ', 4 );
    boost::property_tree::write_xml( filePath.string(), config_tree, std::locale(), settings );
}

boost::filesystem::path Configuration::getIntermediateFolder() const
{
    boost::filesystem::path intermediateFolder = 
        projectFolder / ( strProjectName + std::string( "_build" ) );
    if( !intermediateFolder.empty() )
    {
        boost::filesystem::ensureFoldersExist( intermediateFolder );
    }
    return intermediateFolder;
}

boost::filesystem::path Configuration::getTreeFileName() const
{
    std::ostringstream os;
    os << "master.db";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getAnalysisFileName() const
{
    std::ostringstream os;
    os << "database.db";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getIncludeHeader() const
{
    std::ostringstream os;
    os << "includes.hpp";
        
    return boost::filesystem::edsCannonicalise(
            boost::filesystem::absolute( 
                getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getIncludePCH() const
{
    if( strInputPCH.empty() )
    {
        std::ostringstream os;
        os << "includes.pch";
        return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
    }
    else
    {
        return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strInputPCH ) );
    }
}

boost::filesystem::path Configuration::getInterfaceHeader() const
{
    std::ostringstream os;
    os << "interface.hpp";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getInterfacePCH() const
{
    std::ostringstream os;
    os << "interface.pch";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getOperationsHeader( int iIndex ) const
{
    std::ostringstream os;
    os << "operations_" << std::setw( 3 ) << std::setfill( '0' ) << iIndex - 2 << ".hpp";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getOperationsPCH( int iIndex ) const
{
    std::ostringstream os;
    os << "operations_" << std::setw( 3 ) << std::setfill( '0' ) << iIndex - 2 << ".pch";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getDataStructureSource() const
{
    std::ostringstream os;
    os << "structures.hpp";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getDataStructurePCH() const
{
    std::ostringstream os;
    os << "structures.pch";
    return boost::filesystem::edsCannonicalise(
        boost::filesystem::absolute( 
            getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getImplementationSource( int iIndex ) const
{
    std::ostringstream os;
    os << "impl_" << std::setw( 3 ) << std::setfill( '0' ) << iIndex - 2 << ".cpp";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}
                
boost::filesystem::path Configuration::getTUName( int iIndex ) const
{
    std::ostringstream os;
    os << "tu_" << std::setw( 3 ) << std::setfill( '0' ) << iIndex - 2 << ".db";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}

boost::filesystem::path Configuration::getObjectName( int iIndex ) const
{
    std::ostringstream os;
    os << "object_" << std::setw( 3 ) << std::setfill( '0' ) << iIndex - 2 << ".obj";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( 
                    getIntermediateFolder() / os.str() ) );
}
        
boost::filesystem::path Configuration::getProgramName() const
{
    std::ostringstream os;
    os << strProjectName << ".exe";
    return boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( os.str() ) );
}

void Configuration::startCommand( std::ostream& os, bool bIncludeHostSettings /*= false*/ )
{
    os << printPath( CLANG ) << " ";
    if( !strCompilerFlags.empty() )
        os << strCompilerFlags << " ";
    if( bIncludeHostSettings )
        os << strHostFlags << " ";
}

void Configuration::startHostCommand( std::ostream& os )
{
    //do not use printPath for host exe
    os << hostCmd.generic_string() << " ";
}

void Configuration::filterFiles()
{
    //filter the source code files
    for( const std::string& strSourceFile : inputFiles )
    {
        const boost::filesystem::path sourceFilePath =
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strSourceFile ) );
        //might be host file so may not exist yet...
        //if( !boost::filesystem::exists( sourceFilePath ) )
        //{
        //    THROW_RTE( "Cannot find file: " << sourceFilePath.string() );
        //}
        //else
        {
            //uniquify the file paths here retaining their specification order
            const boost::filesystem::path sourceFileExtension = 
                boost::filesystem::extension( sourceFilePath );
            if( sourceFileExtension == eg::EG_FILE_EXTENSION )
            {
                appendIfUnique( egSourceCode, sourceFilePath );
            }
            else if( sourceFileExtension == ".xml" )
            {
                appendIfUnique( xmlFiles, sourceFilePath );
            }
            else
            {
                appendIfUnique( cppSourceCode, sourceFilePath );
            }
        }
    }
    VERIFY_RTE_MSG( xmlFiles.size() <= 1U, "Multiple config files specified" );
}

void Configuration::checkProjectName()
{
    if( strProjectName.empty() )
    {
        if( !xmlFiles.empty() )
        {
            strProjectName = xmlFiles.front().stem().string();
        }
        else if( !egSourceCode.empty() )
        {
            strProjectName = egSourceCode.front().stem().string();
        }
        
        if( strProjectName.empty() )
        {
            THROW_RTE( "Could not determine project name" );
        }        
    }
}

