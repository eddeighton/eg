
#ifndef PARSER_14_SEPT_2020
#define PARSER_14_SEPT_2020

#include "eg_compiler/sessions/parser_session.hpp"

#include "boost/filesystem/path.hpp"
#include <ostream>

#ifdef EG_DATABASE_SHARED_MODULE
    #define EGDB_EXPORT __declspec( dllexport )
#else
    #define EGDB_EXPORT __declspec( dllimport )
#endif

//EG Database Interface
namespace eg
{
    struct EG_PARSER_INTERFACE
    {
        virtual ~EG_PARSER_INTERFACE(){};
        
        virtual void parseEGSourceFile( const boost::filesystem::path& egSourceFile,
                    const boost::filesystem::path& cwdPath, std::ostream& osError,
                    ParserSession& session, input::Root* pRoot ) = 0;
    };
}

#endif //PARSER_14_SEPT_2020