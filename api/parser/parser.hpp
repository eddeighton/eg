
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
    namespace input
    {
        class Context;
        class Export;
    }
    
    struct EG_PARSER_CALLBACK
    {
        virtual void contextBody( const eg::input::Context* pContext, const char* pszBodyText ) = 0;
        virtual void exportBody( const eg::input::Export* pExport, const char* pszBodyText ) = 0;
    };
    
    struct EGDB_EXPORT EG_PARSER_INTERFACE
    {
        virtual void parseEGSourceFile( EG_PARSER_CALLBACK* pCallback,
                    const boost::filesystem::path& egSourceFile,
                    const boost::filesystem::path& cwdPath, std::ostream& osError,
                    ParserSession& session, input::Root* pRoot ) = 0;
    };
}

#endif //PARSER_14_SEPT_2020