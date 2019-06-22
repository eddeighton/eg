
#include "schema/project.hpp"

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"
#include "egxml/eg_schema-simpl.hxx"

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
#include <map>


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
private:
    std::ostream& os;
    const std::string msg;
    boost::timer::cpu_timer timer_internal;
    bool bBenchCommands;
public:
    LogEntry( std::ostream& os, const std::string& pszMsg, bool bBenchCommands )
        :   os( os ),
            msg( pszMsg ),
            bBenchCommands( bBenchCommands )
    {
    }
    
    ~LogEntry()
    {
        if( bBenchCommands )
            os << timer_internal.format( 3, "%w seconds" ) << ": " << msg << "\n";
    }
};

class FileWriteTracker
{
    using FileTimeMap = std::map< boost::filesystem::path, std::time_t >;
public:

    FileWriteTracker( const boost::filesystem::path& buildFolder )
    {
        if( boost::filesystem::exists( buildFolder ) )
        {
            for( boost::filesystem::directory_iterator itr( buildFolder );
                itr != boost::filesystem::directory_iterator(); ++itr )
            {
                if( boost::filesystem::is_regular_file( itr->status() ) )
                    recordFile( itr->path() );
            }
        }
    }

    bool isModified( const boost::filesystem::path& filePath ) const
    {
        FileTimeMap::const_iterator iFind = m_fileTimes.find( filePath );
        if( iFind == m_fileTimes.end() )
        {
            return true;
        }
        else
        {
            return iFind->second != boost::filesystem::last_write_time( filePath );
        }
    }
    
private:
    void recordFile( const boost::filesystem::path& filePath )
    {
        ASSERT( m_fileTimes.find( filePath ) == m_fileTimes.end() );
        m_fileTimes.insert( std::make_pair( filePath, boost::filesystem::last_write_time( filePath ) ) );
    }
    
    FileTimeMap m_fileTimes;
};


void build_parser_session( const Environment& environment, const Project& project, FileWriteTracker& fileTracker, 
    bool bBenchCommands, bool bLogCommands, bool bNoReUsePCH )
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
        pParserSession->store( project.getParserDBFileName() );
    }
    
    bool bReUsePCH = false;
    
    {
        //generate the includes header
        {
            //careful to only write to the file if it has been modified
            VERIFY_RTE( pParserSession );
            std::ostringstream osInclude;
            eg::generateIncludeHeader( osInclude, 
                pParserSession->getTreeRoot(), 
                project.getHostSystemIncludes(), 
                project.getHostUserIncludes() );
            boost::filesystem::updateFileIfChanged( project.getIncludeHeader(), osInclude.str() );
            
            if( bNoReUsePCH )
            {
                //force the file timestamp to have changed to prevent reusing the pch
                boost::filesystem::last_write_time( project.getIncludeHeader(),
                    boost::filesystem::last_write_time( project.getIncludeHeader() ) + 1 );
            }
        }
        
        if( !bNoReUsePCH && boost::filesystem::exists( project.getIncludePCH() ) )
        {
            LogEntry log( std::cout, "Testing include.pch", bBenchCommands );
            //attempt to reuse pch automagically by comparing preprocessed file

            //simply compare the file time stamps because timestamp is artificially set to the same when it corresponds
            if( boost::filesystem::last_write_time( project.getIncludeHeader() ) == 
                boost::filesystem::last_write_time( project.getIncludePCH() ) )
            {
                bReUsePCH = true;
            }
            
            /*if( boost::filesystem::exists( project.getPreprocessedFile() ) )
            {
                {
                    std::ostringstream osCmd;
                    environment.startCompilationCommand( osCmd );
                    osCmd << " " << project.getCompilerFlags() << " ";
                    
                    osCmd << environment.printPath( project.getIncludeHeader() ) << " ";
                    
                    osCmd << "-I " << environment.getEGLibraryInclude().generic_string() << " ";
                    
                    for( const boost::filesystem::path& includeDirectory : project.getIncludeDirectories() )
                    {
                        osCmd << "-I " << environment.printPath( includeDirectory ) << " ";
                    }
                    
                    osCmd << " -E -o " << environment.printPath( project.getPreprocessedCompareFile() ) << " ";
                    
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
                
                //compare the file contents
                if( boost::filesystem::compareFiles( project.getPreprocessedFile(), project.getPreprocessedCompareFile() ) )
                {
                    std::cout << "Automatically reusing include.pch precompiled header" << std::endl;
                    bUsePCH = true;
                }
            }*/
        }
        
        //generate the preprocessed file
        /*if( !bUsePCH )
        {
            LogEntry log( std::cout, "Generating include preprocessed file", bBenchCommands );
        
            std::ostringstream osCmd;
            environment.startCompilationCommand( osCmd );
            osCmd << " " << project.getCompilerFlags() << " ";
            
            osCmd << environment.printPath( project.getIncludeHeader() ) << " ";
            
            osCmd << "-I " << environment.getEGLibraryInclude().generic_string() << " ";
            
            for( const boost::filesystem::path& includeDirectory : project.getIncludeDirectories() )
            {
                osCmd << "-I " << environment.printPath( includeDirectory ) << " ";
            }
            
            osCmd << " -E -o " << environment.printPath( project.getPreprocessedFile() ) << " ";
            
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
        }*/
        
        //compile the includes header to pch file
        if( !bReUsePCH )
        {
            LogEntry log( std::cout, "Compiling include precompiled header", bBenchCommands );
            
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
                else
                {
                    //artificially set the file time stamp to match the include file
                    boost::filesystem::last_write_time( project.getIncludePCH(), 
                        boost::filesystem::last_write_time( project.getIncludeHeader() ) );
                }
            }
        }
    }
    
    //generate the interface
    {
        LogEntry log( std::cout, "Generating Interface", bBenchCommands );
        VERIFY_RTE( pParserSession );
        std::ostringstream osInterface;
        eg::generateInterface( osInterface, 
            pParserSession->getTreeRoot(), pParserSession->getIdentifiers(), project.getFiberStackSize() );
        boost::filesystem::updateFileIfChanged( project.getInterfaceHeader(), osInterface.str() );
    }
    
    if( fileTracker.isModified( project.getIncludePCH() ) ||
        fileTracker.isModified( project.getInterfaceHeader() ) ||
        fileTracker.isModified( project.getInterfaceDBFileName() ) )
    {
        {
            LogEntry log( std::cout, "Compiling interface to pch", bBenchCommands );
            std::ostringstream osCmd;
            environment.startCompilationCommand( osCmd );
            osCmd << " " << project.getCompilerFlags() << " ";
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
            
            osCmd << "-Xclang -emit-pch -o " << environment.printPath( project.getInterfacePCH() ) << " ";
            osCmd << "-Xclang -egdb=" << environment.printPath( project.getParserDBFileName() ) << " ";
            
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
        
        
        {
            LogEntry log( std::cout, "Performing interface analysis", bBenchCommands );
            
            std::unique_ptr< eg::InterfaceSession > pInterfaceSession
                = std::make_unique< eg::InterfaceSession >( project.getParserDBFileName() );
                
            //perform the analysis
            pInterfaceSession->linkAnalysis();
            pInterfaceSession->instanceAnalysis();
            pInterfaceSession->dependencyAnalysis();
            
            pInterfaceSession->store( project.getInterfaceDBFileName() );
        }
        
    }
    else
    {
        LogEntry log( std::cout, "Skipping interface compilation", bBenchCommands );
    }
}

void build_operations( const Environment& environment, const Project& project, FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands )
{
    std::size_t szTotalTranslationUnits = 1U;
    //std::vector< boost::filesystem::path > operationsPCHPaths, tudbPaths, egObjectFiles;
    {
        //std::unique_ptr< eg::InterfaceSession > pInterfaceSession
        //     = std::make_unique< eg::InterfaceSession >( project.getInterfaceDBFileName() );
        std::unique_ptr< eg::InterfaceSession > pInterfaceSession
             = std::make_unique< eg::InterfaceSession >( project.getParserDBFileName() );
        
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
                std::ostringstream osOperations;
                eg::generateOperationSource( osOperations, pInterfaceSession->getTreeRoot() );
                boost::filesystem::updateFileIfChanged( project.getOperationsHeader( szUnitID ), osOperations.str() );
            }
                
            //compile the operations to pch file
            if( fileTracker.isModified( project.getIncludePCH() ) ||
                fileTracker.isModified( project.getInterfacePCH() ) ||
                fileTracker.isModified( project.getOperationsPCH( szUnitID ) ) ||
                fileTracker.isModified( project.getOperationsHeader( szUnitID ) ) ||
                fileTracker.isModified( project.getInterfaceDBFileName() ) ||
                fileTracker.isModified( project.getTUName( szUnitID ) ) )
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
                osCmd << "-Xclang -egdb=" << environment.printPath( project.getInterfaceDBFileName() ) << " ";
                
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

void generate_objects( const Environment& environment, const Project& project, FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands )
{
    std::size_t szTotalTranslationUnits = 1U;
    
    eg::IndexedFile::FileIDtoPathMap allFiles;
    allFiles.insert( std::make_pair( eg::IndexedObject::MASTER_FILE, project.getInterfaceDBFileName() ) );
    
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
            std::ostringstream osStructures;
            eg::generateBufferStructures( osStructures, *pImplementationSession );
            boost::filesystem::updateFileIfChanged( project.getDataStructureSource(), osStructures.str() );
        }
        
        //generate the implementation files
        for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
        {
            const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;
            //generate the implementation source code
            {
                LogEntry log( std::cout, "Generating implementation", bBenchCommands );
                std::ostringstream osImpl;
                eg::generateImplementationSource( osImpl, *pImplementationSession, szUnitID );
                boost::filesystem::updateFileIfChanged( project.getImplementationSource( szUnitID ), osImpl.str() );
            }
        }
        
        //generate the runtime code
        {
            std::ostringstream osImpl;
            osImpl << "#include \"structures.hpp\"\n";
            eg::generate_dynamic_interface( osImpl, *pImplementationSession );
            eg::generateActionInstanceFunctions( osImpl, *pImplementationSession );
            boost::filesystem::updateFileIfChanged( project.getRuntimeSource(), osImpl.str() );
        }
    }
    
    
    std::ostringstream osPackages;
    bool bHasPackages = false;
    {
        const std::vector< std::string > packages = project.getPackages();
        std::copy( packages.begin(), packages.end(),
            std::ostream_iterator< std::string >( osPackages, " " ) );
        bHasPackages = !packages.empty();
    }
    
    //executing all commands
    std::vector< boost::filesystem::path > commands =  project.getCommands();
    for( const boost::filesystem::path& strCommand : commands )
    {
        std::ostringstream os;
        os << "Executing command: " << strCommand;
        LogEntry log( std::cout, os.str(), bBenchCommands );
        
        std::ostringstream osCmd;
        osCmd << strCommand << " ";
        
        osCmd << "--name " << project.getProject().Name() << " ";
        osCmd << "--database " << project.getAnalysisFileName() << " ";
        osCmd << "--dir " << project.getIntermediateFolder().generic_string() << " ";
        if( bHasPackages )
        {
            osCmd << "--package " << osPackages.str() << " ";
        }
        
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
}

void objectCompilationCommand( std::string strMsg, std::string strCommand, bool bBenchCommands, std::mutex& logMutex )
{
    boost::timer::cpu_timer timer_internal;
    
    const int iResult = boost::process::system( strCommand );
    if( iResult )
    {
        THROW_RTE( "Error invoking clang++ " << iResult );
    }
    if( bBenchCommands )
    {
        std::lock_guard g( logMutex );
        std::cout << timer_internal.format( 3, "%w seconds" ) << ": " << strMsg << "\n";
    }
}

std::vector< boost::filesystem::path > 
    build_objects( const Environment& environment, const Project& project, FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands )
{
    std::vector< boost::filesystem::path > objectFiles;
    
    std::mutex logMutex;
    std::vector< std::function< void() > > commands;
    
    std::size_t szTotalTranslationUnits = 1U;
    for( std::size_t szUnitIndex = 0U; szUnitIndex != szTotalTranslationUnits; ++szUnitIndex )
    {
        const std::size_t szUnitID = eg::IndexedObject::TU_FILES_BEGIN + szUnitIndex;

        boost::filesystem::path objectFilePath = project.getObjectName( szUnitID );
        objectFiles.push_back( objectFilePath );
        
        if( fileTracker.isModified( project.getIncludePCH() ) ||
            fileTracker.isModified( project.getInterfacePCH() ) ||
            fileTracker.isModified( project.getOperationsPCH( szUnitID ) ) ||
            fileTracker.isModified( objectFilePath ) )
        {
            std::ostringstream os;
            os << "Compiling: " << objectFilePath.generic_string();
            
            std::ostringstream osCmd;
            environment.startCompilationCommand( osCmd );
            osCmd << " " << project.getCompilerFlags() << " ";
            
            osCmd << "-c -o " << environment.printPath( objectFilePath ) << " ";
                
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
            
            commands.push_back( std::bind( objectCompilationCommand, os.str(), osCmd.str(), bBenchCommands, std::ref( logMutex ) ) );
        }
    }
    
    {
        const boost::filesystem::path strSourceFile = project.getRuntimeSource();
        boost::filesystem::path objectFilePath = project.getObjectFile( strSourceFile );
        objectFiles.push_back( objectFilePath );
        
        if( fileTracker.isModified( project.getIncludePCH() ) ||
            fileTracker.isModified( project.getInterfacePCH() ) ||
            fileTracker.isModified( strSourceFile ) ||
            fileTracker.isModified( objectFilePath ) )
        {
            std::ostringstream os;
            os << "Compiling: " << objectFilePath.generic_string();
                
            std::ostringstream osCmd;
            environment.startCompilationCommand( osCmd );
            osCmd << " " << project.getCompilerFlags() << " ";
            
            osCmd << "-c -o " << environment.printPath( objectFilePath ) << " ";
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getInterfacePCH() ) << " ";
        
            osCmd << "-I " << environment.printPath( environment.getEGLibraryInclude() ) << " ";
            osCmd << "-I " << environment.printPath( project.getIntermediateFolder() ) << " ";
            
            for( const boost::filesystem::path& includeDirectory : project.getIncludeDirectories() )
            {
                osCmd << "-I " << environment.printPath( includeDirectory ) << " ";
            }
            
            osCmd << environment.printPath( strSourceFile ) << " ";
                
            if( bLogCommands )
            {
                std::cout << "\n" << osCmd.str() << std::endl;
            }
            
            commands.push_back( std::bind( objectCompilationCommand, os.str(), osCmd.str(), bBenchCommands, std::ref( logMutex ) ) );
        }
    }
    
    {
        for( const boost::filesystem::path& strSourceFile : project.getCPPSourceCode() )
        {
            boost::filesystem::path objectFilePath = project.getObjectFile( strSourceFile );
            objectFiles.push_back( objectFilePath );
            
            if( fileTracker.isModified( project.getIncludePCH() ) ||
                fileTracker.isModified( project.getInterfacePCH() ) ||
                fileTracker.isModified( strSourceFile ) ||
                fileTracker.isModified( objectFilePath ) )
            {
                std::ostringstream os;
                os << "Compiling: " << objectFilePath.generic_string();
                    
                std::ostringstream osCmd;
                environment.startCompilationCommand( osCmd );
                osCmd << " " << project.getCompilerFlags() << " ";
                
                osCmd << "-c -o " << environment.printPath( objectFilePath ) << " ";
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
                
                osCmd << "-Xclang -include-pch ";
                osCmd << "-Xclang " << environment.printPath( project.getInterfacePCH() ) << " ";
            
                osCmd << "-I " << environment.printPath( environment.getEGLibraryInclude() ) << " ";
                osCmd << "-I " << environment.printPath( project.getIntermediateFolder() ) << " ";
                
                for( const boost::filesystem::path& includeDirectory : project.getIncludeDirectories() )
                {
                    osCmd << "-I " << environment.printPath( includeDirectory ) << " ";
                }
                
                osCmd << environment.printPath( strSourceFile ) << " ";
                    
                if( bLogCommands )
                {
                    std::cout << "\n" << osCmd.str() << std::endl;
                }
                
                commands.push_back( std::bind( objectCompilationCommand, os.str(), osCmd.str(), bBenchCommands, std::ref( logMutex ) ) );
            }
        }
    }
    
    //brute force attempt to do all compilations at once
    std::vector< std::thread > threads;
    for( std::function< void() >& compilation : commands )
    {
        threads.push_back( std::thread( compilation ) );
    }
    
    for( std::thread& th : threads )
    {
        if( th.joinable() )
            th.join();
    }
    
    return objectFiles;
}

void link_program( const Environment& environment, const Project& project, FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands, 
    const std::vector< boost::filesystem::path >& objectFiles )
{
    std::size_t szTotalTranslationUnits = 1U;
        
    //link the program
    {
        LogEntry log( std::cout, "Linking", bBenchCommands );
        
        std::ostringstream osCmd;
        environment.startCompilationCommand( osCmd );
        osCmd << " " << project.getCompilerFlags() << " " << project.getLinkerFlags() << " ";
        
        osCmd << "-o " << environment.printPath( project.getProgramName() ) << " ";
        
        for( const boost::filesystem::path& objectFile : objectFiles )
        {
            osCmd << environment.printPath( objectFile ) << " ";
        }
        
        for( const boost::filesystem::path& strLibraryPath : project.getLibraryDirectories() )
        {
            osCmd << "-L" << environment.printPath( strLibraryPath ) << " ";
        }
        
        for( const boost::filesystem::path& strSourceFile : project.getCPPLibs() )
        {
            osCmd << environment.printPath( strSourceFile ) << " ";
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
    bool bNoPCH = false;
    bool bFullRebuild = false;
    
    namespace po = boost::program_options;
    po::options_description commandOptions(" Create Project Command");
    {
        commandOptions.add_options()
            ("dir",     po::value< std::string >( &strDirectory ), "Project directory")
            ("bench",   po::bool_switch( &bBenchCommands ), "Benchmark compilation steps" )
            ("trace",   po::bool_switch( &bLogCommands ), "Trace compilation commands" )
            ("nopch",   po::bool_switch( &bNoPCH ), "Force regeneration of precompiled header file" )
            ("full",    po::bool_switch( &bFullRebuild ), "Full rebuild - do not reuse previous objects or precompiled headers" )
        ;
    }
    
    po::positional_options_description p;
    p.add( "dir", -1 );
    
    po::variables_map vm;
    po::store( po::command_line_parser( args ).options( commandOptions ).positional( p ).run(), vm );
    po::notify( vm );
    
    if( bHelp )
    {
        std::cout << commandOptions << "\n";
    }
    else
    {
        if( strBuildCommand.empty() )
        {
            std::cout << "Missing build command type" << std::endl;
            return;
        }
        
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
        
        LogEntry log( std::cout, "Total time", bBenchCommands );
        
        XMLManager::XMLDocPtr pDocument = XMLManager::load( projectFile );
        
        Environment environment;
        
        Project project( projectDirectory, environment, pDocument->Project(), strBuildCommand );
        
        if( bFullRebuild && boost::filesystem::exists( project.getIntermediateFolder() ) )
        {
            std::cout << "Removing: " << project.getIntermediateFolder().generic_string() << std::endl;
            boost::filesystem::remove_all( project.getIntermediateFolder() );
        }
        
        FileWriteTracker fileTracker( project.getIntermediateFolder() ); 
        
        build_parser_session( environment, project, fileTracker, bBenchCommands, bLogCommands, bNoPCH );
        
        build_operations( environment, project, fileTracker, bBenchCommands, bLogCommands );
        
        generate_objects( environment, project, fileTracker, bBenchCommands, bLogCommands );
        
        std::vector< boost::filesystem::path > objectFiles = 
            build_objects( environment, project, fileTracker, bBenchCommands, bLogCommands );
            
        link_program( environment, project, fileTracker, bBenchCommands, bLogCommands, objectFiles );
        
    }
    
}
