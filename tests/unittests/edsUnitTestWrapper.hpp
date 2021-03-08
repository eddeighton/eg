#pragma once

#ifdef _WIN32
#pragma warning( push )
#pragma warning( disable : 4996 ) //boost uuid use of std::algorithms
#pragma warning( disable : 4251 ) //needs to have dll-interface to be used by clients of class 'testing::internal::TypedTestCasePState'
#pragma warning( disable : 4275 ) //non dll-interface class 'testing::TestPartResultReporterInterface'
#endif

#include <stdexcept>
#include <list>
#include <string>
#include <sstream>
#include <fstream>
#include <iterator>
#include <memory>

#include <boost/tokenizer.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>

#include <gtest/gtest.h>

#ifdef _WIN32
#pragma warning( pop )
#endif

namespace EDUTS
{

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
struct UnitTestOptions
{
    UnitTestOptions( bool _bDebug, bool _bReport, int _iRepeats, const char* _pszFilter, const char* _pszXSL )
    :   bDebug( _bDebug ),
        bReport( _bReport ),
        iRepeats( _iRepeats ),
        strFilter( _pszFilter ? _pszFilter : "" ),
        strXSLFile( _pszXSL ? _pszXSL : "" )
    {}
    bool bDebug, bReport;
    int iRepeats;
    std::string strFilter, strXSLFile;
};

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class UnitTestResultWrapper
{
    struct TextStreamWrapper
    {
        typedef std::list< std::string > StringList;
        StringList m_lines;
        StringList::iterator m_iter;

        TextStreamWrapper( const std::string& strSource )
        {
            typedef boost::tokenizer< boost::char_separator< char > > TokenizerType;
            const boost::char_separator< char > sep( "\n" );
            TokenizerType tok( strSource, sep );

            for( TokenizerType::const_iterator tok_iter = tok.begin();
                tok_iter != tok.end(); ++tok_iter)
            {
                std::string s = *tok_iter;
                if( !s.empty() )
                    m_lines.push_back( s );
            }
            m_iter = m_lines.begin();
        }

        const char* getLine()
        {
            const char* pszString = NULL;
            if( m_iter != m_lines.end() )
            {
                pszString = m_iter->c_str();
                ++m_iter;
            }
            return pszString;
        }
    };

    struct OptionsStreamWrapper
    {
        OptionsStreamWrapper( const UnitTestOptions& options )
            :   m_options( options )
        {
            std::ostringstream os;
            os  << "Test Options...\n"
                << "Debug Mode: " << m_options.bDebug << "\n"
                << "Report: " << m_options.bReport << "\n"
                << "Repeats: " << m_options.iRepeats << "\n"
                << "Filter: " << m_options.strFilter << "\n";
            boost::scoped_ptr< TextStreamWrapper > pNewStream( new TextStreamWrapper( os.str() ) );
            m_p.swap( pNewStream );
        }

        const char* getLine() { return m_p->getLine(); }

    private:
        const UnitTestOptions m_options;
        boost::scoped_ptr< TextStreamWrapper > m_p;
    };

    const std::string m_strFileName;
    TextStreamWrapper m_report, m_cout, m_cerr;
    OptionsStreamWrapper m_settings;

public:
    UnitTestResultWrapper( const UnitTestOptions& options, const std::string& strFileName,
        const std::string& strReport, const std::string& strOut, const std::string& strErr )
        :   m_strFileName( strFileName ),
            m_report( strReport ),
            m_cout( strOut ),
            m_cerr( strErr ),
            m_settings( options )
    {
    }

    const char* getSettings()       { return m_settings.getLine();  }
    const char* getReport()         { return m_report.getLine();    }
    const char* getStandardOut()    { return m_cout.getLine();      }
    const char* getStandardErr()    { return m_cerr.getLine();      }
    const char* getResultFileName() { return m_strFileName.c_str(); }
};

static bool g_bDebugAsserts = false;

inline int eds_isalnum( int ch )
{
	return std::isalnum( ch );
}

inline std::string style_replace_non_alpha_numeric( const std::string& str, char r )
{
    std::string strResult;
    std::replace_copy_if( str.begin(), str.end(), std::back_inserter( strResult ), 
        []( const char c ){ return !eds_isalnum( c ); }, r );
    return strResult;
}

inline std::string style_date_nice()
{
    return boost::posix_time::to_simple_string(
                        boost::posix_time::second_clock::universal_time() );
}

inline std::string style_date()
{
    return  style_replace_non_alpha_numeric(
                boost::to_upper_copy( 
                    style_date_nice() ), 
                '_' );
}

/////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////
class UnitTestWrapper
{
public:
#ifdef _WIN32
    static int CrtDbgReportHook( int nRptType, char *szMsg, int *retVal )
    {
        switch( nRptType )
        {
        case _CRT_WARN:
            printf( "!!WARNING!!: %s\n", szMsg );
            std::cout << "!!WARNING!!: " << szMsg << "\n";
            break;
        case _CRT_ERROR:
            printf( "!!EXCEPTION!!: %s\n", szMsg );
            std::cout << "!!EXCEPTION!!: " << szMsg << "\n";
            break;
        case _CRT_ASSERT:
            printf( "!!ASSERT FAILURE!!: %s\n", szMsg );
            std::cout << "!!ASSERT FAILURE!!: " << szMsg << "\n";
            break;
        }
        if( g_bDebugAsserts )
        {
            *retVal = 1;
            return 0;
        }
        else
        {
            *retVal = 0; //do not invoke debugger
            return 1; //no further action required
        }
    }
#endif

    UnitTestWrapper( const UnitTestOptions& options )
        :   m_pCoutBuffer( std::cout.rdbuf() ),
            m_pCerrBuffer( std::cerr.rdbuf() ),
            m_options( options ),
            m_iResult( -1 )
    {
        //install crt hook to stop CrtDbgReportHook from code ASSERTs
        g_bDebugAsserts = m_options.bDebug;
#ifdef _WIN32
        _CrtSetReportHook2( _CRT_RPTHOOK_INSTALL, &UnitTestWrapper::CrtDbgReportHook );
#endif
        generateFileNames();
        //replace cout and cerr stream buffers with internal streams
        std::cout.rdbuf( m_coutStream.rdbuf() );
        std::cerr.rdbuf( m_cerrStream.rdbuf() );
    }

    ~UnitTestWrapper()
    {
#ifdef _WIN32
        _CrtSetReportHook2( _CRT_RPTHOOK_REMOVE, &UnitTestWrapper::CrtDbgReportHook );
#endif

        //restore stream buffers
        std::cout.rdbuf( m_pCoutBuffer );
        std::cerr.rdbuf( m_pCerrBuffer );
    }

    bool run()
    {
        initGoogleTestOptions( m_options );

        char* pszStrings[] = { "test.exe" };
        int iTotalCmdArgs = 1;
        testing::InitGoogleTest( &iTotalCmdArgs, &pszStrings[0] );

        m_iResult = RUN_ALL_TESTS();

        return ( m_iResult == 0 ) ? true : false;
    }

    std::unique_ptr< UnitTestResultWrapper > getResult()
    {
        std::ostringstream os;
        if( m_options.bReport )
        {
            //generate html
            //if( !m_options.strXSLFile.empty() )
            //    generateFile( m_strFileName, m_options.strXSLFile, m_htmlFileName );

            {
                std::ifstream testFile;
                //testFile.open( m_htmlFileName.empty() ? m_strFileName.c_str() : m_htmlFileName.c_str(), std::ifstream::in );
                testFile.open( m_strFileName.c_str(), std::ifstream::in );
                char szBuffer[ 1024 ];
                while( testFile.good() )
                {
                    testFile.getline( &szBuffer[0], 1024 );
                    os << szBuffer << "\n";
                }
            }
        }

        return std::unique_ptr< UnitTestResultWrapper >( new UnitTestResultWrapper(
            m_options,
            m_options.strXSLFile.empty() ? m_strFileName : m_htmlFileName,
            //m_strFileName,
            os.str(), m_coutStream.str(), m_cerrStream.str() ) );
    }

private:
    void generateFileNames()
    {
        //boost::mt19937 ran;
        //boost::uuids::random_generator gen;
        //const boost::uuids::uuid u = gen();
		
		const std::string strTime = style_date();

        {
            std::ostringstream os;
			
            os << "results/" << "result_" << strTime << ".xml";
            m_strFileName = os.str();
        }
        {
            std::ostringstream os;
            os << "results/" << "result_" << strTime << ".html";
            m_htmlFileName = os.str();
        }
    }

    void initGoogleTestOptions( const UnitTestOptions& options )
    {
        std::ostringstream osFileName;
        if( m_options.bReport )
            osFileName << "xml:" << m_strFileName;

        ::testing::GTEST_FLAG(throw_on_failure)             = m_options.bDebug;//false;
        ::testing::GTEST_FLAG(output)                       = osFileName.str().c_str();
        ::testing::GTEST_FLAG(print_time)                   = true;
        ::testing::GTEST_FLAG(stack_trace_depth)            = 10;
        ::testing::GTEST_FLAG(break_on_failure)             = m_options.bDebug;
        ::testing::GTEST_FLAG(catch_exceptions)             = m_options.bDebug;
        ::testing::GTEST_FLAG(show_internal_stack_frames)   = m_options.bDebug;

        ::testing::GTEST_FLAG(repeat)                       = m_options.iRepeats;
        if( !m_options.strFilter.empty() )
            ::testing::GTEST_FLAG(filter)                   = m_options.strFilter.c_str();
			
    }

private:
    std::streambuf* m_pCoutBuffer, *m_pCerrBuffer;
    std::ostringstream m_coutStream, m_cerrStream;
    std::string m_strFileName, m_htmlFileName;
    const UnitTestOptions m_options;
    int m_iResult;

};

}
