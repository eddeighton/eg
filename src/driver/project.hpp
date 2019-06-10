

#ifndef EG_PROJECT_UTIL_10_06_2019
#define EG_PROJECT_UTIL_10_06_2019

#include "schema.hxx"

#include <boost/filesystem.hpp>

#include <vector>

std::string printPath( const boost::filesystem::path& thePath );

class Environment
{
public:
    static const boost::filesystem::path EG_FILE_EXTENSION;
    static void startCompilationCommand( std::ostream& os );
    static const boost::filesystem::path& getEGLibraryInclude();
};

class Project
{
public:
    Project( const boost::filesystem::path& directory, const egxml::Project& project )
        :   m_directory( directory ),
            m_project( project )
    {
        
    }
    
    const egxml::Project& getProject() const { return m_project; }
    
    
    
    std::vector< boost::filesystem::path > getEGSourceCode() const;
    std::vector< boost::filesystem::path > getHostSystemIncludes() const;
    std::vector< boost::filesystem::path > getHostUserIncludes() const;
    std::vector< boost::filesystem::path > getIncludeDirectories() const;
    
    boost::filesystem::path getIntermediateFolder() const;
    boost::filesystem::path getTreeFileName() const;
    boost::filesystem::path getIncludeHeader() const;
    boost::filesystem::path getIncludePCH() const;
    boost::filesystem::path getInterfaceHeader() const;
    boost::filesystem::path getInterfacePCH() const;
    
    
    
private:
    const boost::filesystem::path& m_directory;
    const egxml::Project& m_project;
};

#endif //EG_PROJECT_UTIL_10_06_2019