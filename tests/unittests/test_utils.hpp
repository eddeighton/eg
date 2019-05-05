
#ifndef TEST_UTILS_04_05_2019
#define TEST_UTILS_04_05_2019

#include <boost/filesystem.hpp>

#include "io/indexed_object.hpp"

#include "common/file.hpp"

namespace testutils
{


    inline boost::filesystem::path getEnvironmentVarPath( const char* pszName )
    {
    #pragma warning( push )
    #pragma warning( disable:4996 )
        const char* pszEnvValue = std::getenv( pszName );
    #pragma warning( pop )
        
        if( pszEnvValue )
        {
            return boost::filesystem::edsCannonicalise( 
                        boost::filesystem::absolute( pszEnvValue ) );
        }
        else
        {
            THROW_RTE( "Failed to find environment variable: " << pszName );
        }
    }


    inline const boost::filesystem::path findFile( 
                    const boost::filesystem::path& dir, 
                    const std::string& strFile )
    {
        for( boost::filesystem::directory_iterator i( dir ); 
            i != boost::filesystem::directory_iterator(); ++i )
        {
            if( boost::filesystem::is_regular_file( *i ) )
            {
                if( i->path().filename() == strFile )
                    return *i;
            }
            else if( boost::filesystem::is_directory( *i ) )
            {
                findFile( *i, strFile );
            }
            else
            {
                THROW_RTE( "Bad boost filesystem code" );
            }
        }
        THROW_RTE( "Failed to locate file:" << strFile );
    }

}

#endif //TEST_UTILS_04_05_2019