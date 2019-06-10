
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

void build_parser_session( const Project& project, bool bBenchCommands, bool bLogCommands )
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
            Environment::startCompilationCommand( osCmd );
            
            osCmd << "-Xclang -emit-pch -o " << printPath( project.getIncludePCH() ) << " ";
            
            osCmd << printPath( project.getIncludeHeader() ) << " ";
            
            osCmd << "-I " << Environment::getEGLibraryInclude().generic_string() << " ";
            
            for( const boost::filesystem::path& includeDirectory : project.getIncludeDirectories() )
            {
                osCmd << "-I " << printPath( includeDirectory ) << " ";
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
}

void command_build( bool bHelp, const std::vector< std::string >& args )
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
        
        const boost::filesystem::path projectFile = projectDirectory / ".eg";
        
        if( !boost::filesystem::exists( projectFile ) )
        {
            THROW_RTE( "Could not locate .eg file in directory: " << projectDirectory.generic_string() );
        }
        
        
        
        egxml::EG_paggr eg_p;
        xml_schema::document_pimpl doc_p( eg_p.root_parser(), eg_p.root_name() );
         
        eg_p.pre();
        doc_p.parse( projectFile.string() );
        egxml::EG* pEG = eg_p.post();
        
        //std::cout << "Found eg project: " << pEG->Project().Name() << std::endl;
        
        Project project( projectDirectory, pEG->Project() );
        
        build_parser_session( project, bBenchCommands, bLogCommands );
        
        
        
        
    }
    
}
