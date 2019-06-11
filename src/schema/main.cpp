

#include "schema.hxx"
#include "schema-pimpl.hxx"
#include "schema-simpl.hxx"

#include "common/file.hpp"

#include <boost/program_options.hpp>
#include <boost/format.hpp>

#include <iostream>

int main( int argc, const char* argv[] )
{
    bool bDebug = false;
    std::vector< std::string > xmlFiles;
    
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
                ("xml",      po::value<  std::vector< std::string > >( &xmlFiles ), "XML files" )
            ;

            po::positional_options_description p;
            p.add( "xml", -1 );

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
    
    
    
    if( xmlFiles.empty() )
    {
        std::cout << "No xml files specified" << std::endl;
        return 0;
    }
    
    for( const std::string strXML : xmlFiles )
    {
        const boost::filesystem::path xmlFilePath =
            boost::filesystem::edsCannonicalise(
                boost::filesystem::absolute( strXML ) );
                
        std::cout << "parsing file: " << xmlFilePath.generic_string() << std::endl;
            
        try
        {
            test::eg_paggr eg_p;
            xml_schema::document_pimpl doc_p( eg_p.root_parser(), eg_p.root_name() );
             
            eg_p.pre();
            doc_p.parse( xmlFilePath.string() );
            test::eg* pEG = eg_p.post();

            std::cout << "host: " << pEG->host() << std::endl;
            std::cout << "folder: " << pEG->folder() << std::endl;
            
            for( const test::Config& config : pEG->stuff() )
            {
                std::cout << "config: " << config.name() << " " << config.file() << std::endl;
            }
            
            std::string str = "newconfig";
            test::name n;
            n.assign( str.begin(), str.end() );
            
            test::Config newConfig;
            newConfig.name( n );
            //newConfig.file( "newconfigfile" );
            pEG->stuff().push_back( newConfig );
            
            /*
            for( const int& i : pEG->stuff() )
            {
                std::cout << "stuff: " << i << std::endl;
            }
            
            pEG->stuff().push_back( 125 );*/
            
            pEG->folder( "somethingElse" );

            {
                test::eg_saggr newEG;
                xml_schema::document_simpl doc_s( newEG.root_serializer(), newEG.root_name() );
                newEG.pre( *pEG );
                doc_s.serialize( std::cout, xml_schema::document_simpl::pretty_print );
                newEG.post();
            }
            
        }
        catch (const xml_schema::parser_exception& e)
        {
            std::cout << xmlFilePath.generic_string() << ":" << e.line() << ":" << e.column() << ": " << e.text() << std::endl;
            return 0;
        }
        catch (const xml_schema::serializer_exception& e)
        {
            std::cout << "error: " << e.text() << std::endl;
            return 0;
        }
    }
    
    
    return 0;
}
