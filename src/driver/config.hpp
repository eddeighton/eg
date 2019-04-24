
#ifndef DRIVER_CONFIG_19_04_2019
#define DRIVER_CONFIG_19_04_2019

#include "boost/filesystem/path.hpp"

#include <string>

std::string printPath( const boost::filesystem::path& thePath );

class Configuration
{
public:

    std::string strProjectName;
    std::string strInputPCH;
    std::string strCompilerFlags, strHostFlags; 
    
    boost::filesystem::path projectFolder;
    
    std::vector< std::string > dependencies;
    std::vector< std::string > inputFiles;
    std::vector< boost::filesystem::path > egSourceCode;
    std::vector< boost::filesystem::path > cppSourceCode;
    std::vector< boost::filesystem::path > xmlFiles;
    
    std::vector< boost::filesystem::path > cppLibs;
    std::vector< boost::filesystem::path > cppIncludeDirectories;
    std::vector< boost::filesystem::path > hostIncludeUser;
    std::vector< boost::filesystem::path > hostIncludeSystem;
    
    boost::filesystem::path hostCmd;
    
    boost::filesystem::path EG_LIBRARY;
    boost::filesystem::path DEFAULT;
    boost::filesystem::path CLANG;
    
    Configuration();

    void loadHostConfig( const std::string& strHost );
    
    void load( const boost::filesystem::path& filePath, const std::string& strPrefix );

    void save( const boost::filesystem::path& filePath );
    
    boost::filesystem::path getIntermediateFolder() const;
    
    boost::filesystem::path getTreeFileName() const;

    boost::filesystem::path getAnalysisFileName() const;

    boost::filesystem::path getIncludeHeader() const;

    boost::filesystem::path getIncludePCH() const;

    boost::filesystem::path getInterfaceHeader() const;

    boost::filesystem::path getInterfacePCH() const;

    boost::filesystem::path getOperationsHeader( int iIndex ) const;

    boost::filesystem::path getOperationsPCH( int iIndex ) const;
    
    boost::filesystem::path getDataStructureSource() const;
    
    boost::filesystem::path getDataStructurePCH() const;
    
    boost::filesystem::path getImplementationSource( int iIndex ) const;
                    
    boost::filesystem::path getTUName( int iIndex ) const;

    boost::filesystem::path getObjectName( int iIndex ) const;
            
    boost::filesystem::path getProgramName() const;

    void startCommand( std::ostream& os, bool bIncludeHostSettings = false );
    
    void startHostCommand( std::ostream& os );
    
    void filterFiles();
    
    void checkProjectName();
};


#endif //DRIVER_CONFIG_19_04_2019