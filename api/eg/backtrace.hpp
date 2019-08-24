

#ifndef EG_BACKTRACE_GUARD
#define EG_BACKTRACE_GUARD

#ifdef _WIN32

#include <sstream>

/*
#pragma warning( push )
#pragma warning( disable : 4091 )
#include <windows.h>
#include <Dbghelp.h>
#include <winuser.h>
#include <crtdbg.h>
#pragma warning( pop )
#pragma comment( lib, "Dbghelp.lib" )

namespace eg
{
    //windows only for now...
    inline void getBackTrace( std::ostream& os )
    {
        IMAGEHLP_LINE64 line;
        SymSetOptions(SYMOPT_LOAD_LINES);
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        
        HANDLE         process = GetCurrentProcess();
        SymInitialize( process, NULL, TRUE );
         
        void* stack[ 100U ];
        const USHORT result = CaptureStackBackTrace( 0U, 100U, stack, 0U );
        
        SYMBOL_INFO* symbol  = ( SYMBOL_INFO* )calloc( sizeof( SYMBOL_INFO ) + 1024U * sizeof( char ), 1U );
        symbol->MaxNameLen   = 1023U;
        symbol->SizeOfStruct = sizeof( SYMBOL_INFO );

        DWORD64  dwDisplacement = 0U;
        os << "\nStack Trace...\n";
        //skip this actual function
        for( USHORT ui = result - 1U; ui > 0U; --ui )
        {
            if( SymFromAddr( process, ( DWORD64 )( stack[ ui ] ), &dwDisplacement, symbol ) )
            {
                DWORD dw = 0U;
                if( SymGetLineFromAddr64( process, ( DWORD64 )( stack[ ui ] ), &dw, &line ) )
                {
                    os << ui << ": " << line.FileName << ":" << line.LineNumber << " " << symbol->Name << "\n";
                }
                else
                {
                    os << ui << ": " << symbol->Name << "\n";
                }
            }
            else
            {
                os << "SymFromAddr failed with: " << GetLastError() << "\n";
                break;
            }
        }
        os << "\n";
        
        free( symbol );
    }
}
*/
#elif
/*

#include <sstream>
#include <boost/stacktrace.hpp>

namespace eg
{
    //windows only for now...
    inline void getBackTrace( std::ostream& os )
    {
        os << boost::stacktrace::stacktrace();
    }
}
*/
#endif

#endif //EG_BACKTRACE_GUARD