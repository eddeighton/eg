

#ifndef EG_PROJECT_UTIL_10_06_2019
#define EG_PROJECT_UTIL_10_06_2019

#include "egxml/eg_schema.hxx"

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
    static const boost::filesystem::path INSTALLATION_FILE;
    static const boost::filesystem::path WIZARD_FILE;
    static const boost::filesystem::path PYTHON_FILE_EXTENSION;
    static const std::string ENV_KEY_EG_INSTALLATION;
    static const std::string ENV_KEY_CURRENT_PROJECT;
    //static const std::string ENV_KEY_WINDOWS_10_SDK;
    //static const std::string ENV_KEY_VISUALSTUDIO;
    //static const std::string ENV_KEY_PYTHONHOME;
    
    void commonCtor();
    
    Environment();
    Environment( const boost::filesystem::path& projectDir );
    Environment( const boost::process::environment& env, const boost::filesystem::path& projectDir );
    
    std::string printPath( const boost::filesystem::path& thePath ) const;
    void startCompilationCommand( std::ostream& os ) const;
    void startLogCommand( std::ostream& os ) const;
    void startDriverCommand( std::ostream& os ) const;
    const boost::filesystem::path& getEGLibraryInclude() const;
    const boost::filesystem::path& getClangPluginDll() const;
    
    std::string expand( const std::string& strPath ) const;
    
    const std::string get( const std::string& strKey ) const;
    
    const egxml::Host& getHost( const std::string& strHost ) const;
    const egxml::Package& getPackage( const std::string& strPackage ) const;
    
    const boost::process::environment& getEnvironment() const { return m_environment; }
private:
    boost::process::environment m_environment;
};

class Project
{
public:
    Project( const boost::filesystem::path& projectDir, 
        const Environment& environment, const egxml::Project& project );
    Project( const boost::filesystem::path& projectDir, 
        const Environment& environment, const egxml::Project& project, const std::string& strBuildCommand );
    
    const boost::filesystem::path& getProjectDir() const { return m_projectDir; }
    
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
    
    boost::filesystem::path getParserDBFileName() const;
    boost::filesystem::path getInterfaceDBFileName() const;
    
    boost::filesystem::path getIncludeHeader() const;
    boost::filesystem::path getPreprocessedFile() const;
    boost::filesystem::path getPreprocessedCompareFile() const;
    boost::filesystem::path getIncludePCH() const;
    boost::filesystem::path getInterfaceHeader() const;
    boost::filesystem::path getInterfacePCH() const;
    
    boost::filesystem::path getOperationsHeader( const std::string& strTUName ) const;
    boost::filesystem::path getTUDBName( const std::string& strTUName ) const;
    boost::filesystem::path getOperationsPCH( const std::string& strTUName ) const;
    
    boost::filesystem::path getAnalysisFileName() const;
    boost::filesystem::path getDataStructureSource() const;
    boost::filesystem::path getImplementationSource( const std::string& strTUName ) const;
    boost::filesystem::path getRuntimeSource() const;
    
    boost::filesystem::path getObjectName( const std::string& strTUName ) const;
    boost::filesystem::path getObjectFile( const boost::filesystem::path& sourceFile ) const;
    
    std::vector< boost::filesystem::path > getCommands() const;
   // std::vector< std::string > getPackages() const;
    boost::filesystem::path getProgramName() const;
	
	bool isPybindRequired() const;
private:
    const boost::filesystem::path m_projectDir;
    Environment m_environment;
    const egxml::Project& m_project;
    const std::string m_strBuildCommand;
    const egxml::Host& m_host;
    std::vector< std::reference_wrapper< const egxml::Package > > m_packages;
};

#endif //EG_PROJECT_UTIL_10_06_2019