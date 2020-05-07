

#ifndef GENERATOR_COMMON_GUARD
#define GENERATOR_COMMON_GUARD

#include <boost/filesystem.hpp>

#include <string>
#include <set>

namespace eg
{

    struct CmdLine
    {
        std::string strName = "Demo";
        boost::filesystem::path strBuildDir = boost::filesystem::current_path();
		bool bRunPythonScriptsInMainAction;
        boost::filesystem::path programDataBaseFile;
        std::string targetFileName;
    };

    CmdLine handleCommandLine( int argc, const char* argv[] );
    
}

#endif //GENERATOR_COMMON_GUARD