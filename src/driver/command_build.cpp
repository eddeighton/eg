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


#include "schema/project.hpp"

#include "egxml/eg_schema.hxx"
#include "egxml/eg_schema-pimpl.hxx"
#include "egxml/eg_schema-simpl.hxx"

#include "eg_compiler/sessions/parser_session.hpp"
#include "eg_compiler/sessions/interface_session.hpp"
#include "eg_compiler/sessions/operations_session.hpp"
#include "eg_compiler/sessions/implementation_session.hpp"
#include "eg_compiler/codegen/codegen.hpp"
#include "eg_compiler/codegen/instructionCodeGenerator.hpp"

#include "common/assert_verify.hpp"
#include "common/file.hpp"

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

    FileWriteTracker( const Project& project )
    {
        const boost::filesystem::path& buildFolder = project.getIntermediateFolder();
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
	
    const std::vector< boost::filesystem::path > egSourceCode = project.getEGSourceCode();
    
    std::unique_ptr< eg::ParserSession > pParserSession;
    
    bool bParserDBChanged = true;
    {
        LogEntry log( std::cout, "Parsing EG source code", bBenchCommands );
        
        //is there an old parser database
        
        std::unique_ptr< eg::IncrementalParserSession > pOldParserSession;
        if( boost::filesystem::exists( project.getParserDBFileName() ) )
        {
            //actually load the interface database here NOT the parser database
            pOldParserSession = 
                std::make_unique< eg::IncrementalParserSession >( 
                    environment.getParserDll(), boost::filesystem::current_path().string(), 
                    std::cout, project.getInterfaceDBFileName() );
        }
        
        pParserSession = std::make_unique< eg::ParserSession >( 
            environment.getParserDll(), boost::filesystem::current_path().string(), std::cout );
        pParserSession->parse( egSourceCode );
        
        //build the eg master tree
        pParserSession->buildAbstractTree();
        if( pOldParserSession && !pOldParserSession->update( *pParserSession ) )
        {
            std::cout << "Reusing old parser database" << std::endl;
            
            //rewrite the interface database where only the opaques have changed
            pOldParserSession->store( project.getInterfaceDBFileName() );
            
            pParserSession.reset( pOldParserSession.get() );
            pOldParserSession.release();
            bParserDBChanged = false;
        }
        else
        {
            //save the tree 
            pParserSession->store( project.getParserDBFileName() );
        }
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
        }
        
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
            pParserSession->getTreeRoot(), pParserSession->getIdentifiers() );
        boost::filesystem::updateFileIfChanged( project.getInterfaceHeader(), osInterface.str() );
    }
    
    if( bParserDBChanged ||
        fileTracker.isModified( project.getIncludePCH() ) ||
        fileTracker.isModified( project.getInterfaceHeader() ) )
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
            osCmd << "-Xclang -egdll=" << environment.printPath( environment.getClangPluginDll() ) << " ";
            
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
            pInterfaceSession->instanceAnalysis();
            pInterfaceSession->linkAnalysis();
            
            pInterfaceSession->translationUnitAnalysis( project.getProjectDir(), 
                [ &project ]( const std::string& strName )
                {
                    eg::IndexedObject::FileID fileID = eg::IndexedObject::NO_FILE;
                    
                    const boost::filesystem::path dbPath = project.getTUDBName( strName );
                    if( boost::filesystem::exists( dbPath ) )
                    {
                        //read the fileID out of the file
                        fileID = eg::IndexedFile::readFileID( dbPath );
                    }
                    
                    return fileID;
                }
            );
            
            pInterfaceSession->store( project.getInterfaceDBFileName() );
        }
        
    }
    else
    {
        //check translation unit file ids
        /*std::unique_ptr< eg::InterfaceSession > pInterfaceSession
             = std::make_unique< eg::InterfaceSession >( project.getInterfaceDBFileName() );
        
        pInterfaceSession->checkTranslationUnits( project.getIntermediateFolder() );
        
        pInterfaceSession->store( project.getInterfaceDBFileName() );*/
        
        
        LogEntry log( std::cout, "Skipping interface compilation", bBenchCommands );
    }
}

void build_generics( eg::InterfaceSession& session, const Environment& environment, 
    const Project& project, FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands )
{
    //generate the generics code
    {
        LogEntry log( std::cout, "Generating generics", bBenchCommands );
        std::ostringstream osImpl;
        eg::generateGenericsHeader( osImpl, session );
        boost::filesystem::updateFileIfChanged( project.getGenericsHeader(), osImpl.str() );
    }
    
    if( fileTracker.isModified( project.getIncludePCH() ) ||
        fileTracker.isModified( project.getInterfacePCH() ) ||
        fileTracker.isModified( project.getGenericsHeader() ) )
    {
        LogEntry log( std::cout, "Compiling generics to pch", bBenchCommands );
        
        std::ostringstream osCmd;
        environment.startCompilationCommand( osCmd );
        osCmd << " " << project.getCompilerFlags() << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
        
        osCmd << "-Xclang -include-pch ";
        osCmd << "-Xclang " << environment.printPath( project.getInterfacePCH() ) << " ";
        
        osCmd << "-Xclang -emit-pch -o " << environment.printPath( project.getGenericsPCH() ) << " ";
        
        osCmd << environment.printPath( project.getGenericsHeader() ) << " ";
        
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

void build_operations( eg::InterfaceSession& session, const Environment& environment, 
    const Project& project, FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands )
{
    
    //generate the generics header
    build_generics( session, environment, project, fileTracker, bBenchCommands, bLogCommands );
    
    //interface session MUST NOT store beyond this point - compiler will be loaded TU analysis sessions
    const eg::TranslationUnitAnalysis& tuAnalysis = session.getTranslationUnitAnalysis();
    for( const eg::TranslationUnit* pTranslationUnit : tuAnalysis.getTranslationUnits() )
    {
        const std::string& strTUName = pTranslationUnit->getName();
        
        //generate the operation code
        {
            LogEntry log( std::cout, "Generating operations: " + strTUName, bBenchCommands );
            std::ostringstream osOperations;
            eg::generateIncludeGuard( osOperations, "OPERATIONS" );
            eg::generateOperationSource( osOperations, session.getTreeRoot(), *pTranslationUnit );
            osOperations << "\n" << eg::pszLine << eg::pszLine;
            osOperations << "#endif\n";
            boost::filesystem::updateFileIfChanged( project.getOperationsHeader( strTUName ), osOperations.str() );
        }
            
        //compile the operations to pch file
        if( fileTracker.isModified( project.getIncludePCH() ) ||
            fileTracker.isModified( project.getInterfacePCH() ) ||
            fileTracker.isModified( project.getOperationsPCH( strTUName ) ) ||
            fileTracker.isModified( project.getOperationsHeader( strTUName ) ) ||
            /*fileTracker.isModified( project.getInterfaceDBFileName() ) || */
            fileTracker.isModified( project.getTUDBName( strTUName ) ) )
        {
            LogEntry log( std::cout, "Compiling operations to pch: " + strTUName, bBenchCommands );
            
            std::ostringstream osCmd;
            environment.startCompilationCommand( osCmd );
            osCmd << " " << project.getCompilerFlags() << " ";
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getIncludePCH() ) << " ";
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getInterfacePCH() ) << " ";
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getGenericsPCH() ) << " ";
            
            osCmd << "-Xclang -emit-pch -o " << environment.printPath( project.getOperationsPCH( strTUName ) ) << " ";
            osCmd << "-Xclang -egdb=" << environment.printPath( project.getInterfaceDBFileName() ) << " ";
            osCmd << "-Xclang -egdll=" << environment.printPath( environment.getClangPluginDll() ) << " ";
            
            osCmd << "-Xclang -egtu=" << environment.printPath( project.getTUDBName( strTUName ) ) << " ";
            osCmd << "-Xclang -egtuid=" << pTranslationUnit->getDatabaseFileID() << " ";
            
            osCmd << environment.printPath( project.getOperationsHeader( strTUName ) ) << " ";
            
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

void generate_objects( const eg::TranslationUnitAnalysis& translationUnits, const Environment& environment,
    const Project& project, eg::InstructionCodeGeneratorFactory& instructionCodeGenFactory, eg::PrinterFactory& printerFactory, 
    FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands )
{
    eg::IndexedFile::FileIDtoPathMap allFiles;
    
    //use the interface session to determine the files
    {
        allFiles.insert( std::make_pair( eg::IndexedObject::MASTER_FILE, project.getInterfaceDBFileName() ) );
        
        //load all the translation unit analysis files
        for( const eg::TranslationUnit* pTranslationUnit : translationUnits.getTranslationUnits() )
        {
            allFiles.insert( std::make_pair( pTranslationUnit->getDatabaseFileID(), project.getTUDBName( pTranslationUnit->getName() ) ) );
        }
    }
    
    {
        //perform the full program analysis
        std::unique_ptr< eg::ImplementationSession > pImplementationSession = 
            std::make_unique< eg::ImplementationSession >( allFiles );
        pImplementationSession->fullProgramAnalysis();
        pImplementationSession->store( project.getAnalysisFileName() );
	}
	
	{
		eg::ReadSession session( project.getAnalysisFileName() );
        
        {
            LogEntry log( std::cout, "Compiling data structures", bBenchCommands );
            std::ostringstream osStructures;
            eg::generateBufferStructures( osStructures, session );
            boost::filesystem::updateFileIfChanged( project.getDataStructureSource(), osStructures.str() );
        }
        
        //generate the implementation files
        for( const eg::TranslationUnit* pTranslationUnit : translationUnits.getTranslationUnits() )
        {
            //generate the implementation source code
            {
                LogEntry log( std::cout, "Generating implementation: " + pTranslationUnit->getName(), bBenchCommands );
                std::ostringstream osImpl;
                eg::generateImplementationSource( osImpl, instructionCodeGenFactory, 
                    printerFactory, session, *pTranslationUnit, { "structures.hpp" } );
                boost::filesystem::updateFileIfChanged( project.getImplementationSource( pTranslationUnit->getName() ), osImpl.str() );
            }
        }
        
        //generate the runtime code
        {
            std::ostringstream osImpl;
            osImpl << "#include \"structures.hpp\"\n";
            eg::generateAccessorFunctionImpls( osImpl, printerFactory, session );
            eg::generateActionInstanceFunctions( osImpl, printerFactory, session );
            boost::filesystem::updateFileIfChanged( project.getRuntimeSource(), osImpl.str() );
        }
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
        if( project.isPybindRequired() )
        {
            osCmd << "--python ";
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

void objectCompilationCommand( std::string strMsg, std::string strCommand, 
                                bool bBenchCommands, std::mutex& logMutex )
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

void objectCompilationCommandSetFileTIme( std::string strMsg, std::string strCommand, 
                                bool bBenchCommands, std::mutex& logMutex, 
                                const boost::filesystem::path strSourceFile, 
                                const boost::filesystem::path strObjectFile )
{
    objectCompilationCommand( strMsg, strCommand, bBenchCommands, logMutex );
    
    boost::filesystem::last_write_time( strObjectFile, 
        boost::filesystem::last_write_time( strSourceFile ) );
}

std::vector< boost::filesystem::path > 
    build_objects( const eg::TranslationUnitAnalysis& translationUnits, const Environment& environment, const Project& project,
        FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands )
{
    std::vector< boost::filesystem::path > objectFiles;
    
    std::mutex logMutex;
    std::vector< std::function< void() > > commands;
    
    for( const eg::TranslationUnit* pTranslationUnit : translationUnits.getTranslationUnits() )
    {
        boost::filesystem::path objectFilePath = project.getObjectName( pTranslationUnit->getName() );
        objectFiles.push_back( objectFilePath );
        
        if( fileTracker.isModified( project.getIncludePCH() ) ||
            fileTracker.isModified( project.getInterfacePCH() ) ||
            fileTracker.isModified( project.getOperationsPCH( pTranslationUnit->getName() ) ) ||
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
            osCmd << "-Xclang " << environment.printPath( project.getGenericsPCH() ) << " ";
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getOperationsPCH( pTranslationUnit->getName() ) ) << " ";
                
            osCmd << environment.printPath( project.getImplementationSource( pTranslationUnit->getName() ) ) << " ";
                
            if( bLogCommands )
            {
                std::cout << "\n" << osCmd.str() << std::endl;
            }
            
            commands.push_back( std::bind( objectCompilationCommand, 
                os.str(), osCmd.str(), bBenchCommands, std::ref( logMutex ) ) );
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
            
            osCmd << "-Xclang -include-pch ";
            osCmd << "-Xclang " << environment.printPath( project.getGenericsPCH() ) << " ";
        
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
            
            commands.push_back( std::bind( objectCompilationCommand, 
                os.str(), osCmd.str(), bBenchCommands, std::ref( logMutex ) ) );
        }
    }
    
    {
        for( const boost::filesystem::path& strSourceFile : project.getCPPSourceCode() )
        {
            boost::filesystem::path objectFilePath = project.getObjectFile( strSourceFile );
            objectFiles.push_back( objectFilePath );
            
            bool bObjectFileMatchesSource = false;
            if( boost::filesystem::exists( strSourceFile ) && 
                boost::filesystem::exists( objectFilePath ) &&
                ( boost::filesystem::last_write_time( strSourceFile ) == 
                    boost::filesystem::last_write_time( objectFilePath ) ) )
            {
                bObjectFileMatchesSource = true;
            }
            
            if( fileTracker.isModified( project.getIncludePCH() ) ||
                fileTracker.isModified( project.getInterfacePCH() ) ||
                !bObjectFileMatchesSource )
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
                
                commands.push_back( std::bind( objectCompilationCommandSetFileTIme, 
                    os.str(), osCmd.str(), bBenchCommands, std::ref( logMutex ), 
                    strSourceFile, objectFilePath ) );
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

void link_program( const Environment& environment, const Project& project, 
        FileWriteTracker& fileTracker, bool bBenchCommands, bool bLogCommands, 
    const std::vector< boost::filesystem::path >& objectFiles )
{
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
        
        FileWriteTracker fileTracker( project ); 
        
        build_parser_session( environment, project, fileTracker, bBenchCommands, bLogCommands, bNoPCH );
        
        std::unique_ptr< eg::InterfaceSession > pInterfaceSession
             = std::make_unique< eg::InterfaceSession >( project.getInterfaceDBFileName() );
             
        build_operations( *pInterfaceSession, environment, project, fileTracker, bBenchCommands, bLogCommands );
        
        eg::PrinterFactory::Ptr pPrinterFactory = eg::getDefaultPrinterFactory();
        eg::InstructionCodeGeneratorFactoryDefault defaultInstructionCodeGen;
        
        generate_objects( pInterfaceSession->getTranslationUnitAnalysis(), environment, project,
            defaultInstructionCodeGen, *pPrinterFactory, fileTracker, bBenchCommands, bLogCommands );
        
        std::vector< boost::filesystem::path > objectFiles = 
            build_objects( pInterfaceSession->getTranslationUnitAnalysis(), environment, project, fileTracker, bBenchCommands, bLogCommands );
            
        link_program( environment, project, fileTracker, bBenchCommands, bLogCommands, objectFiles );
        
    }
    
}
