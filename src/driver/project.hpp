

#ifndef EG_PROJECT_UTIL_10_06_2019
#define EG_PROJECT_UTIL_10_06_2019

#include "schema.hxx"

#include "environment_string.hpp"

#include <boost/filesystem.hpp>

#pragma warning( push )
#pragma warning( disable : 4996) //iterator thing
#pragma warning( disable : 4244) //conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#include <boost/process/env.hpp>
#include <boost/dll/runtime_symbol_info.hpp>
#pragma warning( pop )

#include <memory>
#include <vector>
#include <optional>
#include <map>
#include <functional>

class XMLManager
{
public:
    using XMLDocPtr     = std::shared_ptr< egxml::EG >;
    using XMLDocPtrMap  = std::map< boost::filesystem::path, XMLDocPtr >;
    
    static XMLDocPtr load( const boost::filesystem::path& docPath );
    
private:
    static XMLDocPtrMap m_documents;
};

class Environment
{
public:
    static const boost::filesystem::path EG_FILE_EXTENSION;
    static const boost::filesystem::path PYTHON_FILE_EXTENSION;
    
    static const std::string EG_INSTALLATION;
    static const std::string CURRENT_PROJECT;
    static const std::string WINDOWS_10_SDK;
    
    Environment( const boost::filesystem::path& projectDir );
    
    std::string printPath( const boost::filesystem::path& thePath ) const;
    void startCompilationCommand( std::ostream& os ) const;
    const boost::filesystem::path& getEGLibraryInclude() const;
    
    std::string expand( const std::string& strPath ) const;
    
    const boost::filesystem::path& getProjectDir() const { return m_projectDir; }
    
    const std::string get( const std::string& strKey ) const;
    
    const egxml::Host&  getHost( const std::string& strHost ) const;
    const egxml::Package& getPackage( const std::string& strPackage ) const;
    
private:
    boost::filesystem::path m_projectDir;
    boost::process::environment m_environment;
};

class Project
{
public:
    Project( const Environment& environment, const egxml::Project& project, const std::string& strBuildCommand );
    
    const egxml::Project& getProject() const { return m_project; }
    
    const std::string& getCompilerFlags()   const;
    const std::string& getLinkerFlags()     const;
    
    std::vector< boost::filesystem::path > getEGSourceCode() const;
    std::vector< boost::filesystem::path > getHostSystemIncludes() const;
    std::vector< boost::filesystem::path > getHostUserIncludes() const;
    std::vector< boost::filesystem::path > getIncludeDirectories() const;
    std::vector< boost::filesystem::path > getLibraryDirectories() const;
    std::vector< boost::filesystem::path > getCPPSourceCode() const;
    std::vector< boost::filesystem::path > getCPPLibs() const;
    
    boost::filesystem::path getIntermediateFolder() const;
    boost::filesystem::path getTreeFileName() const;
    boost::filesystem::path getIncludeHeader() const;
    boost::filesystem::path getPreprocessedFile() const;
    boost::filesystem::path getIncludePCH() const;
    boost::filesystem::path getInterfaceHeader() const;
    boost::filesystem::path getInterfacePCH() const;
    
    boost::filesystem::path getOperationsHeader( std::size_t szUnitID ) const;
    boost::filesystem::path getTUName( std::size_t szUnitID ) const;
    boost::filesystem::path getOperationsPCH( std::size_t szUnitID ) const;
    
    boost::filesystem::path getAnalysisFileName() const;
    boost::filesystem::path getDataStructureSource() const;
    boost::filesystem::path getImplementationSource( int szUnitID ) const;
    
    boost::filesystem::path getObjectName( int szUnitID ) const;
    boost::filesystem::path getObjectFile( const boost::filesystem::path& sourceFile ) const;
    
    boost::filesystem::path getHostCommand() const;
    boost::filesystem::path getProgramName() const;
private:
    const Environment& m_environment;
    const egxml::Project& m_project;
    const std::string& m_strBuildCommand;
    const egxml::Host& m_host;
    std::vector< std::reference_wrapper< const egxml::Package > > m_packages;
};

#endif //EG_PROJECT_UTIL_10_06_2019