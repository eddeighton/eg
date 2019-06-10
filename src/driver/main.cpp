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



#include "config.hpp"

#include "eg/parser_session.hpp"
#include "eg/interface_session.hpp"
#include "eg/operations_session.hpp"
#include "eg/implementation_session.hpp"
#include "eg/codegen.hpp"

#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/Diagnostic.h"

#pragma warning( pop ) 

#include "common/assert_verify.hpp"
#include "common/file.hpp"

#include <boost/optional.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

#pragma warning( push )
#pragma warning( disable : 4996) //iterator thing
#pragma warning( disable : 4244) //conversion to DWORD from system_clock::rep
#include "boost/process.hpp"
#pragma warning( pop )

#include <cstdlib>

#include <iostream>
#include <iomanip>


class EGDiagConsumer : public clang::DiagnosticConsumer 
{
    virtual void anchor()
    {
    }

    void HandleDiagnostic( clang::DiagnosticsEngine::Level DiagLevel,
                        const clang::Diagnostic &Info) override 
    {
        llvm::SmallString< 100 > msg;
        Info.FormatDiagnostic( msg );
        std::string str = msg.str();
        std::cout << str << std::endl;
    }
};


struct LogEntry
{
    std::ostream& os;
    boost::timer::cpu_timer timer_internal;
    bool bBenchCommands;
    
    LogEntry( std::ostream& os, const char* pszMsg, bool bBenchCommands )
        :   os( os ),
            bBenchCommands( bBenchCommands )
    {
        if( bBenchCommands )
            os << "\n" << pszMsg << std::endl;
    }
    
    ~LogEntry()
    {
        if( bBenchCommands )
            os << timer_internal.format();
    }
};


int main( int argc, const char* argv[] )
{
    boost::timer::cpu_timer timer;
    
    bool bDebug = false;
    bool bPrintMaster = false;
    bool bPrintInstance = false;
    bool bPrintCPP = false;
    bool bLogCommands = false;
    bool bBenchCommands = false;
    bool bTestRun = false;
    bool bSaveConfig = false;
    std::string strConfigurationFile, strHost;
    Configuration config;
    
    //TODO config this from command line
    {
        namespace po = boost::program_options;
        po::variables_map vm;
        try
        {
            po::options_description desc("Allowed options");
            desc.add_options()
                ("help", "produce help message")
                
                //options
                ("debug",       po::value< bool >( &bDebug )->implicit_value( true ), 
                    "Wait at startup to allow attaching a debugger" )
                ("master",      po::value< bool >( &bPrintMaster )->implicit_value( true ),     "Print the master tree" )
                ("instance",    po::value< bool >( &bPrintInstance )->implicit_value( true ),   "Print the instance tree" )
                ("cpp",         po::value< bool >( &bPrintCPP )->implicit_value( true ),        "Print the c++" )
                ("log",         po::value< bool >( &bLogCommands )->implicit_value( true ),     "Log commands" )
                ("bench",       po::value< bool >( &bBenchCommands )->implicit_value( true ),   "Benchmark commands" )
                ("save",        po::value< bool >( &bSaveConfig )->implicit_value( true ),      "Save changes to config file" )
                ("test",        po::value< bool >( &bTestRun )->implicit_value( true ),         "Do not actually execute commands" )
        
                //config
                ("config",      po::value< std::string >( &strConfigurationFile ),              "Configuration File" )
                ("pch",         po::value< std::string >( &config.strInputPCH ),                "Include Files PCH" )
                ("dir",         po::value< boost::filesystem::path >( &config.projectFolder ),  "Project Folder" )
                ("eg",         po::value< boost::filesystem::path >( &config.EG_LIBRARY ),  "Project Folder" )
                ("host",        po::value< std::string >( &strHost ),                           "Target Host Technology" )
                ("src",         po::value< std::vector< std::string > >( &config.inputFiles ),  "input files to compile" )
            ;

            po::positional_options_description p;
            p.add( "src", -1 );

            po::store( po::command_line_parser( argc, argv).
                        options( desc ).
                        positional( p ).run(),
                        vm );
            po::notify(vm);

            if (vm.count("help"))
            {
                std::cout << desc << "\n";
                return 1;
            }
        }
        catch( std::exception& )
        {
            std::cout << "Invalid input. Type '--help' for options\n";
            return 1;
        }
    }

    if( bDebug )
    {
        char c;
        std::cin >> c;
    }
    
    try
    {
        //allow configuration file as default parameter
        if( strConfigurationFile.empty() )
        {
            config.filterFiles();
            if( !config.xmlFiles.empty() )
            {
                strConfigurationFile = 
                    config.xmlFiles.front().string();
            }
        }
        
        //if no config specified then
        if( strConfigurationFile.empty() )
        {
            //attempt to establish project name
            config.checkProjectName();
            
            if( !boost::filesystem::exists( config.EG_LIBRARY ) )
            {
                std::cout << "Cannot locate EG library is EG environment var set?: " << 
                    config.EG_LIBRARY.generic_string() << std::endl;
                return 0;
            }
            
            //load default config
            if( !boost::filesystem::exists( config.DEFAULT ) )
            {
                std::cout << "Cannot locate default configuration file: " << 
                    config.DEFAULT.generic_string() << std::endl;
                return 0;
            }
            config.load( config.DEFAULT, "project" );
    
            //collate the host settings
            if( !strHost.empty() )
                config.loadHostConfig( strHost );
            else
            {
                std::cout << "Missing host specification" << std::endl;
                return 0;
            }
            
            const boost::filesystem::path configFilePath = 
                boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( config.strProjectName + ".xml" ) );
            if( boost::filesystem::exists( configFilePath ) )
            {
                //assume existing config file
                //config.load( configFilePath );
                //std::cout << "Overwriting existing config file: " << configFilePath.generic_string() << std::endl;
                config.save( configFilePath );
                //THROW_RTE( "Config file found for project: " << 
                //    config.strProjectName << " : " << configFilePath.generic_string() );
            }
            else
            {
                //std::cout << "Generating project configuration to: " << configFilePath.generic_string() << std::endl;
                config.save( configFilePath );
            }
        }
        else
        {
            const boost::filesystem::path configFilePath = 
                boost::filesystem::edsCannonicalise(
                    boost::filesystem::absolute( strConfigurationFile ) );
                    
            //set the project name or check it matches if specified
            if( config.strProjectName.empty() )
                config.strProjectName = configFilePath.stem().string();
            else if( config.strProjectName != configFilePath.stem().string() )
            {
                std::cout << "Project name: " << config.strProjectName << " does not match config file: " << 
                    configFilePath.generic_string() << std::endl;
                return 0;
            }
            
            //if config file does not exist
            if( !boost::filesystem::exists( configFilePath ) )
            {
                //load default config
                if( !boost::filesystem::exists( config.DEFAULT ) )
                {
                    std::cout << "Cannot locate default configuration file: " << config.DEFAULT.generic_string() << std::endl;
                    return 0;
                }
                else
                {
                    //std::cout << "Loading default configuration file: " << config.DEFAULT.generic_string() << std::endl;
                }
                config.load( config.DEFAULT, "project" );
    
                //collate the host settings
                if( !strHost.empty() )
                    config.loadHostConfig( strHost );
                
                //then save
                config.save( configFilePath );
            }
            else
            {
                //load the config file
                config.load( configFilePath, "project" );
    
                //collate the host settings
                if( !strHost.empty() )
                    config.loadHostConfig( strHost );
                
                if( bSaveConfig )
                    config.save( configFilePath );
            }
        }
            
    }
    catch( std::exception& e )
    {
        std::cout << "Configuration Error: " << e.what() << std::endl;
        return 0;
    }
    
    if( config.egSourceCode.empty() )
    {
        std::cout << "Missing eg source code" << std::endl;
        return 0;
    }
    
    if( bTestRun ) bLogCommands = true;
    
    
    //create clang file manager
    clang::FileSystemOptions fileSystemOptions = { boost::filesystem::current_path().string() };
    std::shared_ptr< clang::FileManager > pFileManager =
        std::make_shared< clang::FileManager >( fileSystemOptions );
        
    //create clang diagnostics engine
    llvm::IntrusiveRefCntPtr< clang::DiagnosticOptions > pDiagnosticOptions( new clang::DiagnosticOptions() );
    llvm::IntrusiveRefCntPtr< clang::DiagnosticIDs > pDiagnosticIDs( new clang::DiagnosticIDs() );
    llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine( 
        new clang::DiagnosticsEngine( pDiagnosticIDs, pDiagnosticOptions, new EGDiagConsumer() ) );
        
    if( !boost::filesystem::exists( config.CLANG ) )
    {
        std::cout << "Could not locate clang++ in path"  << std::endl;
        return 1;
    }
    else if( bLogCommands )
    {
        std::cout << "\nUsing clang at: " << printPath( config.CLANG ) << std::endl;
    }
    
    //ParserSession
    {
        std::unique_ptr< eg::ParserSession > pParserSession;
        if( !bTestRun )
        {
            LogEntry log( std::cout, "Parsing EG source code", bBenchCommands );
            
            pParserSession = std::make_unique< eg::ParserSession >();
            pParserSession->parse( config.egSourceCode, pFileManager, pDiagnosticsEngine );
            
            //build the eg master tree
            const eg::interface::Root* pRoot = pParserSession->buildAbstractTree();
        
            //print the tree
            if( bPrintMaster )
            {
                std::string strIndent;
                pRoot->print( std::cout, strIndent );
                return 0;
            }
        
            //save the tree 
            pParserSession->store( config.getTreeFileName() );
        }
        
        if( config.strInputPCH.empty() )
        {
            LogEntry log( std::cout, "Compiling includes to pch", bBenchCommands );
            
            //generate the includes header
            if( !bTestRun )
            {
                VERIFY_RTE( pParserSession );
                std::unique_ptr< boost::filesystem::ofstream > pIncludesFileStream =
                        boost::filesystem::createNewFileStream( config.getIncludeHeader() );
                eg::generateIncludeHeader( *pIncludesFileStream, 
                    pParserSession->getTreeRoot(), 
                    config.hostIncludeSystem, 
                    config.hostIncludeUser );
            }
            
            //compile the includes header to pch file
            {
                std::ostringstream osCmd;
                config.startCommand( osCmd );
                
                osCmd << "-Xclang -emit-pch -o " << printPath( config.getIncludePCH() ) << " ";
                
                osCmd << printPath( config.getIncludeHeader() ) << " ";
                
                osCmd << "-I " << config.EG_LIBRARY.generic_string() << " ";
        
                for( const boost::filesystem::path& includeDirectory : config.cppIncludeDirectories )
                {
                    osCmd << "-I " << printPath( includeDirectory ) << " ";
                }
                
                if( bLogCommands )
                {
                    std::cout << "\n" << osCmd.str() << std::endl;
                }
                
                if( !bTestRun )
                {
                    const int iResult = boost::process::system( osCmd.str() );
                    if( iResult )
                    {
                        std::cout << "Error invoking clang++\n";
                        return iResult;
                    }
                }
            }
        }
        
        //generate the interface
        if( !bTestRun )
        {
            LogEntry log( std::cout, "Generating Interface", bBenchCommands );
            VERIFY_RTE( pParserSession );
            std::unique_ptr< boost::filesystem::ofstream > pInterfaceFileStream =
                    boost::filesystem::createNewFileStream( config.getInterfaceHeader() );
            eg::generateInterface( *pInterfaceFileStream, 
                pParserSession->getTreeRoot(), pParserSession->getIdentifiers() );
        }
    }
    
    //compile the interface to pch file
    if( !bPrintInstance )
    {
        LogEntry log( std::cout, "Compiling interface to pch", bBenchCommands );
        std::ostringstream osCmd;
        config.startCommand( osCmd );
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << printPath( config.getIncludePCH() ) << " ";
        
        osCmd << "-Xclang -emit-pch -o " << printPath( config.getInterfacePCH() ) << " ";
        osCmd << "-Xclang -egdb=" << printPath( config.getTreeFileName() ) << " ";
        
        osCmd << "-I " << config.EG_LIBRARY.generic_string() << " ";
        
        osCmd << printPath( config.getInterfaceHeader() ) << " ";
        
        if( bLogCommands )
        {
            std::cout << "\n" << osCmd.str() << std::endl;
        }
        
        if( !bTestRun )
        {
            const int iResult = boost::process::system( osCmd.str() );
            if( iResult )
            {
                std::cout << "Error invoking clang++\n";
                return iResult;
            }
        }
    }
    
    //Interface Session
    std::size_t szTotalTranslationUnits = 1U;
    std::vector< boost::filesystem::path > operationsPCHPaths, tudbPaths, egObjectFiles;
    {
        std::unique_ptr< eg::InterfaceSession > pInterfaceSession;
        if( !bTestRun )
        {
            LogEntry log( std::cout, "Performing master analysis", bBenchCommands );
            
            pInterfaceSession = std::make_unique< eg::InterfaceSession >( config.getTreeFileName() );
            
            //perform the analysis
            pInterfaceSession->linkAnalysis();
            eg::concrete::Action* pInstanceRoot = pInterfaceSession->instanceAnalysis();
            pInterfaceSession->dependencyAnalysis();
            
            pInterfaceSession->store();
            
            if( bPrintInstance )
            {
                std::string strIndent;
                pInstanceRoot->print( std::cout, strIndent );
                return 0;
            }
        }
        
        //interface session MUST NOT store beyond this point - compiler will be loaded TU analysis sessions
        
        //determine the set of translation units...
        szTotalTranslationUnits = 1U;
        
        for( std::size_t szUnitIndex = 0U; 
            szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
        {
            const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;
            
            //generate the operation code
            if( !bTestRun )
            {
                LogEntry log( std::cout, "Generating operations", bBenchCommands );
                
                VERIFY_RTE( pInterfaceSession );
                std::unique_ptr< boost::filesystem::ofstream > pOperationFileStream =
                        boost::filesystem::createNewFileStream( config.getOperationsHeader( szUnitID ) );
                eg::generateOperationSource( *pOperationFileStream, pInterfaceSession->getTreeRoot() );
            }
            
            tudbPaths.push_back( config.getTUName( szUnitID ) );
                    
            operationsPCHPaths.push_back( config.getOperationsPCH( szUnitID ) );
                
            //compile the operations to pch file
            {
                LogEntry log( std::cout, "Compiling operations to pch", bBenchCommands );
                
                std::ostringstream osCmd;
                config.startCommand( osCmd );
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << printPath( config.getIncludePCH() ) << " ";
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << printPath( config.getInterfacePCH() ) << " ";
                
                osCmd << "-Xclang -emit-pch -o " << printPath( config.getOperationsPCH( szUnitID ) ) << " ";
                osCmd << "-Xclang -egdb=" << printPath( config.getTreeFileName() ) << " ";
                
                osCmd << "-Xclang -egtu=" << printPath( config.getTUName( szUnitID ) ) << " ";
                osCmd << "-Xclang -egtuid=" << szUnitID << " ";
                
                osCmd << printPath( config.getOperationsHeader( szUnitID ) ) << " ";
                
                if( bLogCommands )
                {
                    std::cout << "\n" << osCmd.str() << std::endl;
                }
                
                if( !bTestRun )
                {
                    const int iResult = boost::process::system( osCmd.str() );
                    if( iResult )
                    {
                        std::cout << "Error invoking clang++\n";
                        return iResult;
                    }
                }
            }
        }
    }
        
    eg::IndexedFile::FileIDtoPathMap allFiles;
    allFiles.insert( std::make_pair( eg::IndexedObject::MASTER_FILE, config.getTreeFileName() ) );
    
    //load all the translation unit analysis files
    for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
    {
        allFiles.insert( std::make_pair( 
            eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex, tudbPaths[ szUnitIndex ] ) );
    }
    
    {
        //perform the full program analysis
        std::unique_ptr< eg::ImplementationSession > pImplementationSession = 
            std::make_unique< eg::ImplementationSession >( allFiles );
                
        pImplementationSession->fullProgramAnalysis();
        pImplementationSession->store( config.getAnalysisFileName() );
        
        {
            LogEntry log( std::cout, "Compiling data structures", bBenchCommands );
            std::unique_ptr< boost::filesystem::ofstream > pDataStructureFile =
                    boost::filesystem::createNewFileStream( config.getDataStructureSource() );
            eg::generateBufferStructures( *pDataStructureFile, *pImplementationSession );
        }
            
        //generate the implementation files
        for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
        {
            const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;
            //generate the implementation source code
            if( !bTestRun )
            {
                LogEntry log( std::cout, "Generating implementation", bBenchCommands );
                std::unique_ptr< boost::filesystem::ofstream > pImplFile =
                        boost::filesystem::createNewFileStream( config.getImplementationSource( szUnitID ) );
                        
                eg::generateImplementationSource( *pImplFile, *pImplementationSession, szUnitID, config.dependencies );
            }
        
            //compile the object file
            {
                LogEntry log( std::cout, "Compiling implementation", bBenchCommands );
                std::ostringstream osCmd;
                config.startCommand( osCmd );
                
                osCmd << "-c -o " << printPath( config.getObjectName( szUnitID ) ) << " ";
                if( bPrintCPP )
                {
                    osCmd << "-fsyntax-only -Xclang -ast-print ";
                }
                    
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << printPath( config.getIncludePCH() ) << " ";
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << printPath( config.getInterfacePCH() ) << " ";
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << printPath( operationsPCHPaths[ szUnitIndex ] ) << " ";
                    
                osCmd << printPath( config.getImplementationSource( szUnitID ) ) << " ";
                    
                if( bLogCommands )
                {
                    std::cout << "\n" << osCmd.str() << std::endl;
                }
                
                if( !bTestRun )
                {
                    const int iResult = boost::process::system( osCmd.str() );
                    if( iResult )
                    {
                        std::cout << "Error invoking clang++\n";
                        return iResult;
                    }
                }
                egObjectFiles.push_back( config.getObjectName( szUnitID ) );
            }
        }
        
    }
    
    //generate the host code
    if( !config.hostCmd.empty() )
    {
        try
        {
            LogEntry log( std::cout, "Generting Host", bBenchCommands );
            
            std::ostringstream osCmd;
            config.startHostCommand( osCmd );
            
            osCmd << "--program " << config.getAnalysisFileName() << " ";
            osCmd << "--log " << config.strProjectName << "_log ";
            osCmd << "--dir " << config.getIntermediateFolder().generic_string() << " ";
            
            if( bLogCommands )
            {
                std::cout << "\n" << osCmd.str() << std::endl;
            }
            
            if( !bTestRun )
            {
                const int iResult = boost::process::system( osCmd.str() );
                if( iResult )
                {
                    std::cout << "Error invoking host command\n";
                    return iResult;
                }
            }
            
        }
        catch( std::exception& e )
        {
            std::cout << "Error attempting host command: " << e.what() << std::endl;
            return 0;
        }
    }
            
    //finally compile and link executable
    {
        LogEntry log( std::cout, "Compiling and linking program", bBenchCommands );
        std::ostringstream osCmd;
        config.startCommand( osCmd, true );
        
        osCmd << "-o " << printPath( config.getProgramName() ) << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << printPath( config.getIncludePCH() ) << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << printPath( config.getInterfacePCH() ) << " ";
    
        for( const boost::filesystem::path& objFile : egObjectFiles )
        {
            osCmd << printPath( objFile ) << " ";
        }
        
        for( const boost::filesystem::path& ccpFile : config.cppSourceCode )
        {
            osCmd << printPath( ccpFile ) << " ";
        }
        
        for( const boost::filesystem::path& cppLib : config.cppLibs )
        {
            osCmd << printPath( cppLib ) << " ";
        }
        
        osCmd << "-I " << printPath( config.EG_LIBRARY ) << " ";
        osCmd << "-I " << printPath( config.getIntermediateFolder() ) << " ";
        
        for( const boost::filesystem::path& includeDirectory : config.cppIncludeDirectories )
        {
            osCmd << "-I " << printPath( includeDirectory ) << " ";
        }
            
        if( bLogCommands )
        {
            std::cout << "\n" << osCmd.str() << std::endl;
        }
        
        if( !bTestRun )
        {
            const int iResult = boost::process::system( osCmd.str() );
            if( iResult )
            {
                std::cout << "Error invoking clang++\n";
                return iResult;
            }
        }
    }
    
    if( bBenchCommands )
    {
        std::cout << "\nTotal time:\n" << timer.format() << std::endl;
    }
    
    return 0;
}
