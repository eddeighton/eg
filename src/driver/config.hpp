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
    
    boost::filesystem::path getDLLName( int iIndex ) const;

    void startCommand( std::ostream& os, bool bIncludeHostSettings = false );
    
    void startHostCommand( std::ostream& os );
    
    void filterFiles();
    
    void checkProjectName();
};


#endif //DRIVER_CONFIG_19_04_2019