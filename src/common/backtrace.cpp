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


/*
Copyright Deighton Systems Limited (c) 2016
*/

#include "common/backtrace.hpp"
#include "common/assert_verify.hpp"

#ifdef _DEBUG

#include <iostream>

#pragma warning( push )
#pragma warning( disable : 4091 )
#include <windows.h>
#include <Dbghelp.h>
#include <winuser.h>
#include <crtdbg.h>
#pragma warning( pop )

#pragma comment( lib, "Dbghelp.lib" )

namespace Common
{
    static bool g_enable_debug_error_prompts = true;
    
    void disableDebugErrorPrompts()
    {
        g_enable_debug_error_prompts = false;
    }

    void msvcr_debugAssert( size_t uiType, const char* pszFile, size_t uiLine, const char* pszFunction, const char* pszMsg )
    {
        if( g_enable_debug_error_prompts )
        {
            std::cout << "\n!!!!!!!!!ERROR!!!!!!!!:\n" << pszFile << ":" << uiLine << ":" << pszFunction << ":" << pszMsg;
            
            if( uiType == _CRT_ERROR )
            {
                std::ostringstream os;
                os << "File: " << pszFile << ":" << uiLine << "\n" <<
                       "Function: " << pszFunction << "\n" <<
                       pszMsg;
                std::cout << "awaiting message box response...\n";
                const int iResult = ::MessageBoxA( 0U, os.str().c_str(), "Error", MB_ABORTRETRYIGNORE | MB_ICONERROR );
                switch( iResult )
                {
                    case IDABORT:
                        std::cout << "Terminating due to debug message response..." << std::endl;
                        terminate();
                    case IDRETRY:
                        _CrtDbgBreak(); 
                    case IDIGNORE:
                    default:
                        return;
                }
            }
            else
            {
                _CrtDbgBreak(); 
            }
        }
    }

    //windows
    void getBackTrace( std::ostream& os )
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

    void debug_break()
    {
        const int iResult = ::MessageBoxA( 0U, "Debug Break called", "Error", MB_ABORTRETRYIGNORE | MB_ICONERROR );
        switch( iResult )
        {
            case IDABORT:
                terminate();
            case IDRETRY:
                _CrtDbgBreak(); 
            case IDIGNORE:
            default:
                return;
        }
    }
}

namespace boost
{
    void assertion_failed( char const * expr, char const * function, char const * file, long line )
    {
        THROW_RTE( "Boost called abort with: " << expr << " : " << function << " : " << file << ":" << line ); 
        //::abort();
    }
    void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line)
    {
        THROW_RTE( "Boost called abort with: " << expr << " : " << function << " : " << file << ":" << line << " : " << msg ); 
        //::abort();
    }
}

#else //_DEBUG

namespace Common
{
    void disableDebugErrorPrompts()
    {
    }
    
    void msvcr_debugAssert( size_t uiType, const char* pszFile, size_t uiLine, const char* pszFunction, const char* pszMsg )
    {
    }
    
    void getBackTrace( std::ostream& os )
    {
    }

    void debug_break()
    {
        
    }

}

namespace boost
{
    void assertion_failed( char const * expr, char const * function, char const * file, long line )
    {
        THROW_RTE( "Boost called abort with: " << expr << " : " << function << " : " << file << ":" << line ); 
        //::abort();
    }
    void assertion_failed_msg(char const * expr, char const * msg, char const * function, char const * file, long line)
    {
        THROW_RTE( "Boost called abort with: " << expr << " : " << function << " : " << file << ":" << line << " : " << msg ); 
        //::abort();
    }
}

#endif // _DEBUG

    
