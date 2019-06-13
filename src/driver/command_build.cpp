
#include "project.hpp"

#include "schema.hxx"
#include "schema-pimpl.hxx"
#include "schema-simpl.hxx"

#include "eg/parser_session.hpp"
#include "eg/interface_session.hpp"
#include "eg/operations_session.hpp"
#include "eg/implementation_session.hpp"
#include "eg/codegen.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"


#pragma warning( push )
#include "common/clang_warnings.hpp"

#include "clang/Basic/FileSystemOptions.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/DiagnosticIDs.h"
#include "clang/Basic/DiagnosticOptions.h"
#include "clang/Basic/Diagnostic.h"

#pragma warning( pop ) 


#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include <boost/timer/timer.hpp>

#pragma warning( push )
#pragma warning( disable : 4996) //iterator thing
#pragma warning( disable : 4244) //conversion to DWORD from system_clock::rep
#include <boost/process.hpp>
#pragma warning( pop )

#include <iostream>
#include <memory>


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

void build_parser_session( const Environment& environment, const Project& project, bool bBenchCommands, bool bLogCommands )
{
    //create clang file manager
    clang::FileSystemOptions fileSystemOptions = { boost::filesystem::current_path().string() };
    std::shared_ptr< clang::FileManager > pFileManager =
        std::make_shared< clang::FileManager >( fileSystemOptions );
        
    //create clang diagnostics engine
    llvm::IntrusiveRefCntPtr< clang::DiagnosticOptions > pDiagnosticOptions( new clang::DiagnosticOptions() );
    llvm::IntrusiveRefCntPtr< clang::DiagnosticIDs > pDiagnosticIDs( new clang::DiagnosticIDs() );
    llvm::IntrusiveRefCntPtr< clang::DiagnosticsEngine > pDiagnosticsEngine( 
        new clang::DiagnosticsEngine( pDiagnosticIDs, pDiagnosticOptions, new EGDiagConsumer() ) );
        
    const std::vector< boost::filesystem::path > egSourceCode = project.getEGSourceCode();
    
    std::unique_ptr< eg::ParserSession > pParserSession;
    
    {
        LogEntry log( std::cout, "Parsing EG source code", bBenchCommands );
        
        pParserSession = std::make_unique< eg::ParserSession >();
        pParserSession->parse( egSourceCode, pFileManager, pDiagnosticsEngine );
        
        //build the eg master tree
        const eg::interface::Root* pRoot = pParserSession->buildAbstractTree();
    
        //save the tree 
        pParserSession->store( project.getTreeFileName() );
    }
    
    //TODO detect existing pch file here if includes.hpp has not changed...
    
    {
        LogEntry log( std::cout, "Compiling includes to pch", bBenchCommands );
        
        //generate the includes header
        {
            VERIFY_RTE( pParserSession );
            std::unique_ptr< boost::filesystem::ofstream > pIncludesFileStream =
                    boost::filesystem::createNewFileStream( project.getIncludeHeader() );
            eg::generateIncludeHeader( *pIncludesFileStream, 
                pParserSession->getTreeRoot(), 
                project.getHostSystemIncludes(), 
                project.getHostUserIncludes() );
        }
        
        //compile the includes header to pch file
        {
            std::ostringstream osCmd;
            environment.startCompilationCommand( osCmd );
            osCmd << " " << project.getCompilerFlags() << " ";
            
            osCmd << "-Xclang -emit-pch -o " << environment.printPath( project.getIncludePCH() ) << " ";
            
            osCmd << environment.printPath( project.getIncludeHeader() ) << " ";
            
            osCmd << "-I " << environment.getEGLibraryInclude().generic_string() << " ";
            
            for( const boost::filesystem::path& includeDirectory : project.getIncludeDirectories() )
            {
                osCmd << "-I " << environment.printPath( includeDirectory ) << " ";
            }
            
            if( bLogCommands )
            {
                std::cout << "\n" << osCmd.str() << std::endl;
            }
            
            {
                const int iResult = boost::process::system( osCmd.str() );
                if( iResult )
                {
                    THROW_RTE( "Error invoking clang++ " << iResult );
                }
            }
        }
    }
    
    //generate the interface
    {
        LogEntry log( std::cout, "Generating Interface", bBenchCommands );
        VERIFY_RTE( pParserSession );
        std::unique_ptr< boost::filesystem::ofstream > pInterfaceFileStream =
                boost::filesystem::createNewFileStream( project.getInterfaceHeader() );
        eg::generateInterface( *pInterfaceFileStream, 
            pParserSession->getTreeRoot(), pParserSession->getIdentifiers() );
    }
    
    {
        LogEntry log( std::cout, "Compiling interface to pch", bBenchCommands );
        std::ostringstream osCmd;
        environment.startCompilationCommand( osCmd );
        osCmd << " " << project.getCompilerFlags() << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
        
        osCmd << "-Xclang -emit-pch -o " << environment.printPath( project.getInterfacePCH() ) << " ";
        osCmd << "-Xclang -egdb=" << environment.printPath( project.getTreeFileName() ) << " ";
        
        osCmd << "-I " << environment.getEGLibraryInclude().generic_string() << " ";
        
        osCmd << environment.printPath( project.getInterfaceHeader() ) << " ";
        
        if( bLogCommands )
        {
            std::cout << "\n" << osCmd.str() << std::endl;
        }
        
        {
            const int iResult = boost::process::system( osCmd.str() );
            if( iResult )
            {
                THROW_RTE( "Error invoking clang++ " << iResult );
            }
        }
    }
}

void build_operations( const Environment& environment, const Project& project, bool bBenchCommands, bool bLogCommands )
{
    std::size_t szTotalTranslationUnits = 1U;
    //std::vector< boost::filesystem::path > operationsPCHPaths, tudbPaths, egObjectFiles;
    {
        std::unique_ptr< eg::InterfaceSession > pInterfaceSession;
        {
            LogEntry log( std::cout, "Performing master analysis", bBenchCommands );
            
            pInterfaceSession = std::make_unique< eg::InterfaceSession >( project.getTreeFileName() );
            
            //perform the analysis
            pInterfaceSession->linkAnalysis();
            eg::concrete::Action* pInstanceRoot = pInterfaceSession->instanceAnalysis();
            pInterfaceSession->dependencyAnalysis();
            
            pInterfaceSession->store();
        }
        
        //interface session MUST NOT store beyond this point - compiler will be loaded TU analysis sessions
        
        //determine the set of translation units...
        szTotalTranslationUnits = 1U;
        
        for( std::size_t szUnitIndex = 0U; 
            szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
        {
            const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;
            
            //generate the operation code
            {
                LogEntry log( std::cout, "Generating operations", bBenchCommands );
                
                VERIFY_RTE( pInterfaceSession );
                std::unique_ptr< boost::filesystem::ofstream > pOperationFileStream =
                        boost::filesystem::createNewFileStream( project.getOperationsHeader( szUnitID ) );
                eg::generateOperationSource( *pOperationFileStream, pInterfaceSession->getTreeRoot() );
            }
                
            //compile the operations to pch file
            {
                LogEntry log( std::cout, "Compiling operations to pch", bBenchCommands );
                
                std::ostringstream osCmd;
                environment.startCompilationCommand( osCmd );
                osCmd << " " << project.getCompilerFlags() << " ";
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << environment.printPath( project.getInterfacePCH() ) << " ";
                
                osCmd << "-Xclang -emit-pch -o " << environment.printPath( project.getOperationsPCH( szUnitID ) ) << " ";
                osCmd << "-Xclang -egdb=" << environment.printPath( project.getTreeFileName() ) << " ";
                
                osCmd << "-Xclang -egtu=" << environment.printPath( project.getTUName( szUnitID ) ) << " ";
                osCmd << "-Xclang -egtuid=" << szUnitID << " ";
                
                osCmd << environment.printPath( project.getOperationsHeader( szUnitID ) ) << " ";
                
                if( bLogCommands )
                {
                    std::cout << "\n" << osCmd.str() << std::endl;
                }
                
                {
                    const int iResult = boost::process::system( osCmd.str() );
                    if( iResult )
                    {
                        THROW_RTE( "Error invoking clang++ " << iResult );
                    }
                }
            }
        }
    }
}

void generate_objects( const Environment& environment, const Project& project, bool bBenchCommands, bool bLogCommands )
{
    std::size_t szTotalTranslationUnits = 1U;
    
    eg::IndexedFile::FileIDtoPathMap allFiles;
    allFiles.insert( std::make_pair( eg::IndexedObject::MASTER_FILE, project.getTreeFileName() ) );
    
    //load all the translation unit analysis files
    for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
    {
        const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;
        allFiles.insert( std::make_pair( 
            eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex, project.getTUName( szUnitID ) ) );
    }
    
    {
        //perform the full program analysis
        std::unique_ptr< eg::ImplementationSession > pImplementationSession = 
            std::make_unique< eg::ImplementationSession >( allFiles );
                
        pImplementationSession->fullProgramAnalysis();
        pImplementationSession->store( project.getAnalysisFileName() );
        
        {
            LogEntry log( std::cout, "Compiling data structures", bBenchCommands );
            std::unique_ptr< boost::filesystem::ofstream > pDataStructureFile =
                    boost::filesystem::createNewFileStream( project.getDataStructureSource() );
            eg::generateBufferStructures( *pDataStructureFile, *pImplementationSession );
        }
        
        //generate the implementation files
        for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
        {
            const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;
            //generate the implementation source code
            {
                LogEntry log( std::cout, "Generating implementation", bBenchCommands );
                std::unique_ptr< boost::filesystem::ofstream > pImplFile =
                        boost::filesystem::createNewFileStream( project.getImplementationSource( szUnitID ) );
                        
                eg::generateImplementationSource( *pImplFile, *pImplementationSession, szUnitID );
            }
        }
    }
}

void build_objects( const Environment& environment, const Project& project, bool bBenchCommands, bool bLogCommands )
{
    std::size_t szTotalTranslationUnits = 1U;
    
    for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
    {
        const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;

        //compile the object file
        LogEntry log( std::cout, "Compiling implementation", bBenchCommands );
        std::ostringstream osCmd;
        environment.startCompilationCommand( osCmd );
        osCmd << " " << project.getCompilerFlags() << " ";
        
        osCmd << "-c -o " << environment.printPath( project.getObjectName( szUnitID ) ) << " ";
            
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getInterfacePCH() ) << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getOperationsPCH( szUnitID ) ) << " ";
            
        osCmd << environment.printPath( project.getImplementationSource( szUnitID ) ) << " ";
            
        if( bLogCommands )
        {
            std::cout << "\n" << osCmd.str() << std::endl;
        }
        
        {
            const int iResult = boost::process::system( osCmd.str() );
            if( iResult )
            {
                THROW_RTE( "Error invoking clang++ " << iResult );
            }
        }
    }
}

void build_program( const Environment& environment, const Project& project, bool bBenchCommands, bool bLogCommands )
{
    std::size_t szTotalTranslationUnits = 1U;
    
    //generate the host code
    {
        LogEntry log( std::cout, "Executing Host Command", bBenchCommands );
        
        std::ostringstream osCmd;
        osCmd << project.getHostCommand() << " ";
        
        osCmd << "--program " << project.getAnalysisFileName() << " ";
        osCmd << "--log log ";
        osCmd << "--dir " << project.getIntermediateFolder().generic_string() << " ";
        
        if( bLogCommands )
        {
            std::cout << "\n" << osCmd.str() << std::endl;
        }
        
        {
            const int iResult = boost::process::system( osCmd.str() );
            if( iResult )
            {
                THROW_RTE( "Error invoking host command " << iResult );
            }
        }
    }
            
    //finally compile and link executable
    {
        LogEntry log( std::cout, "Compiling and linking program", bBenchCommands );
        std::ostringstream osCmd;
        environment.startCompilationCommand( osCmd );
        osCmd << " " << project.getCompilerFlags() << " " << project.getLinkerFlags() << " ";
        
        osCmd << "-o " << environment.printPath( project.getProgramName() ) << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getInterfacePCH() ) << " ";
    
        for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
        {
            const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;
            osCmd << environment.printPath( project.getObjectName( szUnitID ) ) << " ";
        }
        
        for( const boost::filesystem::path& strLibraryPath : project.getLibraryDirectories() )
        {
            osCmd << "-L" << environment.printPath( strLibraryPath ) << " ";
        }
        
        for( const boost::filesystem::path& strSourceFile : project.getCPPSourceCode() )
        {
            osCmd << environment.printPath( strSourceFile ) << " ";
        }
        
        for( const boost::filesystem::path& strSourceFile : project.getCPPLibs() )
        {
            osCmd << environment.printPath( strSourceFile ) << " ";
        }
        
        osCmd << "-I " << environment.printPath( environment.getEGLibraryInclude() ) << " ";
        osCmd << "-I " << environment.printPath( project.getIntermediateFolder() ) << " ";
        
        for( const boost::filesystem::path& includeDirectory : project.getIncludeDirectories() )
        {
            osCmd << "-I " << environment.printPath( includeDirectory ) << " ";
        }
            
        if( bLogCommands )
        {
            std::cout << "\n" << osCmd.str() << std::endl;
        }
        
        {
            const int iResult = boost::process::system( osCmd.str() );
            if( iResult )
            {
                THROW_RTE( "Error invoking clang++ " << iResult );
            }
        }
    }
    
}

void command_build( bool bHelp, const std::string& strBuildCommand, const std::vector< std::string >& args )
{
    std::string strDirectory;
    bool bBenchCommands = false;
    bool bLogCommands = false;
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Create Project Command");
    {
        commandOptions.add_options()
            ("dir",     po::value< std::string >( &strDirectory ), "Project directory")
            ("bench",   po::bool_switch( &bBenchCommands ), "Benchmark compilation steps" )
            ("log",   po::bool_switch( &bLogCommands ), "Log compilation commands" )
        ;
    }
    
    po::positional_options_description p;
    p.add( "dir", -1 );
    
    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );
    
    if( strBuildCommand.empty() )
    {
        std::cout << "Missing build command type" << std::endl;
        return;
    }
    
    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        const boost::filesystem::path projectDirectory = 
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strDirectory ) );

        if( !boost::filesystem::exists( projectDirectory ) )
        {
            THROW_RTE( "Specified directory does not exist: " << projectDirectory.generic_string() );
        }
        else if( !boost::filesystem::is_directory( projectDirectory ) )
        {
            THROW_RTE( "Specified path is not a directory: " << projectDirectory.generic_string() );
        }
        
        const boost::filesystem::path projectFile = projectDirectory / Environment::EG_FILE_EXTENSION;
        
        if( !boost::filesystem::exists( projectFile ) )
        {
            THROW_RTE( "Could not locate " << Environment::EG_FILE_EXTENSION << " file in directory: " << projectDirectory.generic_string() );
        }
        
        XMLManager::XMLDocPtr pDocument = XMLManager::load( projectFile );
        
        Environment environment( projectDirectory );
        
        Project project( environment, pDocument->Project(), strBuildCommand );
        
        build_parser_session( environment, project, bBenchCommands, bLogCommands );
        
        build_operations( environment, project, bBenchCommands, bLogCommands );
        
        generate_objects( environment, project, bBenchCommands, bLogCommands );
        
        build_objects( environment, project, bBenchCommands, bLogCommands );
        
        build_program( environment, project, bBenchCommands, bLogCommands );
        
    }
    
}
